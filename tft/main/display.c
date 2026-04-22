/*
 * display.c  –  ST7789 SPI driver + GEM-style menu for TB_STv3 (ESP32)
 *
 * Matches Arduino reference sketch:
 *   tft.init(240, 320)
 *   tft.invertDisplay(false)   → ST7789_INVOFF (no inversion)
 *   tft.setRotation(1)         → landscape, MADCTL = MX|MV
 *   SPI clock 20 MHz           → valid divider at 160 MHz CPU
 */

#include "display.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "display";


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 1 – ST7789 DRIVER
 * ═══════════════════════════════════════════════════════════════════════════ */

/* ST7789 command codes */
#define ST7789_SWRST     0x01
#define ST7789_SLPOUT    0x11
#define ST7789_NORON     0x13
#define ST7789_INVOFF    0x20   /* inversion OFF – matches invertDisplay(false) */
#define ST7789_INVON     0x21
#define ST7789_DISPON    0x29
#define ST7789_CASET     0x2A
#define ST7789_RASET     0x2B
#define ST7789_RAMWR     0x2C
#define ST7789_MADCTL    0x36
#define ST7789_COLMOD    0x3A
#define ST7789_PORCTRL   0xB2
#define ST7789_GCTRL     0xB7
#define ST7789_VCOMS     0xBB
#define ST7789_LCMCTRL   0xC0
#define ST7789_VDVVRHEN  0xC2
#define ST7789_VRHS      0xC3
#define ST7789_VDVS      0xC4
#define ST7789_FRCTRL2   0xC6
#define ST7789_PWCTRL1   0xD0
#define ST7789_PVGAMCTRL 0xE0
#define ST7789_NVGAMCTRL 0xE1

/* MADCTL bits */
#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20

/*
 * Rotation table – matches Adafruit setRotation() for ST7789 240×320
 *   0 = portrait           (  0°)
 *   1 = landscape          ( 90°) ← Arduino reference uses this
 *   2 = portrait  inverted (180°)
 *   3 = landscape inverted (270°)
 */
static const uint8_t s_madctl[4] = {
    0x00,                       /* 0 – portrait            */
    MADCTL_MX | MADCTL_MV,      /* 1 – landscape           */
    MADCTL_MX | MADCTL_MY,      /* 2 – portrait  inverted  */
    MADCTL_MY | MADCTL_MV,      /* 3 – landscape inverted  */
};

/* Device struct */
struct st7789_dev {
    spi_device_handle_t spi;
    gpio_num_t          dc_gpio;
    gpio_num_t          rst_gpio;
    uint16_t            width;    /* physical panel width  (240) */
    uint16_t            height;   /* physical panel height (320) */
    uint8_t             rotation;
};

/* -------------------------------------------------------
 * Built-in 5×7 font  (ASCII 32–126)
 * Each entry is 5 column bytes; bit0 = topmost pixel row.
 * ----------------------------------------------------- */
