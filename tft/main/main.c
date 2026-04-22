// // // /*
// // //  * main.c – ESP32 SPI Slave + ST7789 Display
// // //  *
// // //  * Packet from STM32 (64 bytes):
// // //  *  [0]       = 0xA5  start marker
// // //  *  [1]       = payload length (n)
// // //  *  [2..n+1]  = RF payload (ASCII)
// // //  *  [n+2]     = RSSI raw byte
// // //  *  [n+3]     = RSSI dBm low byte  (int16, little-endian)
// // //  *  [n+4]     = RSSI dBm high byte
// // //  *  rest      = 0x00
// // //  */

// // // #include <stdio.h>
// // // #include <string.h>
// // // #include <stdbool.h>

// // // #include "freertos/FreeRTOS.h"
// // // #include "freertos/task.h"
// // // #include "freertos/semphr.h"

// // // #include "driver/spi_master.h"
// // // #include "driver/spi_slave.h"
// // // #include "driver/gpio.h"

// // // #include "esp_log.h"
// // // #include "esp_heap_caps.h"

// // // #include "display.h"

// // // static const char *TAG = "MAIN";

// // // /* ───────── Display pins (SPI2 MASTER → ST7789) ───────── */
// // // #define TFT_BLK   5
// // // #define TFT_SCLK  18
// // // #define TFT_MOSI  23
// // // #define TFT_DC    15
// // // #define TFT_CS    2
// // // #define TFT_RST   26

// // // /* ───────── SPI Slave pins (SPI3 ← STM32) ───────── */
// // // #define SPI_SLAVE_HOST  SPI3_HOST
// // // #define PIN_MOSI        13
// // // #define PIN_MISO        12
// // // #define PIN_CLK         14
// // // #define PIN_CS          19

// // // #define BUF_SIZE        64

// // // /* ───────── Layout (landscape 320×240) ───────── */
// // // #define COL_X           8
// // // #define Y_TITLE         6
// // // #define Y_DIVIDER       42
// // // #define Y_DATA_LABEL    55
// // // #define Y_DATA_VAL      80
// // // #define Y_RSSI_LABEL    150
// // // #define Y_RSSI_VAL      175
// // // #define Y_RAW_VAL       215

// // // #define SCALE_BIG       4
// // // #define SCALE_MED       3
// // // #define SCALE_SM        2

// // // #define PAD_DATA        14
// // // #define PAD_RSSI        12
// // // #define PAD_RAW         18

// // // /* ───────── Colors ───────── */
// // // #define C_FG    ST7789_WHITE
// // // #define C_BG    ST7789_BLACK
// // // #define C_DIM   ST7789_COLOR(110, 110, 110)
// // // #define C_GREEN ST7789_GREEN
// // // #define C_CYAN  ST7789_CYAN
// // // #define C_YEL   ST7789_YELLOW

// // // /* ───────── Shared state (written by SPI task, read by display task) ───────── */
// // // typedef struct {
// // //     char    payload[64];
// // //     char    rssi_str[20];
// // //     char    raw_str[16];
// // //     bool    valid;
// // //     uint32_t updated_at;   /* xTaskGetTickCount() */
// // // } rf_frame_t;

// // // static rf_frame_t s_frame;
// // // static SemaphoreHandle_t s_frame_mutex;

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * pad_right – write src into dst, space-pad to 'len', null-term
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // static void pad_right(char *dst, const char *src, int len)
// // // {
// // //     int n = (int)strlen(src);
// // //     if (n > len) n = len;
// // //     memcpy(dst, src, n);
// // //     for (int i = n; i < len; i++) dst[i] = ' ';
// // //     dst[len] = '\0';
// // // }

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * field_print – only redraws when content changed
// // //  * prev_buf must be at least (len+1) bytes
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // static void field_print(st7789_handle_t disp,
// // //                         const char *new_val, char *prev_buf,
// // //                         int x, int y, int scale, int pad,
// // //                         uint16_t fg)
// // // {
// // //     char padded[64];
// // //     pad_right(padded, new_val, pad);

// // //     if (strcmp(padded, prev_buf) == 0) return;   /* no change */

// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text       = padded,
// // //         .show_value = false,
// // //         .fg         = fg,
// // //         .bg         = C_BG,
// // //         .fill_color = C_BG,
// // //         .x          = x,
// // //         .y          = y,
// // //         .scale      = scale,
// // //         .rotation   = 3,   /* landscape */
// // //     });

// // //     strncpy(prev_buf, padded, pad);
// // //     prev_buf[pad] = '\0';
// // // }

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * draw_static_ui – labels and title; called once
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // static void draw_static_ui(st7789_handle_t disp)
// // // {
// // //     /* Full black screen */
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = " ", .fg = C_BG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = 0, .y = 0, .scale = 1, .rotation = 3,
// // //     });

// // //     /* Title */
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "UHF RECEIVER",
// // //         .fg = C_CYAN, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_TITLE, .scale = SCALE_MED, .rotation = 3,
// // //     });

// // //     /* Divider (drawn as dashes) */
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "------------------------------",
// // //         .fg = C_DIM, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_DIVIDER, .scale = 1, .rotation = 3,
// // //     });

// // //     /* DATA label */
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "DATA:",
// // //         .fg = C_FG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_DATA_LABEL, .scale = SCALE_SM, .rotation = 3,
// // //     });

