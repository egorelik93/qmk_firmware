/*
Copyright 2023 @ Nuphy <https://nuphy.com/>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define SYS_SIDE RIGHT_SIDE
#define BAT_SIDE RIGHT_SIDE
#define RF_SIDE LEFT_SIDE

#define USB_MODE 0
#define THREE_MODE 1
#define WORK_MODE THREE_MODE

#if !defined(NO_DEBUG) && !defined(CONSOLE_ENABLE)
#define NO_DEBUG
#endif // !NO_DEBUG

#if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
#define NO_PRINT
#endif // !NO_PRINT

#if !defined(NO_DEBUG) && defined(CONSOLE_ENABLE)
#define DEBUG_MATRIX_SCAN_RATE
#endif // DEBUG_MATRIX_SCAN_RATE

#define DYNAMIC_KEYMAP_MACRO_DELAY 8
#define TAPPING_TERM 200

#define DEV_MODE_PIN C0
#define SYS_MODE_PIN C1
#define DC_BOOST_PIN C2
#define NRF_RESET_PIN B4
#define NRF_TEST_PIN B5
#define NRF_WAKEUP_PIN B8

#define DRIVER_RGB_DI_PIN A7
#define DRIVER_LED_CS_PIN C6
#define DRIVER_SIDE_PIN C8
#define DRIVER_SIDE_CS_PIN C9

#define GPIO_INPUT_PIN_DELAY                12

#define OS_DETECTION_DEBOUNCE              250
#define OS_DETECTION_KEYBOARD_RESET

// remove unused QMK functions to save space
/*
#define NO_ACTION_ONESHOT
#undef LAYER_LOCK_ENABLE
#undef LEADER_ENABLE
#undef GRAVE_ESC_ENABLE
#undef SPACE_CADET_ENABLE
*/

#define SERIAL_DRIVER SD1
#define UART_TX_PIN B6
#define UART_TX_PAL_MODE 0
#define UART_RX_PIN B7
#define UART_RX_PAL_MODE 0

// JinCao
// #define EECONFIG_KB_DATA_SIZE 10 // match used byte size of kb_config_t + 1 (seems to need extra byte)

// ryodeushii
#ifdef VIA_ENABLE
#    define VIA_EEPROM_CUSTOM_CONFIG_SIZE 32 // 22 // sizeof via_config
     // Needed to satisfy static_assert in kb_util.h
#    define EECONFIG_KB_DATA_SIZE VIA_EEPROM_CUSTOM_CONFIG_SIZE
#elif defined(SIDE_SEPARATE)
#    define EECONFIG_KB_DATA_SIZE 30 // 20
#else
#    define EECONFIG_KB_DATA_SIZE 25 // 15
#endif
// #define EECONFIG_KB_DATA_SIZE               26

#define WS2812_TIMING 850
#define WS2812_T1H 700 // Width of a 1 bit in ns
#define WS2812_T1L (WS2812_TIMING - WS2812_T1H) // Width of a 1 bit in ns
#define WS2812_T0H 150 // Width of a 0 bit in ns
#define WS2812_T0L (WS2812_TIMING - WS2812_T0H)

#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 2
#define WS2812_PWM_PAL_MODE 1
#define WS2812_DMA_STREAM STM32_DMA1_STREAM3
#define WS2812_DMA_CHANNEL 3
// ryodeushii turned this to     800000
#define WS2812_PWM_TARGET_PERIOD 200000

#ifdef RGB_MATRIX_LED_COUNT
#    undef RGB_MATRIX_LED_COUNT
#endif
#define RGB_MATRIX_LED_COUNT 84 // sides 6 + 6, not included here

// #define DEBUG_MATRIX_SCAN_RATE

// NOTE: uncomment if you want to have random colors in Reactive RGB effects
// #define RGB_MATRIX_SOLID_REACTIVE_GRADIENT_MODE

#define DEBOUNCE_STEP 1

// USB sleep workaround :D
#ifdef USB_SUSPEND_WAKEUP_DELAY
#    undef USB_SUSPEND_WAKEUP_DELAY
#endif
#define USB_SUSPEND_WAKEUP_DELAY 50

// sleep timeout change step (minutes)
#define SLEEP_TIMEOUT_STEP 1
// #define LED_HITS_TO_REMEMBER 16
#define WS2812_SPI_USE_CIRCULAR_BUFFER

// debounce override - for clangd compliance - info_json.h - doesn't work most of the times
// See keyboard.json for debounce.
#define RELEASE_DEBOUNCE (DEBOUNCE)
// use dedicated timer for wait_us interrupts instead on ChibiOS defaulting to minimum 100us even if you sleep for 10us
#define WAIT_US_TIMER GPTD14

/*
 * DEFAULT VALUES FOR INITIAL CONFIG to allow override in user keymap
 */
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT
#define RGB_DEFAULT_COLOR 168

#define DEFAULT_SLEEP_MODE SLEEP_MODE_DEEP
#define DEFAULT_USB_SLEEP_TOGGLE false
#define DEFAULT_SLEEP_TIMEOUT 5
#define DEFAULT_CAPS_INDICATOR_TYPE CAPS_INDICATOR_SIDE
#define DEFAULT_BATTERY_INDICATOR_BRIGHTNESS 100
#define DEFAULT_LIGHT_CUSTOM_KEYS 0
// Numbers seem to come from original version of device_reset_init.
#define DEFAULT_SIDE_MODE 0
// Exception is this one; Original sets it to 3, JinCao to 1.
// I like 2.
#define DEFAULT_SIDE_LIGHT 2
//#define DEFAULT_SIDE_LIGHT 3
#define DEFAULT_SIDE_SPEED 2
#define DEFAULT_SIDE_RGB 1
#define DEFAULT_SIDE_COLOR 0
#define DEFAULT_RIGHT_SIDE_MODE 0
#define DEFAULT_RIGHT_SIDE_LIGHT 2
//#define DEFAULT_RIGHT_SIDE_BRIGHTNESS 3
#define DEFAULT_RIGHT_SIDE_SPEED 2
#define DEFAULT_RIGHT_SIDE_RGB 1
#define DEFAULT_RIGHT_SIDE_COLOR 0
#define DEFAULT_DETECT_NUMLOCK 0
#define DEFAULT_BATTERY_INDICATOR_NUMERIC 0
#define DEFAULT_SHOW_SOCD_INDICATOR 0
#define DEFAULT_RF_LINK_TIMEOUT LINK_TIMEOUT_ALT

#define DEFAULT_CAPS_WORD_ENABLE 0
#define DEFAULT_NUMLOCK_STATE 0
#define DEFAULT_RF_DELAY_STEP 2
#define DEFAULT_DEBOUNCE_TYPE 1
/*
 * END OF DEFAULT VALUES
 */
#define CFW_VERSION "put_version_here"
