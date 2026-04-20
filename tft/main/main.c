/*
 * main.c  –  TB_STv3 ESP32 + ST7789 240×320
 *
 * Demonstrates lcd_print() – a single-call text printer that accepts:
 *   - text string
 *   - integer value  (optional, appended as ": <n>")
 *   - foreground colour
 *   - background colour  (behind text only)
 *   - full-screen fill colour
 *   - screen orientation  (0-3)
 *   - pixel scale  (1 = tiny  …  8 = huge)
 *   - x / y position
 *
 * Also boots the GEM-style menu (same items as Arduino reference sketch).
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "display.h"

static const char *TAG = "main";

/* ───────── PIN CONFIG ───────── */
#define TFT_BLK   5
#define TFT_SCLK  18
#define TFT_MOSI  23
#define TFT_DC    15
#define TFT_CS    2
#define TFT_RST   26
// #define TFT_RST   20

/* ───────── Menu state (mirrors Arduino reference) ───────── */
static int  number      = -512;
static bool enablePrint = false;
static gem_menu_t   *menu = NULL;
static gem_item_t    menu_items[3];

static void printData(void)
{
    if (enablePrint) {
        ESP_LOGI(TAG, "Number is: %d", number);
    } else {
        ESP_LOGI(TAG, "Printing is disabled, sorry:(");
    }
}

static void setup_menu(st7789_handle_t display)
{
    menu_items[0] = gem_item_int   ("Number:",       &number);
    menu_items[1] = gem_item_bool  ("Enable print:", &enablePrint);
    menu_items[2] = gem_item_button("Print",          printData);

    menu = gem_menu_create(display);
    gem_menu_set_items(menu, menu_items, 3);
}

/* ═══════════════════════════════════════════════════════════════════════
 * demo_lcd_print
 *
 * Cycles through several lcd_print() calls to show every parameter.
 * Each call is held for 2 seconds so you can read the screen.
 * ═══════════════════════════════════════════════════════════════════════ */
static void demo_lcd_print(st7789_handle_t display)
{
    // /* ── 1. String only, portrait, scale 4, white on blue bg, black screen ── */
    // lcd_print(display, &(lcd_print_config_t){
    //     .text       = "Hello!",
    //     .show_value = false,
    //     .fg         = ST7789_WHITE,
    //     .bg         = ST7789_BLUE,
    //     .fill_color = ST7789_BLACK,
    //     .x          = 10,
    //     .y          = 80,
    //     .scale      = 4,
    //     .rotation   = 1,          /* portrait */
    // });
    // ESP_LOGI(TAG, "lcd_print: string-only, portrait, scale 4");
    // vTaskDelay(pdMS_TO_TICKS(500));

    /* ── 1. String only, landscape, scale 4, white on blue bg, black screen ── */
lcd_print(display, &(lcd_print_config_t){
    .text       = "Hello!",
    .show_value = false,
    .fg         = ST7789_BLACK,
    .bg         = ST7789_WHITE,  // text back
    .fill_color = ST7789_WHITE,  // full screen back
    .x          = 20,
    .y          = 90,
    .scale      = 6,
    .rotation   = 1,          /* landscape */
});
ESP_LOGI(TAG, "lcd_print: string-only, landscape, scale 4");
vTaskDelay(pdMS_TO_TICKS(500));

    // /* ── 2. String + int, landscape, scale 3, black on yellow, red screen ── */
    // lcd_print(display, &(lcd_print_config_t){
    //     .text       = "Temp",
    //     .value      = 42,
    //     .show_value = true,
    //     .fg         = ST7789_BLACK,
    //     .bg         = ST7789_YELLOW,
    //     .fill_color = ST7789_RED,
    //     .x          = 8,
    //     .y          = 50,
    //     .scale      = 3,
    //     .rotation   = 1,          /* landscape */
    // });
    // ESP_LOGI(TAG, "lcd_print: string+int, landscape, scale 3");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // /* ── 3. Negative int, portrait-inverted, scale 2, cyan on black ── */
    // lcd_print(display, &(lcd_print_config_t){
    //     .text       = "Count",
    //     .value      = -512,
    //     .show_value = true,
    //     .fg         = ST7789_CYAN,
    //     .bg         = ST7789_BLACK,
    //     .fill_color = ST7789_BLACK,
    //     .x          = 4,
    //     .y          = 100,
    //     .scale      = 2,
    //     .rotation   = 2,          /* portrait inverted */
    // });
    // ESP_LOGI(TAG, "lcd_print: negative int, portrait-inv, scale 2");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // /* ── 4. Big pixel size, landscape-inverted, magenta on white screen ── */
    // lcd_print(display, &(lcd_print_config_t){
    //     .text       = "HI",
    //     .show_value = false,
    //     .fg         = ST7789_MAGENTA,
    //     .bg         = ST7789_WHITE,
    //     .fill_color = ST7789_WHITE,
    //     .x          = 20,
    //     .y          = 30,
    //     .scale      = 8,          /* largest pixel blocks */
    //     .rotation   = 3,          /* landscape inverted */
    // });
    // ESP_LOGI(TAG, "lcd_print: scale 8, landscape-inv, magenta");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // /* ── 5. Green on dark bg, scale 1 (smallest), landscape ── */
    // lcd_print(display, &(lcd_print_config_t){
    //     .text       = "Speed",
    //     .value      = 9600,
    //     .show_value = true,
    //     .fg         = ST7789_GREEN,
    //     .bg         = ST7789_COLOR(0, 30, 0),   /* very dark green bg */
    //     .fill_color = ST7789_COLOR(0, 10, 0),
    //     .x          = 2,
    //     .y          = 2,
    //     .scale      = 1,          /* smallest – 5×7 px per char */
    //     .rotation   = 1,
    // });
    // ESP_LOGI(TAG, "lcd_print: scale 1, green terminal style");
    // vTaskDelay(pdMS_TO_TICKS(2000));
}

