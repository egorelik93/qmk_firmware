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

#include <stdint.h>
#include "quantum.h"
#include "color.h"
#include "quantum_keycodes.h"
#include "usb_main.h"

typedef enum {
    RX_Idle,
    RX_Receiving,
    RX_Done,
    RX_Fail,
    RX_OV_ERR,
    RX_SUM_ERR,
    RX_CMD_ERR,
    RX_DATA_ERR,
    RX_DATA_OV,
    RX_FORMAT_ERR,

    TX_OK = 0XE0,
    TX_DONE,
    TX_BUSY,
    TX_TIMEOUT,
    TX_DATA_ERR,

} TYPE_RX_STATE;

// clang-format off

typedef enum {
    CAPS_INDICATOR_SIDE = 0,
    CAPS_INDICATOR_UNDER_KEY,
    CAPS_INDICATOR_BOTH,
    CAPS_INDICATOR_OFF,
} CAPS_LOCK_INDICATION;

typedef enum { DEBOUNCE_PRESS = 0, DEBOUNCE_RELEASE } DEBOUNCE_EVENT;

#define RF_IDLE 0
#define RF_PAIRING 1
#define RF_LINKING 2
#define RF_CONNECT 3
#define RF_DISCONNECT 4
#define RF_SLEEP 5
#define RF_SNIF 6
#define RF_INVALID 0XFE
#define RF_ERR_STATE 0XFF
#define RF_WAKE 0XA5 // made this up

#define CMD_POWER_UP 0XF0
#define CMD_SLEEP 0XF1
#define CMD_HAND 0XF2
#define CMD_SNIF 0XF3
#define CMD_24G_SUSPEND 0XF4
#define CMD_IDLE_EXIT 0XFE

#define CMD_RPT_MS 0XE0
#define CMD_RPT_BYTE_KB 0XE1
#define CMD_RPT_BIT_KB 0XE2
#define CMD_RPT_CONSUME 0XE3
#define CMD_RPT_SYS 0XE4

#define CMD_SET_LINK 0XC0
#define CMD_SET_CONFIG 0XC1
#define CMD_GET_CONFIG 0XC2
#define CMD_SET_NAME 0XC3
#define CMD_GET_NAME 0XC4
#define CMD_CLR_DEVICE 0XC5
#define CMD_NEW_ADV 0XC7
#define CMD_RF_STS_SYSC 0XC9
#define CMD_SET_24G_NAME 0XCA
#define CMD_GO_TEST 0XCF
#define CMD_RF_DFU 0XB1
#define CMD_NULL 0X00 // I made this up, don't know if it exists. For tracking RX State

#define CMD_WRITE_DATA 0X80
#define CMD_READ_DATA 0X81

#define LINK_RF_24 0
#define LINK_BT_1 1
#define LINK_BT_2 2
#define LINK_BT_3 3
#define LINK_USB 4

#define UART_HEAD 0x5A
#define FUNC_VALID_LEN 32
#define UART_MAX_LEN 64

#define SYS_SW_WIN 0xa1
#define SYS_SW_MAC 0xa2

#define RF_LINK_SHOW_TIME 300

#define HOST_USB_TYPE 0
#define HOST_BLE_TYPE 1
#define HOST_RF_TYPE 2

#define LINK_TIMEOUT (100 * 120)
#define LINK_TIMEOUT_ALT (100 * 5)
#define SLEEP_TIME_DELAY (100 * 360)
#define TIMER_STEP 10
#define POWER_DOWN_DELAY (24)

// TODO: Do we need this
#define RF_POWER_DOWN_DELAY       (dev_info.link_mode == LINK_RF_24 ? 5 : 30)

#define MICRO_PRESS_DELAY         5
#define SMALL_PRESS_DELAY         10
#define MEDIUM_PRESS_DELAY        30
#define LONG_PRESS_DELAY          50

#define RF_LONG_PRESS_DELAY 30
#define DEV_RESET_PRESS_DELAY 30
#define RGB_TEST_PRESS_DELAY 30

#define T_MIN                     (100 * 60)

// TODO: Do we need these
#define RGB_MATRIX_GAME_MODE      RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define SIDE_MATRIX_GAME_MODE     4
#define SOCD_KEYS                 KC_A, KC_D, KC_LEFT, KC_RIGHT, KC_UP, KC_DOWN

#define SLEEP_MODE_OFF 0
#define SLEEP_MODE_LIGHT 1
#define SLEEP_MODE_DEEP 2

