// /*
//  * main.c  –  TB_STv3 ESP32 + ST7789 240×320
//  *
//  * Demonstrates lcd_print() – a single-call text printer that accepts:
//  *   - text string
//  *   - integer value  (optional, appended as ": <n>")
//  *   - foreground colour
//  *   - background colour  (behind text only)
//  *   - full-screen fill colour
//  *   - screen orientation  (0-3)
//  *   - pixel scale  (1 = tiny  …  8 = huge)
//  *   - x / y position
//  *
//  * Also boots the GEM-style menu (same items as Arduino reference sketch).
//  */

// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/spi_master.h"
// #include "driver/gpio.h"
// #include "esp_log.h"

// #include "display.h"

// static const char *TAG = "main";

// /* ───────── PIN CONFIG ───────── */
// #define TFT_BLK   5
// #define TFT_SCLK  18
// #define TFT_MOSI  23
// #define TFT_DC    15
// #define TFT_CS    2
// #define TFT_RST   26
// // #define TFT_RST   20


// /* ───────── Menu state (mirrors Arduino reference) ───────── */
// static int  number      = -512;
// static bool enablePrint = false;
// static gem_menu_t   *menu = NULL;
// static gem_item_t    menu_items[3];

// static void printData(void)
// {
//     if (enablePrint) {
//         ESP_LOGI(TAG, "Number is: %d", number);
//     } else {
//         ESP_LOGI(TAG, "Printing is disabled, sorry:(");
//     }
// }

// static void setup_menu(st7789_handle_t display)
// {
//     menu_items[0] = gem_item_int   ("Number:",       &number);
//     menu_items[1] = gem_item_bool  ("Enable print:", &enablePrint);
//     menu_items[2] = gem_item_button("Print",          printData);

//     menu = gem_menu_create(display);
//     gem_menu_set_items(menu, menu_items, 3);
// }

// /* ═══════════════════════════════════════════════════════════════════════
//  * demo_lcd_print
//  *
//  * Cycles through several lcd_print() calls to show every parameter.
//  * Each call is held for 2 seconds so you can read the screen.
//  * ═══════════════════════════════════════════════════════════════════════ */
// static void demo_lcd_print(st7789_handle_t display)
// {
//     // /* ── 1. String only, portrait, scale 4, white on blue bg, black screen ── */
//     // lcd_print(display, &(lcd_print_config_t){
//     //     .text       = "Hello!",
//     //     .show_value = false,
//     //     .fg         = ST7789_WHITE,
//     //     .bg         = ST7789_BLUE,
//     //     .fill_color = ST7789_BLACK,
//     //     .x          = 10,
//     //     .y          = 80,
//     //     .scale      = 4,
//     //     .rotation   = 1,          /* portrait */
//     // });
//     // ESP_LOGI(TAG, "lcd_print: string-only, portrait, scale 4");
//     // vTaskDelay(pdMS_TO_TICKS(500));

//     /* ── 1. String only, landscape, scale 4, white on blue bg, black screen ── */
// lcd_print(display, &(lcd_print_config_t){
//     .text       = "Hello!",
//     .show_value = false,
//     .fg         = ST7789_BLACK,
//     .bg         = ST7789_WHITE,  // text back
//     .fill_color = ST7789_WHITE,  // full screen back
//     .x          = 20,
//     .y          = 90,
//     .scale      = 6,
//     .rotation   = 3,          /* landscape */
// });
// ESP_LOGI(TAG, "lcd_print: string-only, landscape, scale 4");
// vTaskDelay(pdMS_TO_TICKS(500));

//     // /* ── 2. String + int, landscape, scale 3, black on yellow, red screen ── */
//     // lcd_print(display, &(lcd_print_config_t){
//     //     .text       = "Temp",
//     //     .value      = 42,
//     //     .show_value = true,
//     //     .fg         = ST7789_BLACK,
//     //     .bg         = ST7789_YELLOW,
//     //     .fill_color = ST7789_RED,
//     //     .x          = 8,
//     //     .y          = 50,
//     //     .scale      = 3,
//     //     .rotation   = 1,          /* landscape */
//     // });
//     // ESP_LOGI(TAG, "lcd_print: string+int, landscape, scale 3");
//     // vTaskDelay(pdMS_TO_TICKS(2000));

//     // /* ── 3. Negative int, portrait-inverted, scale 2, cyan on black ── */
//     // lcd_print(display, &(lcd_print_config_t){
//     //     .text       = "Count",
//     //     .value      = -512,
//     //     .show_value = true,
//     //     .fg         = ST7789_CYAN,
//     //     .bg         = ST7789_BLACK,
//     //     .fill_color = ST7789_BLACK,
//     //     .x          = 4,
//     //     .y          = 100,
//     //     .scale      = 2,
//     //     .rotation   = 2,          /* portrait inverted */
//     // });
//     // ESP_LOGI(TAG, "lcd_print: negative int, portrait-inv, scale 2");
//     // vTaskDelay(pdMS_TO_TICKS(2000));

//     // /* ── 4. Big pixel size, landscape-inverted, magenta on white screen ── */
//     // lcd_print(display, &(lcd_print_config_t){
//     //     .text       = "HI",
//     //     .show_value = false,
//     //     .fg         = ST7789_MAGENTA,
//     //     .bg         = ST7789_WHITE,
//     //     .fill_color = ST7789_WHITE,
//     //     .x          = 20,
//     //     .y          = 30,
//     //     .scale      = 8,          /* largest pixel blocks */
//     //     .rotation   = 3,          /* landscape inverted */
//     // });
//     // ESP_LOGI(TAG, "lcd_print: scale 8, landscape-inv, magenta");
//     // vTaskDelay(pdMS_TO_TICKS(2000));

