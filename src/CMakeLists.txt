# src/CMakeLists.txt
add_executable(main
    main.cpp
    http_weather/http_weather.c
    http_weather/ntp_sync/ntp_client.c
    picow_lcd/display.c
    picow_lcd/lcd_sdk/lcd.c
    picow_lcd/lcd_sdk/icon.c
    picow_lcd/lcd_sdk/font.c
)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/picow_lcd
    ${CMAKE_CURRENT_LIST_DIR}/picow_lcd/lcd_sdk
    ${CMAKE_CURRENT_LIST_DIR}/http_weather
    ${CMAKE_CURRENT_LIST_DIR}/http_weather/json
    ${CMAKE_CURRENT_LIST_DIR}/http_weather/ntp_sync
)

target_link_libraries(main 
    pico_stdlib
    pico_stdio_usb  # usb port connect -> /dev/ttyACM0, if you want chk terminal print
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_spi
    hardware_gpio
)

pico_add_extra_outputs(main) # after build make additional binary files