#define NOP_WAIT                  __asm__ __volatile__("nop;nop;nop;nop;nop;nop;nop;nop;\n\t" ::: "memory")  // sleep 0.125 us (125 ns)
#define sizeof_array(x)           (sizeof(x)/sizeof(*(x)))
#define USB_ACTIVE                ((dev_info.link_mode == LINK_USB && USB_DRIVER.state != USB_SUSPENDED) || (dev_info.link_mode != LINK_USB && dev_info.rf_charge == 0x03))

// clang-format on

typedef struct {
    uint8_t  RXDState;
    uint8_t  RXDLen;
    uint8_t  RXDOverTime;
    uint8_t  TXDLenBack;
    uint8_t  TXDOffset;
    uint8_t  RXCmd;
    uint32_t TXLastCmdTm;
    uint8_t  TXDBuf[UART_MAX_LEN];
    uint8_t  RXDBuf[UART_MAX_LEN];
} USART_MGR_STRUCT;

typedef struct {
    uint8_t link_mode;
    uint8_t rf_channel;
    uint8_t ble_channel;
    uint8_t rf_state;
    uint8_t rf_charge;
    uint8_t rf_led;
    uint8_t rf_battery;
    uint8_t sys_sw_state;
} DEV_INFO_STRUCT;

typedef struct {
    // JinCao renamed this to init_flag, but still only used for default_brightness
    // Keeping original name for now.
    uint8_t  default_brightness_flag;
    // (top) side light config
    uint8_t  side_mode;
    uint8_t  side_light;
    uint8_t  side_speed;
    uint8_t  side_rgb;
    uint8_t  side_colour;
    uint8_t  sleep_mode : 2;
    uint16_t rf_link_timeout;
    // ryodeushii additions
    uint8_t usb_sleep_toggle : 1;
    uint8_t debounce_press_ms;
    uint8_t debounce_release_ms;
    uint8_t sleep_timeout;
    uint8_t caps_indicator_type;
#ifdef SIDE_SEPARATE
    // right side light config
    uint8_t right_side_mode;
    uint8_t right_side_light;
    uint8_t right_side_speed;
    uint8_t right_side_rgb;
    uint8_t right_side_colour;
#endif
    uint8_t battery_indicator_brightness;
    // custom keys highlight
    uint8_t toggle_custom_keys_highlight : 1;
    uint8_t detect_numlock_state : 1;
    uint8_t battery_indicator_numeric : 1;
    uint8_t show_socd_indicator : 1;
    uint8_t caps_word_enable : 1;
    // TODO: Do we need these
    uint8_t numlock_state;
    uint8_t debounce_type;
    /*
    uint8_t game_rgb_mod;
    uint8_t game_rgb_val;
    uint8_t game_rgb_hue;
    uint8_t game_rgb_sat;
    */
    uint8_t socd_mode;
    uint8_t rf_delay_step;
    uint8_t retain1;
    uint8_t retain2;
} kb_config_t;

#ifdef VIA_ENABLE
_Static_assert(sizeof(kb_config_t) <= VIA_EEPROM_CUSTOM_CONFIG_SIZE, "Mismatch in kb EECONFIG stored data");
#else
_Static_assert(sizeof(kb_config_t) <= EECONFIG_KB_DATA_SIZE, "Mismatch in kb EECONFIG stored data");
#endif

typedef struct
{
    uint8_t KC_CAPS;
    uint8_t KC_LGUI;
    uint8_t KC_NUM;
    // TODO: Despite the name, this is actually for DEBOUNCE_T
    uint8_t KC_D;
    // TODO: Despite the name, this is actually for GAME_MODE
    uint8_t KC_G;
    uint8_t KC_F1;
    uint8_t RF_DFU;
    uint8_t KC_F12;
    uint8_t KC_GRV;
    uint8_t SOCD_TOG;
} led_index_t;

// Globals
extern led_index_t        led_idx;
extern DEV_INFO_STRUCT    dev_info;
extern kb_config_t        kb_config;
extern uint8_t            rf_blink_cnt;
extern uint16_t           rf_link_show_time;
extern uint16_t           side_led_last_act;
extern bool               f_bat_hold;
extern bool               f_sys_show;
extern bool               f_sleep_show;
extern rgb_t              bat_pct_rgb;
extern bool               f_rf_sw_press;
extern uint8_t            f_rf_dfu_press;
extern bool               f_dev_reset_press;
extern bool               f_bat_num_show;
extern bool               f_rgb_test_press;
extern uint8_t            f_caps_word_tg;
extern bool               f_rgb_led_press;
extern uint32_t           no_act_time;
extern uint16_t           link_timeout;
extern uint8_t            rf_sw_temp;
extern uint16_t           rf_sw_press_delay;
extern uint16_t           rf_linking_time;
extern uint16_t           sleep_time_delay;
extern bool               f_wakeup_prepare;
extern bool               f_rf_new_adv_ok;
extern bool               f_dial_sw_init_ok;