/* ═══════════════════════════════════════════════════════════════════════
 * app_main
 * ═══════════════════════════════════════════════════════════════════════ */
void app_main(void)
{
    ESP_LOGI(TAG, "TB_STv3 Display Test");

    /* Backlight off during init */
    gpio_reset_pin(TFT_BLK);
    gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
    gpio_set_level(TFT_BLK, 0);

    /* SPI bus */
    spi_bus_config_t buscfg = {
        .mosi_io_num     = TFT_MOSI,
        .miso_io_num     = -1,
        .sclk_io_num     = TFT_SCLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 4096,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    /* Display init – portrait to start (rotation overridden per lcd_print call) */
    st7789_config_t disp_cfg = {
        .spi_host = SPI2_HOST,
        .cs_gpio  = TFT_CS,
        .dc_gpio  = TFT_DC,
        .rst_gpio = TFT_RST,
        .width    = 240,
        .height   = 320,
        .rotation = 0,
    };
    st7789_handle_t display = st7789_init(&disp_cfg);

    /* Backlight ON */
    gpio_set_level(TFT_BLK, 1);
    ESP_LOGI(TAG, "Display initialized");

    /* Run lcd_print demo first */
    while (1) {
        demo_lcd_print(display);

        /* After demo, show the GEM menu in landscape for 5 s */
        // setup_menu(display);

        /* Switch to landscape for menu */
        // lcd_print(display, &(lcd_print_config_t){   /* blank screen in landscape */
        //     .text       = "",
        //     .show_value = false,
        //     .fg         = ST7789_WHITE,
        //     .bg         = ST7789_BLACK,
        //     .fill_color = ST7789_BLACK,
        //     .x = 0, .y = 0, .scale = 1,
        //     .rotation   = 1,
        // });
        // gem_menu_draw(menu);
        // ESP_LOGI(TAG, "Menu drawn – holding 5 s");
        // vTaskDelay(pdMS_TO_TICKS(5000));

        /* Free menu before re-creating in next loop */
        /* (simple demo – no free() needed for static items, just recreate) */
    }
}