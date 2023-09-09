# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(pico-usb2uart-bridge)

pico_sdk_init()

add_executable(pico-usb2uart-bridge
        main.c
        )

target_include_directories(pico-usb2uart-bridge PUBLIC
	./
	pico-sdk/lib/tinyusb/src)

target_link_libraries(pico-usb2uart-bridge
        pico_stdlib
        tinyusb_host
        tinyusb_board
        )

pico_add_extra_outputs(pico-usb2uart-bridge)
