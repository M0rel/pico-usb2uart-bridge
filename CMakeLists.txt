# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "./pico-sdk")
#set(TINYUSB_PATH "${PICO_SDK_PATH}/lib/tinyusb")
set(TINYUSB_PATH "./tinyusb")

set(PICO_BOARD pico)

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Objects for a common tiny usb interface
add_library(tinyusb_common_base_cust INTERFACE)
target_sources(tinyusb_common_base_cust INTERFACE
  ${TINYUSB_PATH}/src/tusb.c
  ${TINYUSB_PATH}/src/common/tusb_fifo.c
)

target_include_directories(tinyusb_common_base_cust INTERFACE
  ${TINYUSB_PATH}/src
  ${TINYUSB_PATH}/src/common
)

target_link_libraries(tinyusb_common_base_cust INTERFACE
  hardware_structs
  hardware_irq
  hardware_resets
  pico_sync
)

set(TINYUSB_DEBUG_LEVEL 0)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Compiling TinyUSB with CFG_TUSB_DEBUG=1")
  set(TINYUSB_DEBUG_LEVEL 1)
endif()

target_compile_definitions(tinyusb_common_base_cust INTERFACE
  CFG_TUSB_MCU=OPT_MCU_RP2040
  CFG_TUSB_OS=OPT_OS_PICO
  CFG_TUSB_DEBUG=${TINYUSB_DEBUG_LEVEL}
)


# Objects for a USB host specific tiny USB APIS
add_library(tinyusb_device_base_cust INTERFACE)
target_sources(tinyusb_device_base_cust INTERFACE
  ${TINYUSB_PATH}/src/portable/raspberrypi/rp2040/hcd_rp2040.c
  ${TINYUSB_PATH}/src/portable/raspberrypi/rp2040/rp2040_usb.c
  ${TINYUSB_PATH}/src/host/usbh.c
  ${TINYUSB_PATH}/src/host/hub.c
  ${TINYUSB_PATH}/src/class/cdc/cdc_host.c
  ${TINYUSB_PATH}/src/class/hid/hid_host.c
  ${TINYUSB_PATH}/src/class/msc/msc_host.c
  ${TINYUSB_PATH}/src/class/hid/hid_host.c
)

# Objects for a device specific stuff
add_library(tinyusb_bsp_cust INTERFACE)
target_sources(tinyusb_bsp_cust INTERFACE
        ${TINYUSB_PATH}/hw/bsp/rp2040/family.c
	)
target_include_directories(tinyusb_bsp_cust INTERFACE
        ${TINYUSB_PATH}/hw
        ${TINYUSB_PATH}/hw/bsp/rp2040
	)
target_link_libraries(tinyusb_bsp_cust	INTERFACE pico_unique_id)


# Main project
project(pico-usb2uart-bridge)

pico_sdk_init()

add_executable(pico-usb2uart-bridge
        main.c
        hid_app.c
        cdc_app.c
        )

target_link_libraries(pico-usb2uart-bridge PUBLIC
        pico_stdlib
        hardware_uart
        tinyusb_bsp_cust
        tinyusb_common_base_cust
        tinyusb_device_base_cust
        )

target_include_directories(pico-usb2uart-bridge PUBLIC
        ./
        )

pico_enable_stdio_uart(pico-usb2uart-bridge 0)

pico_add_extra_outputs(pico-usb2uart-bridge)