//     // /* ── 5. Green on dark bg, scale 1 (smallest), landscape ── */
//     // lcd_print(display, &(lcd_print_config_t){
//     //     .text       = "Speed",
//     //     .value      = 9600,
//     //     .show_value = true,
//     //     .fg         = ST7789_GREEN,
//     //     .bg         = ST7789_COLOR(0, 30, 0),   /* very dark green bg */
//     //     .fill_color = ST7789_COLOR(0, 10, 0),
//     //     .x          = 2,
//     //     .y          = 2,
//     //     .scale      = 1,          /* smallest – 5×7 px per char */
//     //     .rotation   = 1,
//     // });
//     // ESP_LOGI(TAG, "lcd_print: scale 1, green terminal style");
//     // vTaskDelay(pdMS_TO_TICKS(2000));
// }


// /* ═══════════════════════════════════════════════════════════════════════
//  * app_main
//  * ═══════════════════════════════════════════════════════════════════════ */
// void app_main(void)
// {
//     ESP_LOGI(TAG, "TB_STv3 Display Test");

//     /* Backlight off during init */
//     gpio_reset_pin(TFT_BLK);
//     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
//     gpio_set_level(TFT_BLK, 0);

//     /* SPI bus */
//     spi_bus_config_t buscfg = {
//         .mosi_io_num     = TFT_MOSI,
//         .miso_io_num     = -1,
//         .sclk_io_num     = TFT_SCLK,
//         .quadwp_io_num   = -1,
//         .quadhd_io_num   = -1,
//         .max_transfer_sz = 4096,
//     };
//     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

//     /* Display init – portrait to start (rotation overridden per lcd_print call) */
//     st7789_config_t disp_cfg = {
//         .spi_host = SPI2_HOST,
//         .cs_gpio  = TFT_CS,
//         .dc_gpio  = TFT_DC,
//         .rst_gpio = TFT_RST,
//         .width    = 240,
//         .height   = 320,
//         .rotation = 0,
//     };
//     st7789_handle_t display = st7789_init(&disp_cfg);

//     /* Backlight ON */
//     gpio_set_level(TFT_BLK, 1);
//     ESP_LOGI(TAG, "Display initialized");

//     /* Run lcd_print demo first */
//     while (1) {
//         demo_lcd_print(display);

//         /* After demo, show the GEM menu in landscape for 5 s */
//         // setup_menu(display);

//         /* Switch to landscape for menu */
//         // lcd_print(display, &(lcd_print_config_t){   /* blank screen in landscape */
//         //     .text       = "",
//         //     .show_value = false,
//         //     .fg         = ST7789_WHITE,
//         //     .bg         = ST7789_BLACK,
//         //     .fill_color = ST7789_BLACK,
//         //     .x = 0, .y = 0, .scale = 1,
//         //     .rotation   = 1,
//         // });
//         // gem_menu_draw(menu);
//         // ESP_LOGI(TAG, "Menu drawn – holding 5 s");
//         // vTaskDelay(pdMS_TO_TICKS(5000));

//         /* Free menu before re-creating in next loop */
//         /* (simple demo – no free() needed for static items, just recreate) */
//     }
// }



// // // /*
// // //  * main.c – ESP32 + ST7789 + SPI SLAVE (STM32)
// // //  *
// // //  * - SPI2_HOST → ST7789 (MASTER)
// // //  * - SPI3_HOST → STM32 (SLAVE)
// // //  * - Displays received SPI data using lcd_print()
// // //  */

// // // #include <stdio.h>
// // // #include <string.h>
// // // #include <stdbool.h>

// // // #include "freertos/FreeRTOS.h"
// // // #include "freertos/task.h"

// // // #include "driver/spi_master.h"
// // // #include "driver/spi_slave.h"
// // // #include "driver/gpio.h"

// // // #include "esp_log.h"
// // // #include "esp_heap_caps.h"

// // // #include "display.h"

// // // static const char *TAG = "MAIN";

// // // /* ───────── DISPLAY (SPI MASTER) ───────── */
// // // #define TFT_BLK   5
// // // #define TFT_SCLK  18
// // // #define TFT_MOSI  23
// // // #define TFT_DC    15
// // // #define TFT_CS    2
// // // #define TFT_RST   26

// // // /* ───────── SPI SLAVE (STM32) ───────── */
// // // #define SPI_SLAVE_HOST SPI3_HOST

// // // #define PIN_MOSI    13
// // // #define PIN_MISO    12
// // // #define PIN_CLK     14
// // // #define PIN_CS      19

// // // #define BUF_SIZE    64


// // // /* ═══════════════════════════════════════════════
// // //  * SPI SLAVE TASK (runs in parallel)
// // //  * ═══════════════════════════════════════════════ */
// // // static void spi_slave_task(void *arg)
// // // {
// // //     st7789_handle_t display = (st7789_handle_t)arg;

// // //     esp_err_t ret;

// // //     /* SPI BUS CONFIG */
// // //     spi_bus_config_t bus_cfg = {
// // //         .mosi_io_num   = PIN_MOSI,
// // //         .miso_io_num   = PIN_MISO,
// // //         .sclk_io_num   = PIN_CLK,
// // //         .quadwp_io_num = -1,
// // //         .quadhd_io_num = -1,
// // //     };

// // //     /* SLAVE CONFIG */
// // //     spi_slave_interface_config_t slave_cfg = {
// // //         .mode = 0,                 // MUST match STM32
// // //         .spics_io_num = PIN_CS,
// // //         .queue_size = 3,
// // //     };

// // //     /* Pull-ups for stable signals */
// // //     gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
// // //     gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);

// // //     /* INIT SPI SLAVE */
// // //     ret = spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO);
// // //     ESP_ERROR_CHECK(ret);

// // //     ESP_LOGI(TAG, "SPI Slave Ready");

// // //     /* DMA buffers */
// // //     uint8_t *tx_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
// // //     uint8_t *rx_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);

