idf_component_register(
    SRCS
        "lcd_1602.c"
        "internal/lcd_i2c.c"
    INCLUDE_DIRS
        "include"
    PRIV_INCLUDE_DIRS
        "internal"
    REQUIRES
        esp_driver_i2c freertos project_config
)