static const uint8_t s_font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* 32 ' ' */
    {0x00,0x00,0x5F,0x00,0x00}, /* 33 !   */
    {0x00,0x07,0x00,0x07,0x00}, /* 34 "   */
    {0x14,0x7F,0x14,0x7F,0x14}, /* 35 #   */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* 36 $   */
    {0x23,0x13,0x08,0x64,0x62}, /* 37 %   */
    {0x36,0x49,0x55,0x22,0x50}, /* 38 &   */
    {0x00,0x05,0x03,0x00,0x00}, /* 39 '   */
    {0x00,0x1C,0x22,0x41,0x00}, /* 40 (   */
    {0x00,0x41,0x22,0x1C,0x00}, /* 41 )   */
    {0x08,0x2A,0x1C,0x2A,0x08}, /* 42 *   */
    {0x08,0x08,0x3E,0x08,0x08}, /* 43 +   */
    {0x00,0x50,0x30,0x00,0x00}, /* 44 ,   */
    {0x08,0x08,0x08,0x08,0x08}, /* 45 -   */
    {0x00,0x60,0x60,0x00,0x00}, /* 46 .   */
    {0x20,0x10,0x08,0x04,0x02}, /* 47 /   */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 48 0   */
    {0x00,0x42,0x7F,0x40,0x00}, /* 49 1   */
    {0x42,0x61,0x51,0x49,0x46}, /* 50 2   */
    {0x21,0x41,0x45,0x4B,0x31}, /* 51 3   */
    {0x18,0x14,0x12,0x7F,0x10}, /* 52 4   */
    {0x27,0x45,0x45,0x45,0x39}, /* 53 5   */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 54 6   */
    {0x01,0x71,0x09,0x05,0x03}, /* 55 7   */
    {0x36,0x49,0x49,0x49,0x36}, /* 56 8   */
    {0x06,0x49,0x49,0x29,0x1E}, /* 57 9   */
    {0x00,0x36,0x36,0x00,0x00}, /* 58 :   */
    {0x00,0x56,0x36,0x00,0x00}, /* 59 ;   */
    {0x08,0x14,0x22,0x41,0x00}, /* 60 <   */
    {0x14,0x14,0x14,0x14,0x14}, /* 61 =   */
    {0x00,0x41,0x22,0x14,0x08}, /* 62 >   */
    {0x02,0x01,0x51,0x09,0x06}, /* 63 ?   */
    {0x32,0x49,0x79,0x41,0x3E}, /* 64 @   */
    {0x7E,0x11,0x11,0x11,0x7E}, /* 65 A   */
    {0x7F,0x49,0x49,0x49,0x36}, /* 66 B   */
    {0x3E,0x41,0x41,0x41,0x22}, /* 67 C   */
    {0x7F,0x41,0x41,0x22,0x1C}, /* 68 D   */
    {0x7F,0x49,0x49,0x49,0x41}, /* 69 E   */
    {0x7F,0x09,0x09,0x09,0x01}, /* 70 F   */
    {0x3E,0x41,0x49,0x49,0x7A}, /* 71 G   */
    {0x7F,0x08,0x08,0x08,0x7F}, /* 72 H   */
    {0x00,0x41,0x7F,0x41,0x00}, /* 73 I   */
    {0x20,0x40,0x41,0x3F,0x01}, /* 74 J   */
    {0x7F,0x08,0x14,0x22,0x41}, /* 75 K   */
    {0x7F,0x40,0x40,0x40,0x40}, /* 76 L   */
    {0x7F,0x02,0x04,0x02,0x7F}, /* 77 M   */
    {0x7F,0x04,0x08,0x10,0x7F}, /* 78 N   */
    {0x3E,0x41,0x41,0x41,0x3E}, /* 79 O   */
    {0x7F,0x09,0x09,0x09,0x06}, /* 80 P   */
    {0x3E,0x41,0x51,0x21,0x5E}, /* 81 Q   */
    {0x7F,0x09,0x19,0x29,0x46}, /* 82 R   */
    {0x46,0x49,0x49,0x49,0x31}, /* 83 S   */
    {0x01,0x01,0x7F,0x01,0x01}, /* 84 T   */
    {0x3F,0x40,0x40,0x40,0x3F}, /* 85 U   */
    {0x1F,0x20,0x40,0x20,0x1F}, /* 86 V   */
    {0x3F,0x40,0x38,0x40,0x3F}, /* 87 W   */
    {0x63,0x14,0x08,0x14,0x63}, /* 88 X   */
    {0x07,0x08,0x70,0x08,0x07}, /* 89 Y   */
    {0x61,0x51,0x49,0x45,0x43}, /* 90 Z   */
    {0x00,0x7F,0x41,0x41,0x00}, /* 91 [   */
    {0x02,0x04,0x08,0x10,0x20}, /* 92 \   */
    {0x00,0x41,0x41,0x7F,0x00}, /* 93 ]   */
    {0x04,0x02,0x01,0x02,0x04}, /* 94 ^   */
    {0x40,0x40,0x40,0x40,0x40}, /* 95 _   */
    {0x00,0x01,0x02,0x04,0x00}, /* 96 `   */
    {0x20,0x54,0x54,0x54,0x78}, /* 97 a   */
    {0x7F,0x48,0x44,0x44,0x38}, /* 98 b   */
    {0x38,0x44,0x44,0x44,0x20}, /* 99 c   */
    {0x38,0x44,0x44,0x48,0x7F}, /* 100 d  */
    {0x38,0x54,0x54,0x54,0x18}, /* 101 e  */
    {0x08,0x7E,0x09,0x01,0x02}, /* 102 f  */
    {0x08,0x14,0x54,0x54,0x3C}, /* 103 g  */
    {0x7F,0x08,0x04,0x04,0x78}, /* 104 h  */
    {0x00,0x44,0x7D,0x40,0x00}, /* 105 i  */
    {0x20,0x40,0x44,0x3D,0x00}, /* 106 j  */
    {0x7F,0x10,0x28,0x44,0x00}, /* 107 k  */
    {0x00,0x41,0x7F,0x40,0x00}, /* 108 l  */
    {0x7C,0x04,0x18,0x04,0x78}, /* 109 m  */
    {0x7C,0x08,0x04,0x04,0x78}, /* 110 n  */
    {0x38,0x44,0x44,0x44,0x38}, /* 111 o  */
    {0x7C,0x14,0x14,0x14,0x08}, /* 112 p  */
    {0x08,0x14,0x14,0x18,0x7C}, /* 113 q  */
    {0x7C,0x08,0x04,0x04,0x08}, /* 114 r  */
    {0x48,0x54,0x54,0x54,0x20}, /* 115 s  */
    {0x04,0x3F,0x44,0x40,0x20}, /* 116 t  */
    {0x3C,0x40,0x40,0x20,0x7C}, /* 117 u  */
    {0x1C,0x20,0x40,0x20,0x1C}, /* 118 v  */
    {0x3C,0x40,0x30,0x40,0x3C}, /* 119 w  */
    {0x44,0x28,0x10,0x28,0x44}, /* 120 x  */
    {0x0C,0x50,0x50,0x50,0x3C}, /* 121 y  */
    {0x44,0x64,0x54,0x4C,0x44}, /* 122 z  */
    {0x00,0x08,0x36,0x41,0x00}, /* 123 {  */
    {0x00,0x00,0x7F,0x00,0x00}, /* 124 |  */
    {0x00,0x41,0x36,0x08,0x00}, /* 125 }  */
    {0x08,0x08,0x2A,0x1C,0x08}, /* 126 ~  */
};