// // //     if (!tx_buf || !rx_buf) {
// // //         ESP_LOGE(TAG, "DMA buffer allocation failed");
// // //         vTaskDelete(NULL);
// // //     }

// // //     uint32_t counter = 0;

// // //     while (1)
// // //     {
// // //         memset(tx_buf, 0, BUF_SIZE);
// // //         memset(rx_buf, 0, BUF_SIZE);

// // //         /* Data sent back to STM32 */
// // //         snprintf((char*)tx_buf, BUF_SIZE, "ESP32 #%lu", counter++);

// // //         spi_slave_transaction_t t = {
// // //             .length    = BUF_SIZE * 8,
// // //             .tx_buffer = tx_buf,
// // //             .rx_buffer = rx_buf,
// // //         };

// // //         /* Wait for STM32 */
// // //         ret = spi_slave_transmit(SPI_SLAVE_HOST, &t, portMAX_DELAY);

// // //         if (ret == ESP_OK)
// // //         {
// // //             ESP_LOGI("SPI", "Received: %s", rx_buf);

// // //             /* DISPLAY RECEIVED DATA */
// // //             lcd_print(display, &(lcd_print_config_t){
// // //                 .text       = (char*)rx_buf,
// // //                 .show_value = false,
// // //                 .fg         = ST7789_BLACK,
// // //                 .bg         = ST7789_WHITE,
// // //                 .fill_color = ST7789_WHITE,
// // //                 .x          = 10,
// // //                 .y          = 120,
// // //                 .scale      = 2,
// // //                 .rotation   = 3,   // landscape
// // //             });
// // //         }
// // //         else {
// // //             ESP_LOGE(TAG, "SPI error: %s", esp_err_to_name(ret));
// // //         }

// // //         vTaskDelay(pdMS_TO_TICKS(50)); // small stability delay
// // //     }
// // // }


// // // /* ═══════════════════════════════════════════════
// // //  * MAIN
// // //  * ═══════════════════════════════════════════════ */
// // // void app_main(void)
// // // {
// // //     ESP_LOGI(TAG, "System Start");

// // //     /* Backlight OFF during init */
// // //     gpio_reset_pin(TFT_BLK);
// // //     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
// // //     gpio_set_level(TFT_BLK, 0);

// // //     /* SPI BUS FOR DISPLAY (MASTER) */
// // //     spi_bus_config_t buscfg = {
// // //         .mosi_io_num     = TFT_MOSI,
// // //         .miso_io_num     = -1,
// // //         .sclk_io_num     = TFT_SCLK,
// // //         .quadwp_io_num   = -1,
// // //         .quadhd_io_num   = -1,
// // //         .max_transfer_sz = 4096,
// // //     };

// // //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

// // //     /* DISPLAY CONFIG */
// // //     st7789_config_t disp_cfg = {
// // //         .spi_host = SPI2_HOST,
// // //         .cs_gpio  = TFT_CS,
// // //         .dc_gpio  = TFT_DC,
// // //         .rst_gpio = TFT_RST,
// // //         .width    = 240,
// // //         .height   = 320,
// // //         .rotation = 0,
// // //     };

// // //     st7789_handle_t display = st7789_init(&disp_cfg);

// // //     /* Backlight ON */
// // //     gpio_set_level(TFT_BLK, 1);

// // //     ESP_LOGI(TAG, "Display Ready");

// // //     /* Initial screen */
// // //     lcd_print(display, &(lcd_print_config_t){
// // //         .text       = "Waiting for SPI...",
// // //         .show_value = false,
// // //         .fg         = ST7789_WHITE,
// // //         .bg         = ST7789_BLACK,
// // //         .fill_color = ST7789_BLACK,
// // //         .x          = 20,
// // //         .y          = 120,
// // //         .scale      = 2,
// // //         .rotation   = 3,
// // //     });

// // //     /* START SPI SLAVE TASK */
// // //     xTaskCreate(spi_slave_task, "spi_slave_task", 4096, display, 5, NULL);

// // //     /* MAIN LOOP (idle) */
// // //     while (1) {
// // //         vTaskDelay(pdMS_TO_TICKS(1000));
// // //     }
// // // }


// // // // /*
// // // //  * main.c – ESP32 + ST7789 + SPI SLAVE (STM32)
// // // //  *
// // // //  * - SPI2_HOST → ST7789 display (MASTER)
// // // //  * - SPI3_HOST → STM32 (SLAVE)
// // // //  *
// // // //  * Packet format from STM32 (64 bytes):
// // // //  *   [0]       = 0xA5  start marker
// // // //  *   [1]       = payload length (n)
// // // //  *   [2..n+1]  = RF payload string (ASCII)
// // // //  *   [n+2]     = RSSI raw byte
// // // //  *   [n+3]     = RSSI dBm low  byte  (int16, little-endian)
// // // //  *   [n+4]     = RSSI dBm high byte
// // // //  *   rest      = 0x00 padding
// // // //  */

// // // // #include <stdio.h>
// // // // #include <string.h>
// // // // #include <stdbool.h>

// // // // #include "freertos/FreeRTOS.h"
// // // // #include "freertos/task.h"

// // // // #include "driver/spi_master.h"
// // // // #include "driver/spi_slave.h"
// // // // #include "driver/gpio.h"

// // // // #include "esp_log.h"
// // // // #include "esp_heap_caps.h"

// // // // #include "display.h"

// // // // static const char *TAG = "MAIN";

// // // // /* ───────── DISPLAY PINS (SPI MASTER → ST7789) ───────── */
// // // // #define TFT_BLK   5
// // // // #define TFT_SCLK  18
// // // // #define TFT_MOSI  23
// // // // #define TFT_DC    15
// // // // #define TFT_CS    2
// // // // #define TFT_RST   26