// // //     /* RSSI label */
// // //     lcd_print(disp, &(lcd_print_config_t){
// // //         .text = "RSSI:",
// // //         .fg = C_FG, .bg = C_BG, .fill_color = C_BG,
// // //         .x = COL_X, .y = Y_RSSI_LABEL, .scale = SCALE_SM, .rotation = 3,
// // //     });
// // // }

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * display_task – runs at low priority, polls shared frame
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // static void display_task(void *arg)
// // // {
// // //     st7789_handle_t disp = (st7789_handle_t)arg;

// // //     draw_static_ui(disp);

// // //     /* Prev-value buffers for diff-draw */
// // //     char prev_data[PAD_DATA + 1];  memset(prev_data, 0, sizeof(prev_data));
// // //     char prev_rssi[PAD_RSSI + 1];  memset(prev_rssi, 0, sizeof(prev_rssi));
// // //     char prev_raw [PAD_RAW  + 1];  memset(prev_raw,  0, sizeof(prev_raw));

// // //     bool showing_no_data = false;
// // //     uint32_t last_valid  = 0;
// // //     const uint32_t NO_DATA_TICKS = pdMS_TO_TICKS(3000);

// // //     while (1)
// // //     {
// // //         rf_frame_t local;
// // //         xSemaphoreTake(s_frame_mutex, portMAX_DELAY);
// // //         local = s_frame;
// // //         xSemaphoreGive(s_frame_mutex);

// // //         uint32_t now = xTaskGetTickCount();

// // //         if (local.valid && (now - local.updated_at) < NO_DATA_TICKS)
// // //         {
// // //             last_valid = local.updated_at;
// // //             showing_no_data = false;

// // //             field_print(disp, local.payload,  prev_data,
// // //                         COL_X, Y_DATA_VAL, SCALE_BIG, PAD_DATA, C_GREEN);
// // //             field_print(disp, local.rssi_str, prev_rssi,
// // //                         COL_X, Y_RSSI_VAL, SCALE_BIG, PAD_RSSI, C_YEL);
// // //             field_print(disp, local.raw_str,  prev_raw,
// // //                         COL_X, Y_RAW_VAL,  SCALE_SM,  PAD_RAW,  C_DIM);
// // //         }
// // //         else if (!showing_no_data && (now - last_valid) > NO_DATA_TICKS)
// // //         {
// // //             showing_no_data = true;
// // //             field_print(disp, "-- no data --", prev_data,
// // //                         COL_X, Y_DATA_VAL, SCALE_BIG, PAD_DATA, C_DIM);
// // //             field_print(disp, "---",          prev_rssi,
// // //                         COL_X, Y_RSSI_VAL, SCALE_BIG, PAD_RSSI, C_DIM);
// // //             field_print(disp, "RAW --",       prev_raw,
// // //                         COL_X, Y_RAW_VAL,  SCALE_SM,  PAD_RAW,  C_DIM);
// // //         }

// // //         vTaskDelay(pdMS_TO_TICKS(100));   /* refresh cap: 10 fps */
// // //     }
// // // }

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * spi_slave_task – receives frames from STM32, updates s_frame
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // static void spi_slave_task(void *arg)
// // // {
// // //     /* ── SPI bus ── */
// // //     spi_bus_config_t bus_cfg = {
// // //         .mosi_io_num   = PIN_MOSI,
// // //         .miso_io_num   = PIN_MISO,
// // //         .sclk_io_num   = PIN_CLK,
// // //         .quadwp_io_num = -1,
// // //         .quadhd_io_num = -1,
// // //     };

// // //     /* ── Slave interface – MUST match STM32 SPI1 config ──
// // //      *   CPOL=0, CPHA=0  (SPI_POLARITY_LOW + SPI_PHASE_1EDGE = Mode 0) */
// // //     spi_slave_interface_config_t slave_cfg = {
// // //         .mode         = 0,
// // //         .spics_io_num = PIN_CS,
// // //         .queue_size   = 4,
// // //     };

// // //     /* Pull-ups for bus stability */
// // //     gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
// // //     gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);
// // //     gpio_set_pull_mode(PIN_CS,   GPIO_PULLUP_ONLY);

// // //     ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg,
// // //                                          &slave_cfg, SPI_DMA_CH_AUTO));
// // //     ESP_LOGI(TAG, "SPI slave ready (SPI3)");

// // //     /* DMA-aligned buffers */
// // //     uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// // //     uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// // //     if (!tx_buf || !rx_buf) {
// // //         ESP_LOGE(TAG, "DMA alloc failed");
// // //         vTaskDelete(NULL);
// // //         return;
// // //     }

// // //     memset(tx_buf, 0, BUF_SIZE);
// // //     snprintf((char *)tx_buf, BUF_SIZE, "ESP32 READY");

// // //     uint32_t pkt_ok  = 0;
// // //     uint32_t pkt_bad = 0;

// // //     while (1)
// // //     {
// // //         memset(rx_buf, 0, BUF_SIZE);

// // //         spi_slave_transaction_t t = {
// // //             .length    = BUF_SIZE * 8,
// // //             .tx_buffer = tx_buf,
// // //             .rx_buffer = rx_buf,
// // //         };

// // //         esp_err_t ret = spi_slave_transmit(SPI_SLAVE_HOST, &t,
// // //                                            pdMS_TO_TICKS(2000));