/* -------------------------------------------------------
 * Low-level SPI helpers
 * ----------------------------------------------------- */
static void _cmd(struct st7789_dev *dev, uint8_t cmd)
{
    gpio_set_level(dev->dc_gpio, 0);
    spi_transaction_t t = { .length = 8, .tx_buffer = &cmd };
    spi_device_polling_transmit(dev->spi, &t);
}

static void _data(struct st7789_dev *dev, const uint8_t *data, size_t len)
{
    if (!len) return;
    gpio_set_level(dev->dc_gpio, 1);
    spi_transaction_t t = { .length = len * 8, .tx_buffer = data };
    spi_device_polling_transmit(dev->spi, &t);
}

static inline void _data8(struct st7789_dev *dev, uint8_t d) { _data(dev, &d, 1); }

static void _set_window(struct st7789_dev *dev,
                        uint16_t x0, uint16_t y0,
                        uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    _cmd(dev, ST7789_CASET);
    buf[0]=x0>>8; buf[1]=x0&0xFF; buf[2]=x1>>8; buf[3]=x1&0xFF;
    _data(dev, buf, 4);

    _cmd(dev, ST7789_RASET);
    buf[0]=y0>>8; buf[1]=y0&0xFF; buf[2]=y1>>8; buf[3]=y1&0xFF;
    _data(dev, buf, 4);

    _cmd(dev, ST7789_RAMWR);
}