// // // // /* ───────── SPI SLAVE PINS (← STM32) ───────── */
// // // // #define SPI_SLAVE_HOST  SPI3_HOST
// // // // #define PIN_MOSI        13
// // // // #define PIN_MISO        12
// // // // #define PIN_CLK         14
// // // // #define PIN_CS          19
// // // // #define BUF_SIZE        64

// // // // /* ───────── Display layout constants (landscape 320×240) ───────── */
// // // // #define LABEL_X         10
// // // // #define DATA_X          10
// // // // #define ROW_DATA_Y      30    /* RF payload text  */
// // // // #define ROW_RSSI_Y      140   /* RSSI line        */
// // // // #define ROW_RAW_Y       195   /* raw RSSI byte    */


// // // // /* ═══════════════════════════════════════════════════════
// // // //  * clear_screen
// // // //  * Fill the whole screen black without printing any text.
// // // //  * ═══════════════════════════════════════════════════════ */
// // // // static void clear_screen(st7789_handle_t display)
// // // // {
// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = " ",
// // // //         .show_value = false,
// // // //         .fg         = ST7789_BLACK,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = 0,
// // // //         .y          = 0,
// // // //         .scale      = 1,
// // // //         .rotation   = 3,
// // // //     });
// // // // }


// // // // /* ═══════════════════════════════════════════════════════
// // // //  * show_waiting
// // // //  * Boot screen shown before first valid packet arrives.
// // // //  * ═══════════════════════════════════════════════════════ */
// // // // static void show_waiting(st7789_handle_t display)
// // // // {
// // // //     clear_screen(display);

// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = "UHF RECEIVER",
// // // //         .show_value = false,
// // // //         .fg         = ST7789_CYAN,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = LABEL_X,
// // // //         .y          = 80,
// // // //         .scale      = 3,
// // // //         .rotation   = 3,
// // // //     });

// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = "Waiting for data...",
// // // //         .show_value = false,
// // // //         .fg         = ST7789_WHITE,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = LABEL_X,
// // // //         .y          = 150,
// // // //         .scale      = 2,
// // // //         .rotation   = 3,
// // // //     });
// // // // }


// // // // /* ═══════════════════════════════════════════════════════
// // // //  * show_rf_data
// // // //  * Renders the parsed RF payload + RSSI onto the display.
// // // //  *
// // // //  *  ┌──────────────────────────────┐
// // // //  *  │ RF DATA                      │  ← cyan label, scale 2
// // // //  *  │ <payload string>             │  ← green,      scale 4
// // // //  *  │                              │
// // // //  *  │ RSSI: -72 dBm                │  ← yellow,     scale 3
// // // //  *  │ RAW:  0xB4                   │  ← grey,       scale 2
// // // //  *  └──────────────────────────────┘
// // // //  * ═══════════════════════════════════════════════════════ */
// // // // static void show_rf_data(st7789_handle_t display,
// // // //                          const char *payload,
// // // //                          uint8_t     rssi_raw,
// // // //                          int16_t     rssi_dbm)
// // // // {
// // // //     char line_rssi[32];
// // // //     char line_raw[24];

// // // //     snprintf(line_rssi, sizeof(line_rssi), "RSSI: %d dBm", rssi_dbm);
// // // //     snprintf(line_raw,  sizeof(line_raw),  "RAW:  0x%02X",  rssi_raw);

// // // //     /* 1. Clear full screen */
// // // //     clear_screen(display);

// // // //     /* 2. "RF DATA" label */
// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = "RF DATA",
// // // //         .show_value = false,
// // // //         .fg         = ST7789_CYAN,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = LABEL_X,
// // // //         .y          = ROW_DATA_Y,
// // // //         .scale      = 2,
// // // //         .rotation   = 3,
// // // //     });

// // // //     /* 3. Payload — big green text */
// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = payload,
// // // //         .show_value = false,
// // // //         .fg         = ST7789_GREEN,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = DATA_X,
// // // //         .y          = ROW_DATA_Y + 30,
// // // //         .scale      = 4,
// // // //         .rotation   = 3,
// // // //     });

// // // //     /* 4. RSSI dBm — yellow, medium */
// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = line_rssi,
// // // //         .show_value = false,
// // // //         .fg         = ST7789_YELLOW,
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = DATA_X,
// // // //         .y          = ROW_RSSI_Y,
// // // //         .scale      = 3,
// // // //         .rotation   = 3,
// // // //     });

// // // //     /* 5. Raw RSSI byte — dim grey, small */
// // // //     lcd_print(display, &(lcd_print_config_t){
// // // //         .text       = line_raw,
// // // //         .show_value = false,
// // // //         .fg         = ST7789_COLOR(180, 180, 180),
// // // //         .bg         = ST7789_BLACK,
// // // //         .fill_color = ST7789_BLACK,
// // // //         .x          = DATA_X,
// // // //         .y          = ROW_RAW_Y,
// // // //         .scale      = 2,
// // // //         .rotation   = 3,
// // // //     });
// // // // }


// // // // /* ═══════════════════════════════════════════════════════
// // // //  * spi_slave_task
// // // //  * Waits for 64-byte frames from STM32, validates the
// // // //  * 0xA5 start marker, parses payload + RSSI, updates display.
// // // //  * ═══════════════════════════════════════════════════════ */
// // // // static void spi_slave_task(void *arg)
// // // // {
// // // //     st7789_handle_t display = (st7789_handle_t)arg;
// // // //     esp_err_t ret;

// // // //     /* ── SPI bus ── */
// // // //     spi_bus_config_t bus_cfg = {
// // // //         .mosi_io_num   = PIN_MOSI,
// // // //         .miso_io_num   = PIN_MISO,
// // // //         .sclk_io_num   = PIN_CLK,
// // // //         .quadwp_io_num = -1,
// // // //         .quadhd_io_num = -1,
// // // //     };