// // //         if (ret == ESP_ERR_TIMEOUT) {
// // //             ESP_LOGD(TAG, "SPI timeout – no master activity");
// // //             continue;
// // //         }
// // //         if (ret != ESP_OK) {
// // //             ESP_LOGW(TAG, "SPI error: %s", esp_err_to_name(ret));
// // //             vTaskDelay(pdMS_TO_TICKS(10));
// // //             continue;
// // //         }

// // //         /* ── Validate start marker ── */
// // //         if (rx_buf[0] != 0xA5) {
// // //             pkt_bad++;
// // //             ESP_LOGD(TAG, "Bad marker 0x%02X (bad=%lu)", rx_buf[0], pkt_bad);
// // //             continue;
// // //         }

// // //         /* ── Parse length ── */
// // //         uint8_t plen = rx_buf[1];
// // //         if (plen == 0 || plen > (BUF_SIZE - 5)) {
// // //             ESP_LOGD(TAG, "Bad plen=%u", plen);
// // //             continue;
// // //         }

// // //         /* ── Validate ASCII payload ── */
// // //         bool ascii_ok = true;
// // //         for (int i = 0; i < plen; i++) {
// // //             uint8_t c = rx_buf[2 + i];
// // //             if (c < 32 || c > 126) { ascii_ok = false; break; }
// // //         }
// // //         if (!ascii_ok) {
// // //             ESP_LOGD(TAG, "Non-ASCII payload, skipping");
// // //             continue;
// // //         }

// // //         /* ── Extract fields ── */
// // //         char payload[64] = {0};
// // //         memcpy(payload, &rx_buf[2], plen);

// // //         uint8_t rssi_raw = rx_buf[2 + plen];
// // //         int16_t rssi_dbm = (int16_t)(
// // //             (uint16_t)rx_buf[3 + plen] |
// // //             ((uint16_t)rx_buf[4 + plen] << 8)
// // //         );

// // //         pkt_ok++;
// // //         ESP_LOGI(TAG, "[%lu] \"%s\"  RSSI=%d dBm  raw=0x%02X",
// // //                  pkt_ok, payload, rssi_dbm, rssi_raw);

// // //         /* ── Update shared frame ── */
// // //         xSemaphoreTake(s_frame_mutex, portMAX_DELAY);
// // //         strncpy(s_frame.payload, payload, sizeof(s_frame.payload) - 1);
// // //         snprintf(s_frame.rssi_str, sizeof(s_frame.rssi_str),
// // //                  "%d dBm", rssi_dbm);
// // //         snprintf(s_frame.raw_str, sizeof(s_frame.raw_str),
// // //                  "RAW 0x%02X", rssi_raw);
// // //         s_frame.valid      = true;
// // //         s_frame.updated_at = xTaskGetTickCount();
// // //         xSemaphoreGive(s_frame_mutex);

// // //         /* ACK back to STM32 on next transfer */
// // //         snprintf((char *)tx_buf, BUF_SIZE, "ACK %lu", pkt_ok);
// // //     }
// // // }

// // // /* ═══════════════════════════════════════════════════════════════
// // //  * app_main
// // //  * ═══════════════════════════════════════════════════════════════ */
// // // void app_main(void)
// // // {
// // //     ESP_LOGI(TAG, "System start");

// // //     /* ── Shared state init ── */
// // //     memset(&s_frame, 0, sizeof(s_frame));
// // //     s_frame_mutex = xSemaphoreCreateMutex();
// // //     configASSERT(s_frame_mutex);

// // //     /* ── Backlight off during init ── */
// // //     gpio_reset_pin(TFT_BLK);
// // //     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
// // //     gpio_set_level(TFT_BLK, 0);

// // //     /* ── SPI bus for display (MASTER) ── */
// // //     spi_bus_config_t buscfg = {
// // //         .mosi_io_num     = TFT_MOSI,
// // //         .miso_io_num     = -1,
// // //         .sclk_io_num     = TFT_SCLK,
// // //         .quadwp_io_num   = -1,
// // //         .quadhd_io_num   = -1,
// // //         .max_transfer_sz = 4096,
// // //     };
// // //     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

// // //     /* ── Display init ── */
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

// // //     /* ── Backlight on ── */
// // //     gpio_set_level(TFT_BLK, 1);
// // //     ESP_LOGI(TAG, "Display ready");

// // //     /* ── Tasks ──
// // //      *   display_task : priority 3, 4 kB  – does all lcd_print calls
// // //      *   spi_slave_task : priority 5, 5 kB – receives from STM32
// // //      *
// // //      *   SPI slave has higher priority so it never misses a CS edge.
// // //      *   Display task runs whenever slave is waiting for a packet.       */
// // //     xTaskCreate(display_task,   "display",   4096, display, 3, NULL);
// // //     xTaskCreate(spi_slave_task, "spi_slave", 5120, NULL,    5, NULL);

// // //     /* Main loop idles */
// // //     while (1) {
// // //         vTaskDelay(pdMS_TO_TICKS(5000));
// // //     }
// // // }




// // /*
// //  * main.c – ESP32 SPI Slave + ST7789  (single-line, no-flicker)
// //  */

// // #include <stdio.h>
// // #include <string.h>
// // #include <stdbool.h>

// // #include "freertos/FreeRTOS.h"
// // #include "freertos/task.h"
// // #include "freertos/semphr.h"

// // #include "driver/spi_master.h"
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