/* -------------------------------------------------------
 * st7789_init
 *   Clock fixed at 20 MHz – valid divider (÷8) at 160 MHz CPU.
 *   invertDisplay(false) → ST7789_INVOFF, matching Arduino ref.
 * ----------------------------------------------------- */
st7789_handle_t st7789_init(const st7789_config_t *cfg)
{
    struct st7789_dev *dev = calloc(1, sizeof(*dev));
    dev->dc_gpio  = cfg->dc_gpio;
    dev->rst_gpio = cfg->rst_gpio;
    dev->width    = cfg->width;
    dev->height   = cfg->height;
    dev->rotation = cfg->rotation;

    /* DC and RST as outputs */
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << cfg->dc_gpio) | (1ULL << cfg->rst_gpio),
        .mode         = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io);

    /* Add SPI device – 20 MHz is a clean ÷8 of 160 MHz CPU clock */
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 20 * 1000 * 1000,   /* ← was 40 MHz, caused abort */
        .mode           = 0,
        .spics_io_num   = cfg->cs_gpio,
        .queue_size     = 7,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(cfg->spi_host, &dev_cfg, &dev->spi));

    /* Hardware reset */
    gpio_set_level(dev->rst_gpio, 0); vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(dev->rst_gpio, 1); vTaskDelay(pdMS_TO_TICKS(120));

    /* Init sequence */
    _cmd(dev, ST7789_SWRST);                                     vTaskDelay(pdMS_TO_TICKS(150));
    _cmd(dev, ST7789_SLPOUT);                                    vTaskDelay(pdMS_TO_TICKS(10));
    _cmd(dev, ST7789_COLMOD); _data8(dev, 0x55);                 /* 16-bit colour */

    _cmd(dev, ST7789_PORCTRL);
    { uint8_t d[]={0x0C,0x0C,0x00,0x33,0x33}; _data(dev,d,5); }

    _cmd(dev, ST7789_GCTRL);    _data8(dev, 0x35);
    _cmd(dev, ST7789_VCOMS);    _data8(dev, 0x19);
    _cmd(dev, ST7789_LCMCTRL);  _data8(dev, 0x2C);
    _cmd(dev, ST7789_VDVVRHEN); _data8(dev, 0x01);
    _cmd(dev, ST7789_VRHS);     _data8(dev, 0x12);
    _cmd(dev, ST7789_VDVS);     _data8(dev, 0x20);
    _cmd(dev, ST7789_FRCTRL2);  _data8(dev, 0x0F);
    _cmd(dev, ST7789_PWCTRL1);  { uint8_t d[]={0xA4,0xA1}; _data(dev,d,2); }

    _cmd(dev, ST7789_PVGAMCTRL);
    { uint8_t d[]={0xD0,0x04,0x0D,0x11,0x13,0x2B,0x3F,0x54,0x4C,0x18,0x0D,0x0B,0x1F,0x23};
      _data(dev,d,14); }

    _cmd(dev, ST7789_NVGAMCTRL);
    { uint8_t d[]={0xD0,0x04,0x0C,0x11,0x13,0x2C,0x3F,0x44,0x51,0x2F,0x1F,0x1F,0x20,0x23};
      _data(dev,d,14); }

    /* invertDisplay(false) – matches Arduino reference sketch */
    _cmd(dev, ST7789_INVOFF);
    _cmd(dev, ST7789_NORON);

    /* setRotation(rotation) via MADCTL */
    _cmd(dev, ST7789_MADCTL);
    _data8(dev, s_madctl[dev->rotation & 3]);

    _cmd(dev, ST7789_DISPON); vTaskDelay(pdMS_TO_TICKS(100));

    st7789_fill_screen(dev, ST7789_BLACK);
    ESP_LOGI(TAG, "ST7789 ready (%dx%d rot=%d)", cfg->width, cfg->height, cfg->rotation);
    return dev;
}