// // // //     /* ── Slave interface ── */
// // // //     spi_slave_interface_config_t slave_cfg = {
// // // //         .mode         = 0,          /* CPOL=0, CPHA=0 — must match STM32 */
// // // //         .spics_io_num = PIN_CS,
// // // //         .queue_size   = 3,
// // // //     };

// // // //     /* Stable pull-ups on MOSI and CLK */
// // // //     gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
// // // //     gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);

// // // //     ret = spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO);
// // // //     ESP_ERROR_CHECK(ret);
// // // //     ESP_LOGI(TAG, "SPI Slave ready on SPI3");

// // // //     /* DMA-capable buffers */
// // // //     uint8_t *tx_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
// // // //     uint8_t *rx_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);

// // // //     if (!tx_buf || !rx_buf) {
// // // //         ESP_LOGE(TAG, "DMA malloc failed");
// // // //         vTaskDelete(NULL);
// // // //         return;
// // // //     }

// // // //     uint32_t pkt_count  = 0;
// // // //     uint32_t bad_frames = 0;

// // // //     while (1)
// // // //     {
// // // //         memset(tx_buf, 0, BUF_SIZE);
// // // //         memset(rx_buf, 0, BUF_SIZE);

// // // //         /* ACK string sent back to STM32 during the same transfer */
// // // //         snprintf((char *)tx_buf, BUF_SIZE, "ACK #%lu", pkt_count);

// // // //         spi_slave_transaction_t t = {
// // // //             .length    = BUF_SIZE * 8,   /* bits */
// // // //             .tx_buffer = tx_buf,
// // // //             .rx_buffer = rx_buf,
// // // //         };

// // // //         ret = spi_slave_transmit(SPI_SLAVE_HOST, &t, portMAX_DELAY);

// // // //         if (ret != ESP_OK) {
// // // //             ESP_LOGE(TAG, "SPI transmit error: %s", esp_err_to_name(ret));
// // // //             vTaskDelay(pdMS_TO_TICKS(50));
// // // //             continue;
// // // //         }

// // // //         /* ── Validate start marker ── */
// // // //         if (rx_buf[0] != 0xA5) {
// // // //             bad_frames++;
// // // //             ESP_LOGW("SPI", "Bad frame [%lu] marker=0x%02X", bad_frames, rx_buf[0]);
// // // //             vTaskDelay(pdMS_TO_TICKS(50));
// // // //             continue;
// // // //         }

// // // //         /* ── Parse packet ── */
// // // //         uint8_t payload_len = rx_buf[1];
// // // //         if (payload_len > BUF_SIZE - 5) payload_len = BUF_SIZE - 5;  /* clamp */

// // // //         char payload[64] = {0};
// // // //         memcpy(payload, &rx_buf[2], payload_len);
// // // //         payload[payload_len] = '\0';   /* null-terminate — critical for lcd_print */

// // // //         uint8_t rssi_raw = rx_buf[2 + payload_len];

// // // //         /* Reconstruct signed int16 from two little-endian bytes */
// // // //         int16_t rssi_dbm = (int16_t)(
// // // //             (uint16_t)rx_buf[3 + payload_len] |
// // // //             ((uint16_t)rx_buf[4 + payload_len] << 8)
// // // //         );

// // // //         pkt_count++;
// // // //         ESP_LOGI("SPI", "[%lu] payload=\"%s\"  RSSI=%d dBm  raw=0x%02X",
// // // //                  pkt_count, payload, rssi_dbm, rssi_raw);

// // // //         /* ── Update display ── */
// // // //         show_rf_data(display, payload, rssi_raw, rssi_dbm);

// // // //         vTaskDelay(pdMS_TO_TICKS(50));   /* small yield */
// // // //     }
// // // // }


// // // // /* ═══════════════════════════════════════════════════════
// // // //  * app_main
// // // //  * ═══════════════════════════════════════════════════════ */
// // // // void app_main(void)
// // // // {
// // // //     ESP_LOGI(TAG, "System Start");

// // // //     /* ── Backlight OFF during init ── */
// // // //     gpio_reset_pin(TFT_BLK);
// // // //     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
// // // //     gpio_set_level(TFT_BLK, 0);

// // // //     /* ── SPI bus for display (MASTER) ── */
// // // //     spi_bus_config_t buscfg = {
// // // //         .mosi_io_num     = TFT_MOSI,
// // // //         .miso_io_num     = -1,
// // // //         .sclk_io_num     = TFT_SCLK,
// // // //         .quadwp_io_num   = -1,
// // // //         .quadhd_io_num   = -1,
// // // //         .max_transfer_sz = 4096,
// // // //     };
// // // //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

// // // //     /* ── Display init ── */
// // // //     st7789_config_t disp_cfg = {
// // // //         .spi_host = SPI2_HOST,
// // // //         .cs_gpio  = TFT_CS,
// // // //         .dc_gpio  = TFT_DC,
// // // //         .rst_gpio = TFT_RST,
// // // //         .width    = 240,
// // // //         .height   = 320,
// // // //         .rotation = 0,
// // // //     };
// // // //     st7789_handle_t display = st7789_init(&disp_cfg);

// // // //     /* ── Backlight ON ── */
// // // //     gpio_set_level(TFT_BLK, 1);
// // // //     ESP_LOGI(TAG, "Display Ready");

// // // //     /* ── Boot screen ── */
// // // //     show_waiting(display);

// // // //     /* ── Start SPI slave task (priority 5, 4 kB stack) ── */
// // // //     xTaskCreate(spi_slave_task, "spi_slave", 4096, display, 5, NULL);

// // // //     /* ── Main loop idles — all work is in the task ── */
// // // //     while (1) {
// // // //         vTaskDelay(pdMS_TO_TICKS(1000));
// // // //     }
// // // // }




// // // /*
// // //  * main.c – ESP32 + ST7789 + SPI SLAVE (STM32)
// // //  * Improved: Stability + Bigger Font + Slow UI Refresh
// // //  */

