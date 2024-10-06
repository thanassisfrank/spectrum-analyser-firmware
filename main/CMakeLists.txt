idf_component_register(
    SRCS "main.c" "app.c" "input.c" "display.c" "receiver.c" "gui.c" 
    INCLUDE_DIRS "."
    REQUIRES esp_lcd esp_adc u8g2 u8g2_hal lua ws2812
)