/* -------------------------------------------------------
 * Logical dimensions (swap w/h for landscape rotations 1 & 3)
 * ----------------------------------------------------- */
uint16_t st7789_width (st7789_handle_t dev) { return (dev->rotation & 1) ? dev->height : dev->width;  }
uint16_t st7789_height(st7789_handle_t dev) { return (dev->rotation & 1) ? dev->width  : dev->height; }

/* -------------------------------------------------------
 * st7789_fill_rect
 * ----------------------------------------------------- */
void st7789_fill_rect(st7789_handle_t dev,
                      uint16_t x, uint16_t y,
                      uint16_t w, uint16_t h,
                      uint16_t color)
{
    if (!w || !h) return;

    uint16_t lw = st7789_width(dev), lh = st7789_height(dev);
    if (x >= lw || y >= lh) return;
    if (x + w > lw) w = lw - x;
    if (y + h > lh) h = lh - y;

    uint16_t px0, py0, px1, py1;
    switch (dev->rotation & 3) {
        case 1:
            px0=y;                        py0=dev->width-1-(x+w-1);
            px1=y+h-1;                    py1=dev->width-1-x;
            break;
        case 2:
            px0=dev->width-1-(x+w-1);    py0=dev->height-1-(y+h-1);
            px1=dev->width-1-x;          py1=dev->height-1-y;
            break;
        case 3:
            px0=dev->height-1-(y+h-1);   py0=x;
            px1=dev->height-1-y;         py1=x+w-1;
            break;
        default:
            px0=x; py0=y; px1=x+w-1; py1=y+h-1;
            break;
    }

    _set_window(dev, px0, py0, px1, py1);

    uint32_t total    = (uint32_t)w * h;
    uint16_t color_be = (color >> 8) | (color << 8);
    const uint32_t CHUNK = 1024;
    uint8_t *buf = malloc(CHUNK * 2);
    if (!buf) return;
    for (uint32_t i = 0; i < CHUNK; i++) { buf[i*2]=color_be>>8; buf[i*2+1]=color_be&0xFF; }

    gpio_set_level(dev->dc_gpio, 1);
    while (total > 0) {
        uint32_t batch = (total < CHUNK) ? total : CHUNK;
        spi_transaction_t t = { .length = batch * 16, .tx_buffer = buf };
        spi_device_polling_transmit(dev->spi, &t);
        total -= batch;
    }
    free(buf);
}

/* -------------------------------------------------------
 * st7789_fill_screen
 * ----------------------------------------------------- */
void st7789_fill_screen(st7789_handle_t dev, uint16_t color)
{
    st7789_fill_rect(dev, 0, 0, st7789_width(dev), st7789_height(dev), color);
}

/* -------------------------------------------------------
 * st7789_draw_char  (5×7 glyph, software scaled)
 * ----------------------------------------------------- */
uint16_t st7789_draw_char(st7789_handle_t dev,
                          uint16_t x, uint16_t y,
                          char c,
                          uint16_t fg, uint16_t bg,
                          uint8_t scale)
{
    if (c < 32 || c > 126) c = '?';
    const uint8_t *glyph = s_font5x7[(uint8_t)c - 32];
    for (int col = 0; col < 5; col++) {
        uint8_t bits = glyph[col];
        for (int row = 0; row < 7; row++) {
            st7789_fill_rect(dev,
                             x + col * scale,
                             y + row * scale,
                             scale, scale,
                             (bits & (1 << row)) ? fg : bg);
        }
    }
    st7789_fill_rect(dev, x + 5 * scale, y, scale, 7 * scale, bg);
    return x + 6 * scale;
}

/* -------------------------------------------------------
 * st7789_draw_string
 * ----------------------------------------------------- */
void st7789_draw_string(st7789_handle_t dev,
                        uint16_t x, uint16_t y,
                        const char *str,
                        uint16_t fg, uint16_t bg,
                        uint8_t scale)
{
    uint16_t cx = x, lw = st7789_width(dev);
    while (*str) {
        if (cx + 6 * scale > lw) { cx = x; y += 8 * scale; }
        cx = st7789_draw_char(dev, cx, y, *str++, fg, bg, scale);
    }
}


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 1b – lcd_print
 * ═══════════════════════════════════════════════════════════════════════════ */