// // // #include <stdio.h>
// // // #include <string.h>
// // // #include <stdbool.h>

// // // #include "freertos/FreeRTOS.h"
// // // #include "freertos/task.h"

// // // #include "driver/spi_master.h"
// // // #include "driver/spi_slave.h"
// // // #include "driver/gpio.h"

// // // #include "esp_log.h"
// // // #include "esp_heap_caps.h"

// // // #include "display.h"

// // // static const char *TAG = "MAIN";

// // // /* ── Display pins ── */
// // // #define TFT_BLK   5
// // // #define TFT_SCLK  18
// // // #define TFT_MOSI  23
// // // #define TFT_DC    15
// // // #define TFT_CS    2
// // // #define TFT_RST   26

// // // /* ── SPI slave pins ── */
// // // #define SPI_SLAVE_HOST  SPI3_HOST
// // // #define PIN_MOSI        13
// // // #define PIN_MISO        12
// // // #define PIN_CLK         14
// // // #define PIN_CS          19

// // // #define BUF_SIZE 128

// // // /* ── Colors ── */
// // // #define C_FG   ST7789_WHITE
// // // #define C_BG   ST7789_BLACK
// // // #define C_DIM  ST7789_COLOR(120,120,120)

// // // /* ── Layout ── */
// // // #define COL_X          8
// // // #define Y_TITLE        5
// // // #define Y_DATA_LBL     48
// // // #define Y_DATA_VAL     75
// // // #define Y_RSSI_LBL    145
// // // #define Y_RSSI_VAL    168
// // // #define Y_RAW_VAL     210

// // // /* ── Bigger font ── */
// // // #define SCALE_BIG 4
// // // #define SCALE_MED 3

// // // #define PAD_VAL3 14
// // // #define PAD_VAL2 20

// // // /* ── UI timing ── */
// // // #define UI_REFRESH_MS      300
// // // #define NO_DATA_TIMEOUT_MS 2000

// // // /* ── State ── */
// // // static char s_payload[64] = {0};
// // // static char s_rssi[32]    = {0};
// // // static char s_raw[24]     = {0};
// // // static char last_payload[64] = {0};

// // // static uint32_t last_ui_update = 0;
// // // static uint32_t last_valid_pkt = 0;


// // // /* ================= Helpers ================= */

// // // static void pad_right(char *dst, const char *src, int total_len)
// // // {
// // //     int n = strlen(src);
// // //     if (n > total_len) n = total_len;

// // //     memcpy(dst, src, n);
// // //     for (int i = n; i < total_len; i++) dst[i] = ' ';
// // //     dst[total_len] = '\0';
// // // }

// // // static void field_print(st7789_handle_t disp,
// // //                         const char *new_val, char *prev_val,
// // //                         int x, int y, int scale, int pad,
// // //                         uint16_t fg)
// // // {
// // //     if (strcmp(new_val, prev_val) == 0) return;

// // //     char buf[64];
// // //     pad_right(buf, new_val, pad);

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = buf,
// // //         .fg = fg,
// // //         .bg = C_BG,
// // //         .fill_color = C_BG,
// // //         .x = x,
// // //         .y = y,
// // //         .scale = scale,
// // //         .rotation = 3,
// // //     });

// // //     strncpy(prev_val, new_val, 63);
// // // }


// // // /* ================= UI ================= */

// // // static void draw_static_ui(st7789_handle_t disp)
// // // {
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = " ", .fg = C_BG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = 0, .y = 0, .scale = 1, .rotation = 3,
// // //     });

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "UHF RECEIVER",
// // //         .fg = C_FG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_TITLE, .scale = SCALE_BIG, .rotation = 3,
// // //     });

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "__________________",
// // //         .fg = C_DIM, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_TITLE + 32, .scale = SCALE_MED, .rotation = 3,
// // //     });

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "DATA:",
// // //         .fg = C_FG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_DATA_LBL, .scale = SCALE_MED, .rotation = 3,
// // //     });

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "RSSI:",
// // //         .fg = C_FG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_RSSI_LBL, .scale = SCALE_MED, .rotation = 3,
// // //     });
// // // }

// // // static void show_no_data(st7789_handle_t disp)
// // // {
// // //     field_print(disp, "-- no data --", s_payload,
// // //                 COL_X, Y_DATA_VAL, SCALE_BIG, PAD_VAL3, C_DIM);

// // //     field_print(disp, "---", s_rssi,
// // //                 COL_X, Y_RSSI_VAL, SCALE_BIG, PAD_VAL3, C_DIM);

// // //     field_print(disp, "RAW --", s_raw,
// // //                 COL_X, Y_RAW_VAL, SCALE_MED, PAD_VAL2, C_DIM);
// // // }

// // // static void show_rf_data(st7789_handle_t disp,
// // //                          const char *payload,
// // //                          uint8_t rssi_raw, int16_t rssi_dbm)
// // // {
// // //     char l_rssi[24];
// // //     char l_raw[20];

// // //     snprintf(l_rssi, sizeof(l_rssi), "%d dBm", rssi_dbm);
// // //     snprintf(l_raw, sizeof(l_raw), "RAW 0x%02X", rssi_raw);

// // //     field_print(disp, payload, s_payload,
// // //                 COL_X, Y_DATA_VAL, SCALE_BIG, PAD_VAL3, C_FG);

// // //     field_print(disp, l_rssi, s_rssi,
// // //                 COL_X, Y_RSSI_VAL, SCALE_BIG, PAD_VAL3, C_FG);

// // //     field_print(disp, l_raw, s_raw,
// // //                 COL_X, Y_RAW_VAL, SCALE_MED, PAD_VAL2, C_DIM);
// // // }


// // // /* ================= SPI TASK ================= */

// // // static void spi_slave_task(void *arg)
// // // {
// // //     st7789_handle_t disp = (st7789_handle_t)arg;
// // //     esp_err_t ret;