// // /* ── SPI Slave pins ── */
// // #define SPI_SLAVE_HOST  SPI3_HOST
// // #define PIN_MOSI        13
// // #define PIN_MISO        12
// // #define PIN_CLK         14
// // #define PIN_CS          19

// // #define BUF_SIZE 64

// // /* ── Shared state ── */
// // typedef struct {
// //     char    line[64];       /* ready-to-print string */
// //     bool    updated;
// // } rf_frame_t;

// // static rf_frame_t   s_frame;
// // static SemaphoreHandle_t s_mutex;

// // /* ═══════════════════════════════════════
// //  * display_task – one lcd_print per cycle
// //  * ═══════════════════════════════════════ */
// // static void display_task(void *arg)
// // {
// //     st7789_handle_t disp = (st7789_handle_t)arg;

// //     char prev_line[64] = {0};

// //     /* draw black screen once */
// //     lcd_print(disp, &(lcd_print_config_t){
// //         .text = " ", .fg = ST7789_BLACK, .bg = ST7789_BLACK,
// //         .fill_color = ST7789_BLACK,
// //         .x = 0, .y = 0, .scale = 1, .rotation = 3,
// //     });

// //     while (1)
// //     {
// //         char local[64];

// //         xSemaphoreTake(s_mutex, portMAX_DELAY);
// //         memcpy(local, s_frame.line, sizeof(local));
// //         xSemaphoreGive(s_mutex);

// //         /* only redraw when content changed */
// //         if (strcmp(local, prev_line) != 0)
// //         {
// //             /* pad with spaces to overwrite previous longer text */
// //             char padded[64];
// //             int n = strlen(local);
// //             memcpy(padded, local, n);
// //             for (int i = n; i < 63; i++) padded[i] = ' ';
// //             padded[63] = '\0';

// //             lcd_print(disp, &(lcd_print_config_t){
// //                 .text       = padded,
// //                 .show_value = false,
// //                 .fg         = ST7789_WHITE,
// //                 .bg         = ST7789_BLACK,
// //                 .fill_color = ST7789_BLACK,
// //                 .x          = 8,
// //                 .y          = 30,          /* "up 30" */
// //                 .scale      = 2,
// //                 .rotation   = 3,
// //             });

// //             memcpy(prev_line, local, sizeof(prev_line));
// //         }

// //         vTaskDelay(pdMS_TO_TICKS(120));
// //     }
// // }

// // /* ═══════════════════════════════════════
// //  * spi_slave_task – parse → build one line
// //  * ═══════════════════════════════════════ */
// // static void spi_slave_task(void *arg)
// // {
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

// //     ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg,
// //                                          &slave_cfg, SPI_DMA_CH_AUTO));

// //     uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// //     uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
// //     if (!tx_buf || !rx_buf) { vTaskDelete(NULL); return; }

// //     memset(tx_buf, 0, BUF_SIZE);
// //     snprintf((char *)tx_buf, BUF_SIZE, "READY");

// //     /* seed display with "no data" */
// //     xSemaphoreTake(s_mutex, portMAX_DELAY);
// //     snprintf(s_frame.line, sizeof(s_frame.line), "data: ---, rssi: ---");
// //     xSemaphoreGive(s_mutex);

// //     uint32_t pkt = 0;

// //     while (1)
// //     {
// //         memset(rx_buf, 0, BUF_SIZE);

// //         spi_slave_transaction_t t = {
// //             .length    = BUF_SIZE * 8,
// //             .tx_buffer = tx_buf,
// //             .rx_buffer = rx_buf,
// //         };

// //         esp_err_t ret = spi_slave_transmit(SPI_SLAVE_HOST, &t,
// //                                            pdMS_TO_TICKS(2000));

// //         if (ret == ESP_ERR_TIMEOUT) {
// //             /* show no-data after 2 s silence */
// //             xSemaphoreTake(s_mutex, portMAX_DELAY);
// //             snprintf(s_frame.line, sizeof(s_frame.line),
// //                      "data: ---, rssi: ---");
// //             xSemaphoreGive(s_mutex);
// //             continue;
// //         }
// //         if (ret != ESP_OK) continue;

// //         /* validate */
// //         if (rx_buf[0] != 0xA5) continue;

// //         uint8_t plen = rx_buf[1];
// //         if (plen == 0 || plen > BUF_SIZE - 5) continue;

// //         /* ASCII check */
// //         bool ok = true;
// //         for (int i = 0; i < plen; i++) {
// //             if (rx_buf[2+i] < 32 || rx_buf[2+i] > 126) { ok = false; break; }
// //         }
// //         if (!ok) continue;

// //         char payload[64] = {0};
// //         memcpy(payload, &rx_buf[2], plen);

// //         int16_t rssi_dbm = (int16_t)(
// //             (uint16_t)rx_buf[3 + plen] |
// //             ((uint16_t)rx_buf[4 + plen] << 8)
// //         );

// //         pkt++;
// //         snprintf((char *)tx_buf, BUF_SIZE, "ACK %lu", pkt);

// //         /* ── single line, one atomic write ── */
// //         xSemaphoreTake(s_mutex, portMAX_DELAY);
// //         snprintf(s_frame.line, sizeof(s_frame.line),
// //                  "data: %s, rssi: %d", payload, (int)rssi_dbm);
// //         xSemaphoreGive(s_mutex);
// //     }
// // }