/*
 * st7789_set_rotation  (internal helper)
 *   Updates MADCTL and swaps the stored logical dimensions so that
 *   st7789_width()/height() return correct values for the new orientation.
 */
static void _st7789_set_rotation(struct st7789_dev *dev, uint8_t rot)
{
    dev->rotation = rot & 3;
    _cmd(dev, ST7789_MADCTL);
    _data8(dev, s_madctl[dev->rotation]);
}

/*
 * lcd_print
 *   All-in-one print function.  Sets rotation, fills screen, then draws
 *   the text string and (optionally) the integer value.
 *
 *   Parameters come from lcd_print_config_t – see display.h for field docs.
 */
 void lcd_print(st7789_handle_t dev, const lcd_print_config_t *cfg)
{
    if (!dev || !cfg) return;

    /* 1. Apply rotation */
    _st7789_set_rotation(dev, cfg->rotation);

    /* 2. Full-screen fill */
    st7789_fill_screen(dev, cfg->fill_color);

    /* 3. Build the display string:
     *      show_value=false  →  just cfg->text
     *      show_value=true   →  "<text>: <value>"
     */
    char buf[64];
    if (cfg->show_value) {
        snprintf(buf, sizeof(buf), "%s: %d",
                 cfg->text ? cfg->text : "", cfg->value);
    } else {
        snprintf(buf, sizeof(buf), "%s",
                 cfg->text ? cfg->text : "");
    }

    /* 4. Draw string at requested position with requested pixel scale */
    st7789_draw_string(dev,
                       cfg->x, cfg->y,
                       buf,
                       cfg->fg, cfg->bg,
                       cfg->scale);
}


/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 2 – GEM-STYLE MENU
 * ═══════════════════════════════════════════════════════════════════════════ */

#define FONT_SCALE   2
#define CHAR_W       (6 * FONT_SCALE)
#define CHAR_H       (8 * FONT_SCALE)
#define MARGIN_X     4
#define MARGIN_Y     4
#define HEADER_H     (CHAR_H + MARGIN_Y * 2)
#define ROW_H        (CHAR_H + 6)

#define HEADER_FG    ST7789_WHITE
#define HEADER_BG    ST7789_COLOR(0, 0, 160)
#define ITEM_FG      ST7789_WHITE
#define ITEM_BG      ST7789_BLACK
#define SEL_FG       ST7789_BLACK
#define SEL_BG       ST7789_CYAN
#define EDIT_BG      ST7789_BLUE

struct gem_menu_ctx {
    st7789_handle_t  display;
    gem_item_t      *items;
    int              count;
    int              selected;
    bool             editing;
};

gem_menu_t *gem_menu_create(st7789_handle_t display)
{
    struct gem_menu_ctx *m = calloc(1, sizeof(*m));
    m->display  = display;
    m->selected = 0;
    m->editing  = false;
    return m;
}

void gem_menu_set_items(gem_menu_t *menu, gem_item_t *items, int count)
{
    menu->items   = items;
    menu->count   = count;
    if (menu->selected >= count) menu->selected = 0;
}