// // //     spi_bus_config_t bus_cfg = {
// // //         .mosi_io_num = PIN_MOSI,
// // //         .miso_io_num = PIN_MISO,
// // //         .sclk_io_num = PIN_CLK,
// // //     };

// // //     spi_slave_interface_config_t slave_cfg = {
// // //         .mode = 0,
// // //         .spics_io_num = PIN_CS,
// // //         .queue_size = 4,
// // //     };

// // //     gpio_set_pull_mode(PIN_CS, GPIO_PULLUP_ONLY);

// // //     ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO));

// // //     uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// // //     uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);

// // //     memset(tx_buf, 0, BUF_SIZE);
// // //     snprintf((char*)tx_buf, BUF_SIZE, "ESP32 READY");

// // //     draw_static_ui(disp);
// // //     show_no_data(disp);

// // //     while (1)
// // //     {
// // //         memset(rx_buf, 0, BUF_SIZE);

// // //         spi_slave_transaction_t t = {
// // //             .length = BUF_SIZE * 8,
// // //             .tx_buffer = tx_buf,
// // //             .rx_buffer = rx_buf,
// // //         };

// // //         ret = spi_slave_transmit(SPI_SLAVE_HOST, &t, pdMS_TO_TICKS(3000));

// // //         uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

// // //         if (ret == ESP_ERR_TIMEOUT) {
// // //             if (now - last_valid_pkt > NO_DATA_TIMEOUT_MS) {
// // //                 show_no_data(disp);
// // //             }
// // //             continue;
// // //         }

// // //         if (ret != ESP_OK) continue;

// // //         if (rx_buf[0] != 0xA5) continue;

// // //         uint8_t plen = rx_buf[1];
// // //         if (plen == 0 || plen > 60) continue;

// // //         char payload[64] = {0};
// // //         memcpy(payload, &rx_buf[2], plen);

// // //         /* ASCII validation */
// // //         bool valid = true;
// // //         for (int i = 0; i < plen; i++) {
// // //             if (payload[i] < 32 || payload[i] > 126) {
// // //                 valid = false;
// // //                 break;
// // //             }
// // //         }
// // //         if (!valid) continue;

// // //         /* Duplicate filter */
// // //         if (strcmp(payload, last_payload) == 0) continue;
// // //         strcpy(last_payload, payload);

// // //         uint8_t rssi_raw = rx_buf[2 + plen];
// // //         int16_t rssi_dbm = (int16_t)(
// // //             rx_buf[3 + plen] |
// // //             (rx_buf[4 + plen] << 8)
// // //         );

// // //         last_valid_pkt = now;

// // //         /* Throttled UI update */
// // //         if (now - last_ui_update > UI_REFRESH_MS) {
// // //             show_rf_data(disp, payload, rssi_raw, rssi_dbm);
// // //             last_ui_update = now;
// // //         }

// // //         snprintf((char*)tx_buf, BUF_SIZE, "ACK");
// // //     }
// // // }


// // // /* ================= MAIN ================= */

// // // void app_main(void)
// // // {
// // //     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
// // //     gpio_set_level(TFT_BLK, 0);

// // //     spi_bus_config_t buscfg = {
// // //         .mosi_io_num = TFT_MOSI,
// // //         .sclk_io_num = TFT_SCLK,
// // //         .max_transfer_sz = 4096,
// // //     };

// // //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

// // //     st7789_config_t disp_cfg = {
// // //         .spi_host = SPI2_HOST,
// // //         .cs_gpio = TFT_CS,
// // //         .dc_gpio = TFT_DC,
// // //         .rst_gpio = TFT_RST,
// // //         .width = 240,
// // //         .height = 320,
// // //     };

// // //     st7789_handle_t display = st7789_init(&disp_cfg);

// // //     gpio_set_level(TFT_BLK, 1);

// // //     xTaskCreate(spi_slave_task, "spi_slave", 6144, display, 5, NULL);

// // //     while (1) vTaskDelay(pdMS_TO_TICKS(5000));
// // // }













// // /*
// //  * main.c – ESP32 + ST7789 + SPI SLAVE (STM32)
// //  *
// //  * Simplest possible display:
// //  * Takes raw incoming SPI string, splits into 16-char lines,
// //  * prints them top to bottom. No packet parsing.
// //  */

// // #include <stdio.h>
// // #include <string.h>

// // #include "freertos/FreeRTOS.h"
// // #include "freertos/task.h"

// // #include "driver/spi_slave.h"
// // #include "driver/gpio.h"

// // #include "esp_log.h"
// // #include "esp_heap_caps.h"

// // #include "display.h"

// // static const char *TAG = "MAIN";

// // /* ── Display pins ── */
// // #define TFT_BLK   5
// // #define TFT_SCLK  18
// // #define TFT_MOSI  23
// // #define TFT_DC    15
// // #define TFT_CS    2
// // #define TFT_RST   26

// // /* ── SPI slave pins (← STM32) ── */
// // #define SPI_SLAVE_HOST  SPI3_HOST
// // #define PIN_MOSI        13
// // #define PIN_MISO        12
// // #define PIN_CLK         14
// // #define PIN_CS          19

// // #define BUF_SIZE   128

// // /* ── Text layout (landscape 320×240, scale 2) ──
// //  *   char cell = 12 px wide, 16 px tall
// //  *   chars per line = 320 / 12 = 26 → use 24 to leave margin
// //  *   max lines      = 240 / 16 = 15 → use 6 lines with spacing  */
// // #define SCALE       2
// // #define CHARS_LINE  24
// // #define LINE_H      20    /* px between line tops */
// // #define START_Y     10
// // #define START_X     8

// // /* Previous lines — diff draw to avoid flicker */
// // static char prev_line[6][CHARS_LINE + 1];