// // /* ═══════════════════════════════════════
// //  * app_main
// //  * ═══════════════════════════════════════ */
// // void app_main(void)
// // {
// //     s_mutex = xSemaphoreCreateMutex();
// //     configASSERT(s_mutex);
// //     memset(&s_frame, 0, sizeof(s_frame));

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

// //     xTaskCreate(display_task,   "display",   4096, display, 3, NULL);
// //     xTaskCreate(spi_slave_task, "spi_slave", 5120, NULL,    5, NULL);

// //     while (1) vTaskDelay(pdMS_TO_TICKS(5000));
// // }


// /*
//  * main.c – ESP32 SPI Slave + ST7789  (single-line, no-flicker)
//  */

// #include <stdio.h>
// #include <string.h>
// #include <stdbool.h>

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"

// #include "driver/spi_master.h"
// #include "driver/spi_slave.h"
// #include "driver/gpio.h"

// #include "esp_log.h"
// #include "esp_heap_caps.h"

// #include "display.h"

// /* ── Display pins ── */
// #define TFT_BLK   5
// #define TFT_SCLK  18
// #define TFT_MOSI  23
// #define TFT_DC    15
// #define TFT_CS    2
// #define TFT_RST   26

// /* ── SPI Slave pins ── */
// #define SPI_SLAVE_HOST  SPI3_HOST
// #define PIN_MOSI        13
// #define PIN_MISO        12
// #define PIN_CLK         14
// #define PIN_CS          19

// #define BUF_SIZE 64

// /* ── Shared state ── */
// typedef struct {
//     char line[96];   /* payload(40) + "data: , rssi: -32768" overhead */
//     bool updated;
// } rf_frame_t;

// static rf_frame_t        s_frame;
// static SemaphoreHandle_t s_mutex;

// /* ═══════════════════════════════════════
//  * display_task – one lcd_print per cycle
//  * ═══════════════════════════════════════ */
// static void display_task(void *arg)
// {
//     st7789_handle_t disp = (st7789_handle_t)arg;

//     char prev_line[96] = {0};

//     /* black screen once on boot */
//     lcd_print(disp, &(lcd_print_config_t){
//         .text       = " ",
//         .fg         = ST7789_BLACK,
//         .bg         = ST7789_BLACK,
//         .fill_color = ST7789_BLACK,
//         .x          = 0,
//         .y          = 0,
//         .scale      = 1,
//         .rotation   = 3,
//     });

//     while (1)
//     {
//         char local[96];

//         xSemaphoreTake(s_mutex, portMAX_DELAY);
//         memcpy(local, s_frame.line, sizeof(local));
//         xSemaphoreGive(s_mutex);

//         /* only redraw when content actually changed */
//         if (strcmp(local, prev_line) != 0)
//         {
//             /* space-pad to fixed width so old longer text is erased */
//             char padded[96];
//             int  n = (int)strlen(local);
//             memcpy(padded, local, n);
//             for (int i = n; i < 95; i++) padded[i] = ' ';
//             padded[95] = '\0';

//             lcd_print(disp, &(lcd_print_config_t){
//                 .text       = padded,
//                 .show_value = false,
//                 .fg         = ST7789_WHITE,
//                 .bg         = ST7789_BLACK,
//                 .fill_color = ST7789_BLACK,
//                 .x          = 8,
//                 .y          = 30,
//                 .scale      = 2,
//                 .rotation   = 3,
//             });

//             memcpy(prev_line, local, sizeof(prev_line));
//         }

//         vTaskDelay(pdMS_TO_TICKS(120));
//     }
// }

// /* ═══════════════════════════════════════
//  * spi_slave_task – parse → build one line
//  * ═══════════════════════════════════════ */
// static void spi_slave_task(void *arg)
// {
//     spi_bus_config_t bus_cfg = {
//         .mosi_io_num   = PIN_MOSI,
//         .miso_io_num   = PIN_MISO,
//         .sclk_io_num   = PIN_CLK,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//     };

//     spi_slave_interface_config_t slave_cfg = {
//         .mode         = 0,      /* CPOL=0 CPHA=0 – must match STM32 */
//         .spics_io_num = PIN_CS,
//         .queue_size   = 4,
//     };

//     gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
//     gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);
//     gpio_set_pull_mode(PIN_CS,   GPIO_PULLUP_ONLY);

//     ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg,
//                                          &slave_cfg, SPI_DMA_CH_AUTO));

//     uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
//     uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);

//     if (!tx_buf || !rx_buf) { vTaskDelete(NULL); return; }

//     memset(tx_buf, 0, BUF_SIZE);
//     snprintf((char *)tx_buf, BUF_SIZE, "READY");

//     /* seed display – shown until first valid packet */
//     xSemaphoreTake(s_mutex, portMAX_DELAY);
//     snprintf(s_frame.line, sizeof(s_frame.line), "waiting...");
//     xSemaphoreGive(s_mutex);

//     uint32_t pkt = 0;

//     while (1)
//     {
//         memset(rx_buf, 0, BUF_SIZE);

//         spi_slave_transaction_t t = {
//             .length    = BUF_SIZE * 8,
//             .tx_buffer = tx_buf,
//             .rx_buffer = rx_buf,
//         };

//         esp_err_t ret = spi_slave_transmit(SPI_SLAVE_HOST, &t,
//                                            pdMS_TO_TICKS(2000));

//         if (ret == ESP_ERR_TIMEOUT) continue;   /* keep last line as-is */
//         if (ret != ESP_OK)          continue;

