idf_component_register(
    SRCS "main.c" "lcd_1602.c" "internal/lcd_i2c.c"
    INCLUDE_DIRS "." "internal"
    REQUIRES driver
)