// // /* ── pad_right: fill field with spaces so old text is erased ── */
// // static void pad_right(char *dst, const char *src, int len)
// // {
// //     int n = strlen(src);
// //     if (n > len) n = len;
// //     memcpy(dst, src, n);
// //     for (int i = n; i < len; i++) dst[i] = ' ';
// //     dst[len] = '\0';
// // }


// // /* ── print_raw: split buf into lines and display ── */
// // static void print_raw(st7789_handle_t disp, const char *buf)
// // {
// //     int total = strlen(buf);

// //     for (int row = 0; row < 6; row++)
// //     {
// //         char line[CHARS_LINE + 1] = {0};
// //         int  offset = row * CHARS_LINE;

// //         if (offset < total)
// //         {
// //             int avail = total - offset;
// //             int take  = avail < CHARS_LINE ? avail : CHARS_LINE;
// //             memcpy(line, buf + offset, take);
// //         }
// //         /* else line stays empty → spaces will erase old content */

// //         /* Only redraw if changed */
// //         if (strcmp(line, prev_line[row]) == 0) continue;

// //         char padded[CHARS_LINE + 1];
// //         pad_right(padded, line, CHARS_LINE);

// //         lcd_print(disp, &(lcd_print_config_t){
// //             .text       = padded,
// //             .show_value = false,
// //             .fg         = ST7789_WHITE,
// //             .bg         = ST7789_BLACK,
// //             .fill_color = ST7789_BLACK,
// //             .x          = START_X,
// //             .y          = START_Y + row * LINE_H,
// //             .scale      = SCALE,
// //             .rotation   = 3,
// //         });

// //         strncpy(prev_line[row], line, CHARS_LINE);
// //     }
// // }


// // /* ── spi_slave_task ── */
// // static void spi_slave_task(void *arg)
// // {
// //     st7789_handle_t disp = (st7789_handle_t)arg;

// //     spi_bus_config_t bus_cfg = {
// //         .mosi_io_num   = PIN_MOSI,
// //         .miso_io_num   = PIN_MISO,
// //         .sclk_io_num   = PIN_CLK,
// //         .quadwp_io_num = -1,
// //         .quadhd_io_num = -1,
// //     };

// //     spi_slave_interface_config_t slave_cfg = {
// //         .mode         = 0,
// //         .spics_io_num = PIN_CS,
// //         .queue_size   = 4,
// //     };

// //     gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
// //     gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);
// //     gpio_set_pull_mode(PIN_CS,   GPIO_PULLUP_ONLY);

// //     ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg, &slave_cfg, SPI_DMA_CH_AUTO));
// //     ESP_LOGI(TAG, "SPI slave ready");

// //     uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// //     uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);

// //     memset(tx_buf, 0, BUF_SIZE);
// //     snprintf((char *)tx_buf, BUF_SIZE, "ESP32 READY");

// //     /* Boot screen */
// //     lcd_print(disp, &(lcd_print_config_t){
// //         .text = " ", .fg = ST7789_BLACK, .bg = ST7789_BLACK,
// //         .fill_color = ST7789_BLACK, .x = 0, .y = 0, .scale = 1, .rotation = 3,
// //     });
// //     lcd_print(disp, &(lcd_print_config_t){
// //         .text = "Waiting...", .show_value = false,
// //         .fg = ST7789_WHITE, .bg = ST7789_BLACK, .fill_color = ST7789_BLACK,
// //         .x = START_X, .y = 100, .scale = 3, .rotation = 3,
// //     });

// //     while (1)
// //     {
// //         memset(rx_buf, 0, BUF_SIZE);

// //         spi_slave_transaction_t t = {
// //             .length    = BUF_SIZE * 8,
// //             .tx_buffer = tx_buf,
// //             .rx_buffer = rx_buf,
// //         };

// //         esp_err_t ret = spi_slave_transmit(SPI_SLAVE_HOST, &t, pdMS_TO_TICKS(3000));

// //         if (ret == ESP_ERR_TIMEOUT) continue;
// //         if (ret != ESP_OK)          continue;

// //         /* Null-terminate and sanitise — replace non-printable with '.' */
// //         rx_buf[BUF_SIZE - 1] = '\0';
// //         for (int i = 0; i < BUF_SIZE - 1; i++) {
// //             if (rx_buf[i] == '\0') break;
// //             if (rx_buf[i] < 32 || rx_buf[i] > 126) rx_buf[i] = '.';
// //         }

// //         ESP_LOGI(TAG, "RX: %s", rx_buf);

// //         print_raw(disp, (char *)rx_buf);

// //         snprintf((char *)tx_buf, BUF_SIZE, "ACK");
// //     }
// // }


// // /* ── app_main ── */
// // void app_main(void)
// // {
// //     gpio_reset_pin(TFT_BLK);
// //     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
// //     gpio_set_level(TFT_BLK, 0);

// //     spi_bus_config_t buscfg = {
// //         .mosi_io_num     = TFT_MOSI,
// //         .miso_io_num     = -1,
// //         .sclk_io_num     = TFT_SCLK,
// //         .quadwp_io_num   = -1,
// //         .quadhd_io_num   = -1,
// //         .max_transfer_sz = 4096,
// //     };
// //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

// //     st7789_config_t disp_cfg = {
// //         .spi_host = SPI2_HOST,
// //         .cs_gpio  = TFT_CS,
// //         .dc_gpio  = TFT_DC,
// //         .rst_gpio = TFT_RST,
// //         .width    = 240,
// //         .height   = 320,
// //         .rotation = 0,
// //     };
// //     st7789_handle_t display = st7789_init(&disp_cfg);

// //     gpio_set_level(TFT_BLK, 1);

// //     xTaskCreate(spi_slave_task, "spi_slave", 5120, display, 5, NULL);

// //     while (1) vTaskDelay(pdMS_TO_TICKS(5000));
// // }
