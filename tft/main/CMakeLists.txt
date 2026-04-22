idf_component_register(
    SRCS
        "main.c"
        "display.c"
    INCLUDE_DIRS "."
    REQUIRES
        driver
        esp_wifi
        esp_http_client
        nvs_flash
        esp_netif
        esp_event
        mbedtls
)