extern uint8_t            f_numlock_press;
extern uint8_t            f_gmode_reset_press;

extern uint32_t           eeprom_update_timer;
extern bool               rgb_update;
extern bool               user_update;
extern bool               flush_side_leds;
extern uint8_t            rgb_required;
extern uint8_t            low_bat_level;
extern bool               rgb_power_save;
extern uint32_t           dequeue_delay;
extern bool               game_mode_enable;

extern void               user_config_override(void);

void    dev_sts_sync(void);
void    rf_uart_init(void);
void    rf_device_init(void);
void    uart_send_report_repeat(void);
void    uart_receive_pro(void);
void    uart_send_report(uint8_t report_type, uint8_t *report_buf, uint8_t report_size);
void    side_speed_control(uint8_t dir);
void    side_light_control(uint8_t dir);
void    side_colour_control(uint8_t dir);
void    side_mode_control(uint8_t dir);
void    led_show(void);
void    side_led_show(void);
void    sleep_handle(void);
void    num_led_show(void);
void    bat_pct_led_kb(void);
void    update_bat_pct_rgb(uint8_t bat_percent);
void    rgb_test_show(void);
void    gpio_init(void);
void    long_press_key(void);
void    break_all_key(void);
void    switch_dev_link(uint8_t mode);
void    dial_sw_scan(void);
void    dial_sw_fast_scan(void);
void    timer_pro(void);
void    load_eeprom_data(void);
void    delay_update_eeprom_data(void);
void    kb_config_reset(void);
void    kb_config_init(void);
void    user_set_rgb_color(int index, uint8_t red, uint8_t green, uint8_t blue);
uint8_t get_led_index(uint8_t row, uint8_t col);

void    sleep_handle(void);
void    led_power_handle(void);
void    toggle_sleep_mode(void);
void    toggle_caps_indication(void);
void    toggle_usb_sleep(void);
void    link_mode_set(void);

uint8_t uart_send_cmd(uint8_t cmd, uint8_t ack_cnt, uint8_t delayms);

uint8_t  two_digit_decimals_led(uint8_t value);
uint8_t  two_digit_ones_led(uint8_t value);
void     adjust_debounce(uint8_t dir, DEBOUNCE_EVENT debounce_event);
uint32_t get_sleep_timeout(void);
void     adjust_sleep_timeout(uint8_t dir);

void    matrix_io_delay(void);
void    game_mode_tweak(void);
void    user_debug(void);
void    call_update_eeprom_data(bool* eeprom_update_init);
void    signal_rgb_led(uint8_t selected_color, uint8_t start_led, uint8_t end_led, uint16_t show_time);
void    reset_led_idx(void);
void    debounce_type(void);
void    game_config_reset(uint8_t save_to_eeprom);
void    rgb_matrix_step_game_mode(uint8_t step);
void    power_save(void);
void    clear_rgb(void);
//uint8_t step_helper(uint8_t dir, uint8_t value);
uint8_t get_led_idx(uint16_t keycode);

#ifdef SIDE_SEPARATE
void    right_side_speed_control(uint8_t dir);
void    right_side_light_control(uint8_t dir);
void    right_side_colour_control(uint8_t dir);
void    right_side_mode_control(uint8_t dir);
void    right_side_led_loop(void);
#endif


#ifdef VIA_ENABLE
enum via_indicator_value {
    id_usb_sleep_toggle    = 0,
    id_debounce_press      = 1,
    id_debounce_release    = 2,
    id_sleep_timeout       = 3,
    id_caps_indicator_type = 4,
    id_sleep_mode          = 5,
    // side light controls
    id_side_light_mode       = 10,
    id_side_light_speed      = 11,
    id_side_light_color      = 12,
    id_side_light_brightness = 13,
    // right side light controls
    id_right_side_light_mode        = 20,
    id_right_side_light_speed       = 21,
    id_right_side_light_color       = 22,
    id_right_side_light_brightness  = 23,
    id_battery_indicator_brightness = 31,
    id_toggle_custom_keys_highlight = 32,
    id_toggle_detect_numlock_state  = 33,
    id_battery_indicator_numeric    = 34,
    id_toggle_socd_indicator        = 35,
};

// function declaration
void indicator_config_set_value(uint8_t *data);
void indicator_config_get_value(uint8_t *data);
void _set_color(HSV *color, uint8_t *data);
void _get_color(HSV *color, uint8_t *data);
#endif

void save_config_to_eeprom(void);
void load_config_from_eeprom(void);
void save_config_to_eeprom_now(void);
void save_rgb_config(void);

void debug_show_led(uint8_t show);
