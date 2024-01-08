add_executable(main
        main.c
        usock.c
        econsole.c
        )
target_compile_definitions(main PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(main PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(main
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        ePaper GUI Fonts Config
        )

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
pico_add_extra_outputs(main)

# add_executable(main
#         main.c
#         )
# target_compile_definitions(main PRIVATE
#         WIFI_SSID=\"${WIFI_SSID}\"
#         WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
#         )
# target_include_directories(main PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#         )
# target_link_libraries(main
#         pico_cyw43_arch_lwip_poll
#         pico_stdlib
#         )
# pico_enable_stdio_usb(main 1)
# pico_enable_stdio_uart(main 0)
# pico_add_extra_outputs(main)
