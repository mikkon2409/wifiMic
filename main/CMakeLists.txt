idf_component_register(SRCS "main.c"
                            "sdcard.c"
                            "rgb_led.c"
                            "input_gpio.c"
                            "wifi.c"
                            # "http_server.c"
                            "wav.c"
                            "ftp_client.c"
                       EMBED_FILES "index.html")