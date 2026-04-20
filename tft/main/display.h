#pragma once
/*
 * display.h  –  ST7789 driver + GEM-style menu for TB_STv3 (ESP32)
 *
 * Pin mapping (ESP32_Display.SchDoc):
 *   IO5  - TFT_BLK  (Backlight)
 *   IO18 - TFT_SCLK (SPI CLK)
 *   IO23 - TFT_MOSI (SPI MOSI)
 *   IO15 - TFT_DC   (Data/Cmd)
 *   IO2  - TFT_CS   (Chip Select)
 *   IO26 - TFT_RST  (Reset)
 */

#include <stdint.h>
#include <stdbool.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════
 * SECTION 1 – ST7789 DRIVER
 * ═══════════════════════════════════════════════════════ */

/* RGB565 colour helpers */
#define ST7789_COLOR(r,g,b) \
    ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3)))

#define ST7789_BLACK   0x0000
#define ST7789_WHITE   0xFFFF
#define ST7789_RED     0xF800
#define ST7789_GREEN   0x07E0
#define ST7789_BLUE    0x001F
#define ST7789_CYAN    0x07FF
#define ST7789_MAGENTA 0xF81F
#define ST7789_YELLOW  0xFFE0

/* ST7789 configuration */
typedef struct {
    spi_host_device_t spi_host;
    gpio_num_t        cs_gpio;    /* IO2  */
    gpio_num_t        dc_gpio;    /* IO15 */
    gpio_num_t        rst_gpio;   /* IO26 */
    uint16_t          width;      /* 240  */
    uint16_t          height;     /* 320  */
    uint8_t           rotation;   /* 0-3  */
} st7789_config_t;

typedef struct st7789_dev *st7789_handle_t;

/* ST7789 API */
st7789_handle_t st7789_init       (const st7789_config_t *cfg);
void            st7789_fill_rect  (st7789_handle_t dev, uint16_t x, uint16_t y,
                                   uint16_t w, uint16_t h, uint16_t color);
void            st7789_fill_screen(st7789_handle_t dev, uint16_t color);
uint16_t        st7789_draw_char  (st7789_handle_t dev, uint16_t x, uint16_t y,
                                   char c, uint16_t fg, uint16_t bg, uint8_t scale);
void            st7789_draw_string(st7789_handle_t dev, uint16_t x, uint16_t y,
                                   const char *str, uint16_t fg, uint16_t bg,
                                   uint8_t scale);
uint16_t        st7789_width      (st7789_handle_t dev);
uint16_t        st7789_height     (st7789_handle_t dev);


/* ═══════════════════════════════════════════════════════
 * SECTION 1b – lcd_print  (simple one-call text printer)
 *
 * Prints a string + optional int value onto a solid-colour
 * background with full control over every visual parameter.
 *
 * Usage example:
 *
 *   lcd_print_config_t cfg = {
 *       .text       = "Temp:",
 *       .value      = 42,          // appended as " 42"  (ignored if show_value=false)
 *       .show_value = true,
 *       .fg         = ST7789_WHITE,
 *       .bg         = ST7789_BLUE,
 *       .fill_color = ST7789_BLACK, // full-screen background
 *       .x          = 10,
 *       .y          = 60,
 *       .scale      = 3,           // pixel size: 1=tiny … 8=huge
 *       .rotation   = 1,           // 0-3, applied before drawing
 *   };
 *   lcd_print(display, &cfg);
 *
 * ═══════════════════════════════════════════════════════ */

typedef struct {
    const char *text;        /* string to display                          */
    int         value;       /* integer appended after text (e.g. ": 42") */
    bool        show_value;  /* set false to show text only                */
    uint16_t    fg;          /* text foreground colour  (RGB565)           */
    uint16_t    bg;          /* text background colour  (RGB565)           */
    uint16_t    fill_color;  /* full-screen fill before drawing            */
    uint16_t    x;           /* top-left x of text (logical coords)       */
    uint16_t    y;           /* top-left y of text (logical coords)       */
    uint8_t     scale;       /* pixel size multiplier: 1–8                */
    uint8_t     rotation;    /* screen orientation: 0=portrait … 3=land'  */
} lcd_print_config_t;

/*
 * lcd_print
 *   1. Sets screen rotation.
 *   2. Fills the entire screen with cfg->fill_color.
 *   3. Draws cfg->text at (cfg->x, cfg->y) using cfg->fg/bg and cfg->scale.
 *   4. If cfg->show_value is true, appends ": <value>" on the same line
 *      (wraps to next line automatically if it overflows screen width).
 */
void lcd_print(st7789_handle_t dev, const lcd_print_config_t *cfg);

/* ═══════════════════════════════════════════════════════
 * SECTION 2 – GEM-STYLE MENU
 * ═══════════════════════════════════════════════════════ */

/* Key codes */
typedef enum {
    GEM_KEY_UP     = 0,
    GEM_KEY_DOWN,
    GEM_KEY_LEFT,
    GEM_KEY_RIGHT,
    GEM_KEY_OK,
    GEM_KEY_CANCEL,
} gem_key_t;

/* Menu item types – mirrors Arduino GEM library */
typedef enum {
    GEM_ITEM_INT    = 0,
    GEM_ITEM_BOOL,
    GEM_ITEM_BUTTON,
} gem_item_type_t;

typedef void (*gem_button_cb_t)(void);

typedef struct {
    gem_item_type_t type;
    const char     *label;
    union {
        int             *int_val;
        bool            *bool_val;
        gem_button_cb_t  callback;
    };
} gem_item_t;

/* Inline constructors – mirror Arduino GEMItem ctors */
static inline gem_item_t gem_item_int(const char *label, int *val)
{
    gem_item_t it = { .type = GEM_ITEM_INT, .label = label };
    it.int_val = val;
    return it;
}

static inline gem_item_t gem_item_bool(const char *label, bool *val)
{
    gem_item_t it = { .type = GEM_ITEM_BOOL, .label = label };
    it.bool_val = val;
    return it;
}

static inline gem_item_t gem_item_button(const char *label, gem_button_cb_t cb)
{
    gem_item_t it = { .type = GEM_ITEM_BUTTON, .label = label };
    it.callback = cb;
    return it;
}

/* Menu handle (opaque) */
typedef struct gem_menu_ctx gem_menu_t;

/* Menu API */
gem_menu_t *gem_menu_create   (st7789_handle_t display);
void        gem_menu_set_items(gem_menu_t *menu, gem_item_t *items, int count);
void        gem_menu_draw     (gem_menu_t *menu);
void        gem_menu_key      (gem_menu_t *menu, gem_key_t key);

#ifdef __cplusplus
}
#endif