//         /* validate start marker */
//         if (rx_buf[0] != 0xA5) continue;

//         /* validate length */
//         uint8_t plen = rx_buf[1];
//         if (plen == 0 || plen > BUF_SIZE - 5) continue;

//         /* ASCII check */
//         bool ok = true;
//         for (int i = 0; i < plen; i++) {
//             if (rx_buf[2 + i] < 32 || rx_buf[2 + i] > 126) {
//                 ok = false;
//                 break;
//             }
//         }
//         if (!ok) continue;

//         /* extract payload – cap at 40 chars for display */
//         char payload[41] = {0};
//         int  copy = plen < 40 ? plen : 40;
//         memcpy(payload, &rx_buf[2], copy);

//         /* extract rssi dBm (int16, little-endian) */
//         int16_t rssi_dbm = (int16_t)(
//             (uint16_t)rx_buf[3 + plen] |
//             ((uint16_t)rx_buf[4 + plen] << 8)
//         );

//         pkt++;
//         snprintf((char *)tx_buf, BUF_SIZE, "ACK %lu", pkt);
//         // pkt++;
//         snprintf((char *)tx_buf, BUF_SIZE, "ACK %lu", pkt);

//         /* ── serial monitor print ── */
//         ESP_LOGI("RX", "[%lu] data: %s, rssi: %d dBm", pkt, payload, (int)rssi_dbm);

//         /* single atomic line update – display task picks it up */
//         xSemaphoreTake(s_mutex, portMAX_DELAY);
//         snprintf(s_frame.line, sizeof(s_frame.line),
//                  "data: %s, rssi: %d", payload, (int)rssi_dbm);
//         xSemaphoreGive(s_mutex);

//         /* single atomic line update – display task picks it up */
//         xSemaphoreTake(s_mutex, portMAX_DELAY);
//         snprintf(s_frame.line, sizeof(s_frame.line),
//                  "data: %s, rssi: %d", payload, (int)rssi_dbm);
//         xSemaphoreGive(s_mutex);
//     }
// }

// /* ═══════════════════════════════════════
//  * app_main
//  * ═══════════════════════════════════════ */
// void app_main(void)
// {
//     s_mutex = xSemaphoreCreateMutex();
//     configASSERT(s_mutex);
//     memset(&s_frame, 0, sizeof(s_frame));

//     /* backlight off during init */
//     gpio_reset_pin(TFT_BLK);
//     gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
//     gpio_set_level(TFT_BLK, 0);

//     /* SPI bus for display (master) */
//     spi_bus_config_t buscfg = {
//         .mosi_io_num     = TFT_MOSI,
//         .miso_io_num     = -1,
//         .sclk_io_num     = TFT_SCLK,
//         .quadwp_io_num   = -1,
//         .quadhd_io_num   = -1,
//         .max_transfer_sz = 4096,
//     };
//     ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

//     /* display init */
//     st7789_config_t disp_cfg = {
//         .spi_host = SPI2_HOST,
//         .cs_gpio  = TFT_CS,
//         .dc_gpio  = TFT_DC,
//         .rst_gpio = TFT_RST,
//         .width    = 240,
//         .height   = 240,
//         .rotation = 0,
//     };
//     st7789_handle_t display = st7789_init(&disp_cfg);

//     gpio_set_level(TFT_BLK, 1);

//     xTaskCreate(display_task,   "display",   4096, display, 3, NULL);
//     xTaskCreate(spi_slave_task, "spi_slave", 5120, NULL,    5, NULL);

//     while (1) vTaskDelay(pdMS_TO_TICKS(5000));
// }


/*
 * main.c – ESP32 SPI Slave + ST7789 + Firebase (multi-WiFi, non-blocking)
 */



#include <stdio.h>
#include <string.h>
#include <stdbool.h>
// #include "esp_tls.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_http_client.h"

#include "display.h"

/* ══════════════════════════════════════════════════
 * WiFi – add / remove SSIDs as needed
 * ══════════════════════════════════════════════════ */
typedef struct { const char *ssid; const char *pass; } wifi_cred_t;

static const wifi_cred_t WIFI_LIST[] = {
    { "Galaxyadi", "ciib1438"  },
    { "M31E761",   "cwzy3089"  },
    { "Pranjal",   "12345678"  },
    /* add more here if needed */
};
#define WIFI_COUNT  (sizeof(WIFI_LIST) / sizeof(WIFI_LIST[0]))

/* ══════════════════════════════════════════════════
 * Firebase
 * ══════════════════════════════════════════════════ */
#define FIREBASE_BASE  "https://uhfpixiq-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH  "tcMGYaoS3yfapNEjsql8suRTEmETxJLgENKeUa4T"
#define FIREBASE_NODE  "/rf_data.json"
#define FIREBASE_URL   FIREBASE_BASE FIREBASE_NODE "?auth=" FIREBASE_AUTH

/* ── Display pins ── */
#define TFT_BLK   5
#define TFT_SCLK  18
#define TFT_MOSI  23
#define TFT_DC    15
#define TFT_CS    2
#define TFT_RST   26

/* ── SPI Slave pins ── */
#define SPI_SLAVE_HOST  SPI3_HOST
#define PIN_MOSI        13
#define PIN_MISO        12
#define PIN_CLK         14
#define PIN_CS          19

#define BUF_SIZE 64

/* ── Shared display state ── */
typedef struct {
    char line[96];
    bool updated;
} rf_frame_t;