static void _menu_draw_row(gem_menu_t *menu, int idx)
{
    st7789_handle_t dsp = menu->display;
    gem_item_t     *it  = &menu->items[idx];
    bool            sel = (idx == menu->selected);

    uint16_t fg = sel ? SEL_FG : ITEM_FG;
    uint16_t bg = sel ? SEL_BG : ITEM_BG;
    uint16_t y  = HEADER_H + 2 + idx * ROW_H;
    uint16_t lw = st7789_width(dsp);
    uint16_t ty = y + (ROW_H - CHAR_H) / 2;

    st7789_fill_rect(dsp, 0, y, lw, ROW_H, bg);

    if (sel && !menu->editing)
        st7789_draw_char(dsp, MARGIN_X, ty, '>', fg, bg, FONT_SCALE);

    uint16_t x_label = MARGIN_X + CHAR_W + 2;
    st7789_draw_string(dsp, x_label, ty, it->label, fg, bg, FONT_SCALE);

    char val[24] = {0};
    switch (it->type) {
        case GEM_ITEM_INT:
            snprintf(val, sizeof(val), "%d", *it->int_val);
            if (sel && menu->editing) {
                uint16_t vx = lw - (uint16_t)strlen(val) * CHAR_W - MARGIN_X;
                st7789_fill_rect(dsp, vx - 2, y, lw - (vx - 2), ROW_H, EDIT_BG);
                st7789_draw_string(dsp, vx, ty, val, ST7789_WHITE, EDIT_BG, FONT_SCALE);
                return;
            }
            break;
        case GEM_ITEM_BOOL:
            snprintf(val, sizeof(val), *it->bool_val ? "[ON] " : "[OFF]");
            break;
        case GEM_ITEM_BUTTON:
            snprintf(val, sizeof(val), "[BTN]");
            break;
    }
    uint16_t vx = lw - (uint16_t)strlen(val) * CHAR_W - MARGIN_X;
    st7789_draw_string(dsp, vx, ty, val, fg, bg, FONT_SCALE);
}

void gem_menu_draw(gem_menu_t *menu)
{
    st7789_handle_t dsp = menu->display;
    uint16_t lw  = st7789_width(dsp);
    uint16_t lh  = st7789_height(dsp);

    st7789_fill_screen(dsp, ITEM_BG);

    st7789_fill_rect(dsp, 0, 0, lw, HEADER_H, HEADER_BG);
    st7789_draw_string(dsp, MARGIN_X, MARGIN_Y, "Main Menu",
                       HEADER_FG, HEADER_BG, FONT_SCALE);

    st7789_fill_rect(dsp, 0, HEADER_H, lw, 2, ST7789_WHITE);

    for (int i = 0; i < menu->count; i++) _menu_draw_row(menu, i);

    uint16_t used = HEADER_H + 2 + menu->count * ROW_H;
    if (used < lh) st7789_fill_rect(dsp, 0, used, lw, lh - used, ITEM_BG);
}

void gem_menu_key(gem_menu_t *menu, gem_key_t key)
{
    int        prev = menu->selected;
    gem_item_t *it  = &menu->items[menu->selected];

    if (menu->editing) {
        switch (key) {
            case GEM_KEY_RIGHT: (*it->int_val) += 1;  _menu_draw_row(menu, menu->selected); break;
            case GEM_KEY_LEFT:  (*it->int_val) -= 1;  _menu_draw_row(menu, menu->selected); break;
            case GEM_KEY_UP:    (*it->int_val) += 10; _menu_draw_row(menu, menu->selected); break;
            case GEM_KEY_DOWN:  (*it->int_val) -= 10; _menu_draw_row(menu, menu->selected); break;
            case GEM_KEY_OK:
            case GEM_KEY_CANCEL:
                menu->editing = false;
                _menu_draw_row(menu, menu->selected);
                break;
            default: break;
        }
        return;
    }

    switch (key) {
        case GEM_KEY_UP:
            if (menu->selected > 0) menu->selected--;
            break;
        case GEM_KEY_DOWN:
            if (menu->selected < menu->count - 1) menu->selected++;
            break;
        case GEM_KEY_OK:
            switch (it->type) {
                case GEM_ITEM_INT:    menu->editing = true; _menu_draw_row(menu, menu->selected); return;
                case GEM_ITEM_BOOL:   *it->bool_val = !(*it->bool_val); _menu_draw_row(menu, menu->selected); return;
                case GEM_ITEM_BUTTON: if (it->callback) it->callback(); return;
            }
            break;
        default: break;
    }

    if (menu->selected != prev) {
        _menu_draw_row(menu, prev);
        _menu_draw_row(menu, menu->selected);
    }
}