/* ── Firebase queue entry ── */
typedef struct {
    char payload[41];
    int  rssi_dbm;
} fb_entry_t;

static rf_frame_t        s_frame;
static SemaphoreHandle_t s_mutex;
static QueueHandle_t     s_fb_queue;
static volatile bool     s_wifi_connected = false;

/* ══════════════════════════════════════════════════
 * WiFi event handler
 * ══════════════════════════════════════════════════ */
static void wifi_event_handler(void *arg, esp_event_base_t base,
                                int32_t id, void *data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        s_wifi_connected = false;
        ESP_LOGW("WIFI", "Disconnected");
        /* reconnect handled by wifi_task */
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        s_wifi_connected = true;
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)data;
        ESP_LOGI("WIFI", "Connected  IP:" IPSTR,
                 IP2STR(&event->ip_info.ip));
    }
}

/* ══════════════════════════════════════════════════
 * wifi_task – tries each SSID in order, retries forever
 * Runs at lowest priority so it never affects SPI/display
 * ══════════════════════════════════════════════════ */
static void wifi_task(void *arg)
{
    /* one-time init */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                wifi_event_handler, NULL);

    while (1)
    {
        if (s_wifi_connected) {
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        /* try each network in order */
        for (int i = 0; i < (int)WIFI_COUNT; i++)
        {
            ESP_LOGI("WIFI", "Trying SSID: %s", WIFI_LIST[i].ssid);

            wifi_config_t wcfg = {0};
            strncpy((char *)wcfg.sta.ssid,     WIFI_LIST[i].ssid, 32);
            strncpy((char *)wcfg.sta.password,  WIFI_LIST[i].pass, 64);
            wcfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

            esp_wifi_disconnect();
            esp_wifi_set_config(WIFI_IF_STA, &wcfg);
            esp_wifi_start();
            esp_wifi_connect();

            /* wait up to 10 s for connection */
            for (int t = 0; t < 20 && !s_wifi_connected; t++)
                vTaskDelay(pdMS_TO_TICKS(500));

            if (s_wifi_connected) break;

            ESP_LOGW("WIFI", "Failed: %s", WIFI_LIST[i].ssid);
        }

        if (!s_wifi_connected) {
            ESP_LOGE("WIFI", "All networks failed – retry in 10 s");
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }
}

/* ══════════════════════════════════════════════════
 * firebase_task – dequeues entries, POSTs to DB
 * If WiFi/HTTP is down only THIS task is affected
 * ══════════════════════════════════════════════════ */
static void firebase_task(void *arg)
{
    fb_entry_t entry;

    while (1)
    {
        /* block until something arrives (or 5 s timeout) */
        if (xQueueReceive(s_fb_queue, &entry, pdMS_TO_TICKS(5000)) != pdTRUE)
            continue;

        if (!s_wifi_connected) {
            ESP_LOGW("FB", "No WiFi – entry dropped");
            continue;
        }

        /* build JSON body */
        char json[128];
        snprintf(json, sizeof(json),
                 "{\"data\":\"%s\",\"rssi\":%d}",
                 entry.payload, entry.rssi_dbm);

        // esp_http_client_config_t http_cfg = {
        //     .url                        = FIREBASE_URL,
        //     .method                     = HTTP_METHOD_POST,
        //     .timeout_ms                 = 5000,
        //     .skip_cert_common_name_check = true,
        // };


        esp_http_client_config_t http_cfg = {
    .url         = FIREBASE_URL,
    .method      = HTTP_METHOD_POST,
    .timeout_ms  = 5000,
    .use_global_ca_store = true,   /* ← replaces crt_bundle_attach */
};

        esp_http_client_handle_t client = esp_http_client_init(&http_cfg);
        if (!client) {
            ESP_LOGE("FB", "HTTP init failed");
            continue;
        }

        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, json, (int)strlen(json));

        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI("FB", "POST OK  status=%d  %s",
                     esp_http_client_get_status_code(client), json);
        } else {
            ESP_LOGW("FB", "POST failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
    }
}

/* ══════════════════════════════════════════════════
 * display_task
 * ══════════════════════════════════════════════════ */
static void display_task(void *arg)
{
    st7789_handle_t disp = (st7789_handle_t)arg;
    char prev_line[96] = {0};

    lcd_print(disp, &(lcd_print_config_t){
        .text = " ", .fg = ST7789_BLACK, .bg = ST7789_BLACK,
        .fill_color = ST7789_BLACK,
        .x = 0, .y = 0, .scale = 1, .rotation = 3,
    });

    while (1)
    {
        char local[96];
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        memcpy(local, s_frame.line, sizeof(local));
        xSemaphoreGive(s_mutex);

        if (strcmp(local, prev_line) != 0)
        {
            char padded[96];
            int  n = (int)strlen(local);
            memcpy(padded, local, n);
            for (int i = n; i < 95; i++) padded[i] = ' ';
            padded[95] = '\0';

            lcd_print(disp, &(lcd_print_config_t){
                .text       = padded,
                .show_value = false,
                .fg         = ST7789_WHITE,
                .bg         = ST7789_BLACK,
                .fill_color = ST7789_BLACK,
                .x          = 8,
                .y          = 30,
                .scale      = 2,
                .rotation   = 3,
            });

            memcpy(prev_line, local, sizeof(prev_line));
        }

        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

/* ══════════════════════════════════════════════════
 * spi_slave_task
 * ══════════════════════════════════════════════════ */
static void spi_slave_task(void *arg)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num   = PIN_MOSI,
        .miso_io_num   = PIN_MISO,
        .sclk_io_num   = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_slave_interface_config_t slave_cfg = {
        .mode         = 0,
        .spics_io_num = PIN_CS,
        .queue_size   = 4,
    };

    gpio_set_pull_mode(PIN_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_CLK,  GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_CS,   GPIO_PULLUP_ONLY);

    ESP_ERROR_CHECK(spi_slave_initialize(SPI_SLAVE_HOST, &bus_cfg,
                                         &slave_cfg, SPI_DMA_CH_AUTO));

    uint8_t *tx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
    uint8_t *rx_buf = heap_caps_aligned_alloc(32, BUF_SIZE, MALLOC_CAP_DMA);
    if (!tx_buf || !rx_buf) { vTaskDelete(NULL); return; }

    memset(tx_buf, 0, BUF_SIZE);
    snprintf((char *)tx_buf, BUF_SIZE, "READY");

    xSemaphoreTake(s_mutex, portMAX_DELAY);
    snprintf(s_frame.line, sizeof(s_frame.line), "waiting...");
    xSemaphoreGive(s_mutex);

    uint32_t pkt = 0;

    while (1)
    {
        memset(rx_buf, 0, BUF_SIZE);

        spi_slave_transaction_t t = {
            .length    = BUF_SIZE * 8,
            .tx_buffer = tx_buf,
            .rx_buffer = rx_buf,
        };

        esp_err_t ret = spi_slave_transmit(SPI_SLAVE_HOST, &t,
                                           pdMS_TO_TICKS(2000));

        if (ret == ESP_ERR_TIMEOUT) continue;
        if (ret != ESP_OK)          continue;

        if (rx_buf[0] != 0xA5) continue;

        uint8_t plen = rx_buf[1];
        if (plen == 0 || plen > BUF_SIZE - 5) continue;

        bool ok = true;
        for (int i = 0; i < plen; i++) {
            if (rx_buf[2 + i] < 32 || rx_buf[2 + i] > 126) {
                ok = false; break;
            }
        }
        if (!ok) continue;

        char payload[41] = {0};
        int  copy = plen < 40 ? plen : 40;
        memcpy(payload, &rx_buf[2], copy);

        int16_t rssi_dbm = (int16_t)(
            (uint16_t)rx_buf[3 + plen] |
            ((uint16_t)rx_buf[4 + plen] << 8)
        );

        pkt++;
        snprintf((char *)tx_buf, BUF_SIZE, "ACK %lu", pkt);

        /* serial monitor */
        ESP_LOGI("RX", "[%lu] data: %s, rssi: %d dBm",
                 pkt, payload, (int)rssi_dbm);

        /* display */
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        snprintf(s_frame.line, sizeof(s_frame.line),
                 "data: %s, rssi: %d", payload, (int)rssi_dbm);
        xSemaphoreGive(s_mutex);

        /* firebase – non-blocking, drop if queue full */
        fb_entry_t entry;
        strncpy(entry.payload, payload, sizeof(entry.payload) - 1);
        entry.payload[sizeof(entry.payload) - 1] = '\0';
        entry.rssi_dbm = (int)rssi_dbm;
        xQueueSend(s_fb_queue, &entry, 0);   /* 0 = never blocks */
    }
}

/* ══════════════════════════════════════════════════
 * app_main
 * ══════════════════════════════════════════════════ */
void app_main(void)
{
    s_mutex    = xSemaphoreCreateMutex();
    s_fb_queue = xQueueCreate(8, sizeof(fb_entry_t));
    configASSERT(s_mutex);
    configASSERT(s_fb_queue);
    memset(&s_frame, 0, sizeof(s_frame));

    /* backlight off during init */
    gpio_reset_pin(TFT_BLK);
    gpio_set_direction(TFT_BLK, GPIO_MODE_OUTPUT);
    gpio_set_level(TFT_BLK, 0);

    spi_bus_config_t buscfg = {
        .mosi_io_num     = TFT_MOSI,
        .miso_io_num     = -1,
        .sclk_io_num     = TFT_SCLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = 4096,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    st7789_config_t disp_cfg = {
        .spi_host = SPI2_HOST,
        .cs_gpio  = TFT_CS,
        .dc_gpio  = TFT_DC,
        .rst_gpio = TFT_RST,
        .width    = 240,
        .height   = 240,
        .rotation = 0,
    };
    st7789_handle_t display = st7789_init(&disp_cfg);

    gpio_set_level(TFT_BLK, 1);

    /* ── Task priority map ─────────────────────────────
     *  spi_slave  5  – highest, never misses a CS edge
     *  display    3  – screen refresh
     *  wifi       2  – background reconnect loop
     *  firebase   1  – lowest, HTTP stalls here only
     * ─────────────────────────────────────────────────*/
    xTaskCreate(display_task,   "display",   4096,  display, 3, NULL);
    xTaskCreate(spi_slave_task, "spi_slave", 5120,  NULL,    5, NULL);
    xTaskCreate(wifi_task,      "wifi",      4096,  NULL,    2, NULL);
    xTaskCreate(firebase_task,  "firebase",  8192,  NULL,    1, NULL);

    while (1) vTaskDelay(pdMS_TO_TICKS(5000));
}