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

#include "action.h"
#include "color.h"
#include "common/features/socd_cleaner.h"
#include "config.h"
#include "host.h"
#include "keycodes.h"
#include "rgb_matrix.h"
#include "kb_util.h"
#include "ansi.h"
#include "usb_main.h"
#include "mcu_pwr.h"

#ifdef VIA_ENABLE
#    include "eeprom.h"
#    include "via.h"
#else
#    include "eeconfig.h"
#endif
extern bool            f_rf_sw_press;
extern bool            f_sleep_show;
extern bool            f_dev_reset_press;
extern bool            f_bat_num_show;
extern bool            f_rgb_test_press;
extern bool            f_bat_hold;
extern uint32_t        no_act_time;
extern uint8_t         rf_sw_temp;
extern bool            f_debounce_press_show;
extern bool            f_debounce_release_show;
extern bool            f_sleep_timeout_show;
extern uint16_t        rf_sw_press_delay;
extern uint16_t        rf_linking_time;

extern void exit_light_sleep(void);

extern kb_config_t     kb_config;

bool pre_process_record_kb(uint16_t keycode, keyrecord_t *record) {
    no_act_time     = 0;
    rf_linking_time = 0;

    // wakeup check for light sleep/no sleep - fire this immediately to not lose wake keys.
    if (f_wakeup_prepare) {
        f_wakeup_prepare = 0;
        if (kb_config.sleep_mode) exit_light_sleep();
    }

    return pre_process_record_user(keycode, record);
}

socd_cleaner_t socd_v = {{KC_W, KC_S}, SOCD_CLEANER_LAST};
socd_cleaner_t socd_h = {{KC_A, KC_D}, SOCD_CLEANER_LAST};

/* qmk process record */
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }
    // socd handling
    if (!process_socd_cleaner(keycode, record, &socd_v)) {
        return false;
    }
    if (!process_socd_cleaner(keycode, record, &socd_h)) {
        return false;
    }

    switch (keycode) {
#if (WORK_MODE == THREE_MODE)
        case RF_DFU:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    return false;
                }
                uart_send_cmd(CMD_RF_DFU, 10, 20);
            }
            return false;
#endif
        case LNK_USB:
            if (record->event.pressed) {
                break_all_key();
            } else {
                dev_info.link_mode = LINK_USB;
                uart_send_cmd(CMD_SET_LINK, 10, 10);
            }
            return false;

#if (WORK_MODE == THREE_MODE)
        case LNK_RF:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    rf_sw_temp    = LINK_RF_24;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else if (f_rf_sw_press) {
                f_rf_sw_press = 0;
                if (rf_sw_press_delay < RF_LONG_PRESS_DELAY) {
                    link_mode_set();
                }
            }
            return false;

        case LNK_BLE1:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    rf_sw_temp    = LINK_BT_1;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else if (f_rf_sw_press) {
                f_rf_sw_press = 0;
                if (rf_sw_press_delay < RF_LONG_PRESS_DELAY) {
                    link_mode_set();
                }
            }
            return false;

        case LNK_BLE2:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    rf_sw_temp    = LINK_BT_2;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else if (f_rf_sw_press) {
                f_rf_sw_press = 0;
                if (rf_sw_press_delay < RF_LONG_PRESS_DELAY) {
                    link_mode_set();
                }
            }
            return false;

        case LNK_BLE3:
            if (record->event.pressed) {
                if (dev_info.link_mode != LINK_USB) {
                    rf_sw_temp    = LINK_BT_3;
                    f_rf_sw_press = 1;
                    break_all_key();
                }
            } else if (f_rf_sw_press) {
                f_rf_sw_press = 0;
                if (rf_sw_press_delay < RF_LONG_PRESS_DELAY) {
                    link_mode_set();
                }
            }
            return false;
#endif
        case MAC_TASK:
            if (record->event.pressed) {
                host_consumer_send(0x029F);
            } else {
                host_consumer_send(0);
            }
            return false;

        case MAC_SEARCH:
            if (record->event.pressed) {
                register_code(KC_LGUI);
                register_code(KC_SPACE);
                wait_ms(50);
                unregister_code(KC_LGUI);
                unregister_code(KC_SPACE);
            }
            return false;

        case MAC_VOICE:
            if (record->event.pressed) {
                host_consumer_send(0xcf);
            } else {
                host_consumer_send(0);
            }
            return false;

        case MAC_CONSOLE:
            if (record->event.pressed) {
                host_consumer_send(0x02A0);
            } else {
                host_consumer_send(0);
            }
            return false;

        case MAC_DND:
            if (record->event.pressed) {
                host_system_send(0x9b);
            } else {
                host_system_send(0);
            }
            return false;

        case MAC_GLOBE:
            if (record->event.pressed) {
                host_consumer_send(0x029D);
            } else {
                host_consumer_send(0);
            }
            return false;

        case MAC_PRT:
            if (record->event.pressed) {
                register_code(KC_LGUI);
                register_code(KC_LSFT);
                register_code(KC_3);
                wait_ms(50);
                unregister_code(KC_3);
                unregister_code(KC_LSFT);
                unregister_code(KC_LGUI);
            }
            return false;

        case MAC_PRTA:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_WIN) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_S);
                    wait_ms(50);
                    unregister_code(KC_S);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                }

                else {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_4);
                    wait_ms(50);
                    unregister_code(KC_4);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                }
            }
            return false;

        case SIDE_VAI:
            if (record->event.pressed) {
                side_light_control(1);
            }
            return false;

        case SIDE_VAD:
            if (record->event.pressed) {
                side_light_control(0);
            }
            return false;

        case SIDE_MOD:
            if (record->event.pressed) {
                side_mode_control(1);
            }
            return false;

        case SIDE_HUI:
            if (record->event.pressed) {
                side_colour_control(1);
            }
            return false;

        case SIDE_SPI:
            if (record->event.pressed) {
                side_speed_control(1);
            }
            return false;

        case SIDE_SPD:
            if (record->event.pressed) {
                side_speed_control(0);
            }
            return false;

#ifdef SIDE_SEPARATE
        case RIGHT_SIDE_VAI:
            if (record->event.pressed) {
                right_side_light_control(1);
            }
            return false;
        case RIGHT_SIDE_VAD:
            if (record->event.pressed) {
                right_side_light_control(0);
            }
            return false;
        case RIGHT_SIDE_MOD:
            if (record->event.pressed) {
                right_side_mode_control(1);
            }
            return false;
        case RIGHT_SIDE_HUI:
            if (record->event.pressed) {
                right_side_colour_control(1);
            }
            return false;
        case RIGHT_SIDE_SPI:
            if (record->event.pressed) {
                right_side_speed_control(1);
            }
            return false;
        case RIGHT_SIDE_SPD:
            if (record->event.pressed) {
                right_side_speed_control(0);
            }
            return false;
#endif

        case DEV_RESET:
            if (record->event.pressed) {
                f_dev_reset_press = 1;
                break_all_key();
            } else {
                f_dev_reset_press = 0;
            }
            return false;

        case SLEEP_MODE:
            if (record->event.pressed) {
                toggle_sleep_mode();
            }
            return false;

        case BAT_SHOW:
            if (record->event.pressed) {
                f_bat_hold = !f_bat_hold;
            }
            return false;

        case WIN_LOCK:
            if (record->event.pressed) {
                keymap_config.no_gui = !keymap_config.no_gui;
                eeconfig_update_keymap(&keymap_config);
                break_all_key();
            } else
                unregister_code16(keycode);
            break;

        case BAT_NUM:
            f_bat_num_show = record->event.pressed;
            return false;

        case RGB_TEST:
            f_rgb_test_press = record->event.pressed;
            return false;

        case LINK_TO:
            if (record->event.pressed) {
                uint16_t mask = LINK_TIMEOUT ^ LINK_TIMEOUT_ALT;
                kb_config.rf_link_timeout ^= mask; // XOR swap
                eeconfig_update_kb_datablock(&kb_config, 0, EECONFIG_KB_DATA_SIZE);
            }
            return false;

        case RM_VALU: // ensure LED powers on with brightness increase
            if (record->event.pressed) {
                pwr_rgb_led_on();
            }
            return true;

        case KB_SLP:
            if (record->event.pressed) {
                uint16_t mask = (100 * 30) ^ SLEEP_TIME_DELAY; // 30s or default
                sleep_time_delay ^= mask;                      // XOR swap
            }
            return false;

        case SHIFT_GRV:
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_GRV);
            } else {
                unregister_code(KC_LSFT);
                unregister_code(KC_GRV);
            }
            return false;

        case TOG_USB_SLP:
            if (record->event.pressed) {
                toggle_usb_sleep();
            }
            return false;
        case TOG_CAPS_IND:
            if (record->event.pressed) {
                toggle_caps_indication();
            }

            return false;

        case DEBOUNCE_PRESS_SHOW:
            if (record->event.pressed) {
                f_debounce_press_show = !f_debounce_press_show;
            }
            return false;

        case DEBOUNCE_PRESS_INC:
            if (record->event.pressed) {
                adjust_debounce(1, false);
            }
            return false;

        case DEBOUNCE_PRESS_DEC:
            if (record->event.pressed) {
                adjust_debounce(0, false);
            }
            return false;
        case DEBOUNCE_RELEASE_SHOW:
            if (record->event.pressed) {
                f_debounce_release_show = !f_debounce_release_show;
            }
            return false;

        case DEBOUNCE_RELEASE_INC:
            if (record->event.pressed) {
                adjust_debounce(1, true);
            }
            return false;

        case DEBOUNCE_RELEASE_DEC:
            if (record->event.pressed) {
                adjust_debounce(0, true);
            }
            return false;

        case SLEEP_TIMEOUT_SHOW:
            if (record->event.pressed) {
                f_sleep_timeout_show = !f_sleep_timeout_show;
            }
            return false;

        case SLEEP_TIMEOUT_INC:
            if (record->event.pressed) {
                adjust_sleep_timeout(1);
            }
            return false;

        case SLEEP_TIMEOUT_DEC:
            if (record->event.pressed) {
                adjust_sleep_timeout(0);
            }
            return false;
        case TOG_BAT_IND_NUM:
            if (record->event.pressed) {
                kb_config.battery_indicator_numeric = !kb_config.battery_indicator_numeric;
                save_config_to_eeprom();
            }
            return false;
        case SOCDON: // Turn SOCD Cleaner on.
            if (record->event.pressed) {
                socd_cleaner_enabled = true;
            }
            return false;
        case SOCDOFF: // Turn SOCD Cleaner off.
            if (record->event.pressed) {
                socd_cleaner_enabled = false;
            }
            return false;
        case SOCDTOG: // Toggle SOCD Cleaner.
            if (record->event.pressed) {
                socd_cleaner_enabled = !socd_cleaner_enabled;
            }
            return false;
        case FW_VERSION:
            if (record->event.pressed) {
                SEND_STRING(CFW_VERSION);
            }
            return false;

        default:
            return true;
    }
    return true;
}

// self note - this won't get called if RGB matrix is suspended or if there's no effect
bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) {
        return false;
    }

    // JinCao version: (f_bat_num_show || f_bat_hold)
    // Effect of f_bat_num_show moved to advanced, as per ryodeushii

    if (debug_enable) {
        user_set_rgb_color(56, 0x80, 0x00, 0x00);
    }

    // JinCao customization
    // light up corresponding BT mode key during connection
    /*if (rf_blink_cnt && dev_info.link_mode >= LINK_BT_1 && dev_info.link_mode <= LINK_BT_3) {
        user_set_rgb_color(30 - dev_info.link_mode, 0, 0, 0x80);
    }*/
    
    if (rf_blink_cnt) {
        uint8_t col = 4;
        if (dev_info.link_mode >= LINK_BT_1 && dev_info.link_mode <= LINK_BT_3) {
            col = dev_info.link_mode;
        } else if (dev_info.link_mode == LINK_RF_24) {
            col = 4;
        }
        user_set_rgb_color(get_led_index(1, col), 0, 0, 0x80);
    }
    
    // Original
    // fix power-on brightness is abnormal
    //user_set_rgb_color(RGB_MATRIX_LED_COUNT - 1, 0, 0, 0);

    // JinCao version
    // power down unused LEDs
    led_power_handle();

    return true;
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (keymap_config.no_gui) {
        // fixed position in top right corner, key position in matrix is (0,16), led index is (16)
        user_set_rgb_color(get_led_index(0, 15), 0x00, 0x80, 0x00);
    }

    if (f_debounce_press_show) { // green numbers - press debounce
        user_set_rgb_color(two_digit_decimals_led(kb_config.debounce_press_ms), 0x00, 0x80, 0x00);
        user_set_rgb_color(two_digit_ones_led(kb_config.debounce_press_ms), 0x00, 0x80, 0x00);
    }
    if (f_debounce_release_show) { // red numbers - release deboucne
        user_set_rgb_color(two_digit_decimals_led(kb_config.debounce_release_ms), 0x80, 0x00, 0x00);
        user_set_rgb_color(two_digit_ones_led(kb_config.debounce_release_ms), 0x80, 0x00, 0x00);
    }

    if (f_sleep_timeout_show) { // cyan numbers - sleep timeout
        user_set_rgb_color(two_digit_decimals_led(kb_config.sleep_timeout), 0x00, 0x80, 0x80);
        user_set_rgb_color(two_digit_ones_led(kb_config.sleep_timeout), 0x00, 0x80, 0x80);
    }

    if (kb_config.show_socd_indicator && socd_cleaner_enabled) {
        user_set_rgb_color(get_led_index(2, 2), RGB_BLUE);
        user_set_rgb_color(get_led_index(3, 2), RGB_BLUE);
        user_set_rgb_color(get_led_index(3, 1), RGB_BLUE);
        user_set_rgb_color(get_led_index(3, 3), RGB_BLUE);
    }

    if (kb_config.detect_numlock_state) {
        uint8_t showNumLock = 0;
        if (dev_info.link_mode != LINK_USB) {
            showNumLock = dev_info.rf_led & 0x01;
        } else {
            showNumLock = host_keyboard_led_state().num_lock;
        }

        if (showNumLock) {
            user_set_rgb_color(get_led_index(0, 14), 0x00, 0x80, 0x00);
        }
    }

    user_set_rgb_color(RGB_MATRIX_LED_COUNT - 1, 0, 0, 0);

    if (kb_config.toggle_custom_keys_highlight) {
        uint8_t layer = get_highest_layer(layer_state);
        switch (layer) {
            case 0:
            case 2:
                break;
            default: {
                for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
                    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                        uint8_t index = g_led_config.matrix_co[row][col];

                        if (index >= led_min && index <= led_max && index != NO_LED) {
                            int keycode = keymap_key_to_keycode(layer, (keypos_t){col, row});

                            if (keycode >= RIGHT_SIDE_VAI && keycode <= RIGHT_SIDE_SPD) {
                                user_set_rgb_color(index, RGB_WHITE);
                            } else if (keycode >= SIDE_VAI && keycode <= SIDE_SPD) {
                                user_set_rgb_color(index, RGB_YELLOW);
                            } else if (keycode >= DEBOUNCE_PRESS_INC && keycode <= DEBOUNCE_PRESS_SHOW) {
                                user_set_rgb_color(index, 0, 255, 0);
                            } else if (keycode >= DEBOUNCE_RELEASE_INC && keycode <= DEBOUNCE_RELEASE_SHOW) {
                                user_set_rgb_color(index, 255, 0, 0);
                            } else if (keycode == SLEEP_MODE || keycode == TOG_USB_SLP || (keycode >= SLEEP_TIMEOUT_INC && keycode <= SLEEP_TIMEOUT_SHOW)) {
                                user_set_rgb_color(index, RGB_CYAN);
                            } else if (keycode >= LNK_USB && keycode <= LNK_BLE3) {
                                if (dev_info.link_mode != LINK_USB) {
                                    user_set_rgb_color(index, RGB_BLUE);
                                }
                            } else if (keycode > KC_NUM_LOCK && keycode <= KC_KP_DOT) {
                                user_set_rgb_color(index, RGB_RED);
                            } else if (keycode >= SOCDON && keycode <= SOCDTOG) {
                                user_set_rgb_color(index, RGB_BLUE);
                            } else if (keycode > KC_TRNS) {
                                user_set_rgb_color(index, 225, 65, 140);
                            }
                        }
                    }
                }
            }
        }
    }

    // ryodeushii version - basically the same logic, except a constant color
    /*if (f_bat_hold && kb_config.battery_indicator_numeric) {
        user_set_rgb_color(two_digit_decimals_led(dev_info.rf_battery), 0x00, 0x80, 0x80);
        user_set_rgb_color(two_digit_ones_led(dev_info.rf_battery), 0x00, 0x80, 0x80);
    }*/

    if (f_bat_num_show) {
        if (kb_config.battery_indicator_numeric) {
            // In the JinCao version, the lit-up F-key and number form a percentage.
            bat_pct_led_kb();
        } else {
            // In the Original version, the number keys are lit up like a battery bar.
            // This is nice, but AFAIK sort of redundant with the side bar.
            // ryodeushii version does not have the equivalent of this else branch
            num_led_show();
        }
    }

    return rgb_matrix_indicators_advanced_user(led_min, led_max);
}

/* qmk keyboard post init */
void keyboard_post_init_kb(void) {
    gpio_init();

#if (WORK_MODE == THREE_MODE)
    rf_uart_init();
    wait_ms(500);
    rf_device_init();
#endif
    break_all_key();
    dial_sw_fast_scan();
    load_eeprom_data();
    keyboard_post_init_user();
}

/* qmk housekeeping task */
void housekeeping_task_kb(void) {
    timer_pro();

#if (WORK_MODE == THREE_MODE)
    uart_receive_pro();

    uart_send_report_repeat();

    dev_sts_sync();
#endif

    long_press_key();

    dial_sw_scan();

    side_led_show();

    sleep_handle();
}

void kb_config_init(void) {
    kb_config.default_brightness_flag      = 0xA5;
    kb_config.sleep_mode                   = DEFAULT_SLEEP_MODE;
    kb_config.usb_sleep_toggle             = DEFAULT_USB_SLEEP_TOGGLE;
    kb_config.sleep_timeout                = DEFAULT_SLEEP_TIMEOUT;
    kb_config.debounce_press_ms            = DEBOUNCE;
    kb_config.debounce_release_ms          = RELEASE_DEBOUNCE;
    kb_config.caps_indicator_type          = DEFAULT_CAPS_INDICATOR_TYPE;
    kb_config.battery_indicator_brightness = DEFAULT_BATTERY_INDICATOR_BRIGHTNESS;
    kb_config.toggle_custom_keys_highlight = DEFAULT_LIGHT_CUSTOM_KEYS;
    kb_config.side_mode                    = DEFAULT_SIDE_MODE;
    kb_config.side_light                   = DEFAULT_SIDE_LIGHT;
    kb_config.side_speed                   = DEFAULT_SIDE_SPEED;
    kb_config.side_rgb                     = DEFAULT_SIDE_RGB;
    kb_config.side_colour                  = DEFAULT_SIDE_COLOR;
#ifdef SIDE_SEPARATE
    kb_config.right_side_mode              = DEFAULT_RIGHT_SIDE_MODE;
    kb_config.right_side_light             = DEFAULT_RIGHT_SIDE_LIGHT;
    kb_config.right_side_speed             = DEFAULT_RIGHT_SIDE_SPEED;
    kb_config.right_side_rgb               = DEFAULT_RIGHT_SIDE_RGB;
    kb_config.right_side_colour            = DEFAULT_RIGHT_SIDE_COLOR;
#endif
    kb_config.detect_numlock_state         = DEFAULT_DETECT_NUMLOCK;
    kb_config.battery_indicator_numeric    = DEFAULT_BATTERY_INDICATOR_NUMERIC;
    kb_config.show_socd_indicator          = DEFAULT_SHOW_SOCD_INDICATOR;
    kb_config.rf_link_timeout              = LINK_TIMEOUT_ALT;
}

uint32_t read_custom_config(void *data, uint32_t offset, uint32_t length) {
#ifdef VIA_ENABLE
    return via_read_custom_config(data, offset, length);
#else
    return eeconfig_read_kb_datablock(data, offset, length);
#endif
}

uint32_t write_custom_config(const void *data, uint32_t offset, uint32_t length) {
#ifdef VIA_ENABLE
    return via_update_custom_config(data, offset, length);
#else
    return eeconfig_update_kb_datablock(data, offset, length);
#endif
}

void load_config_from_eeprom(void) {
    read_custom_config(&kb_config, 0, sizeof(kb_config));
}

void save_config_to_eeprom(void) {
    write_custom_config(&kb_config, 0, sizeof(kb_config));
}

#ifdef VIA_ENABLE
void via_init_kb(void) {
    init_kb_config();
    // If the EEPROM has the magic, the data is good.
    // OK to load from EEPROM
    if (eeconfig_is_enabled()) {
        load_config_from_eeprom();
    } else {
        save_config_to_eeprom();
        // DO NOT set EEPROM valid here, let caller do this
    }
}

void via_config_set_value(uint8_t *data)

{
    // data = [ value_id, value_data ]

    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_usb_sleep_toggle:
            kb_config.usb_sleep_toggle = *value_data;
            break;
        case id_debounce_press:
            kb_config.debounce_press_ms = *value_data;
            break;
        case id_debounce_release:
            kb_config.debounce_release_ms = *value_data;
            break;
        case id_sleep_timeout:
            kb_config.sleep_timeout = *value_data + 1;
            break;
        case id_caps_indicator_type:
            kb_config.caps_indicator_type = *value_data;
            break;
        case id_sleep_mode:
            kb_config.sleep_mode = *value_data;
            break;

        case id_side_light_mode:
            kb_config.side_mode = *value_data;
            break;
        case id_side_light_speed:
            kb_config.side_speed = *value_data;
            break;
        case id_side_light_color:
            kb_config.side_color = *value_data;
            break;
        case id_side_light_brightness:
            kb_config.side_light = *value_data;
            break;

#ifdef SIDE_SEPARATE
        case id_right_side_light_mode:
            kb_config.right_side_mode = *value_data;
            break;
        case id_right_side_light_speed:
            kb_config.right_side_speed = *value_data;
            break;
        case id_right_side_light_color:
            kb_config.right_side_color = *value_data;
            break;
        case id_right_side_light_brightness:
            kb_config.right_side_light = *value_data;
            break;
#endif
        case id_battery_indicator_brightness:
            kb_config.battery_indicator_brightness = *value_data;
            break;
        case id_toggle_custom_keys_highlight:
            kb_config.toggle_custom_keys_highlight = *value_data;
            break;
        case id_toggle_detect_numlock_state:
            kb_config.detect_numlock_state = *value_data;
            break;
        case id_battery_indicator_numeric:
            kb_config.battery_indicator_numeric = *value_data;
            break;
        case id_toggle_socd_indicator:
            kb_config.show_socd_indicator = *value_data;
            break;
    }
}

void via_config_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_usb_sleep_toggle:
            *value_data = kb_config.usb_sleep_toggle;
            break;
        case id_debounce_press:
            *value_data = kb_config.debounce_press_ms;
            break;
        case id_debounce_release:
            *value_data = kb_config.debounce_release_ms;
            break;
        case id_sleep_timeout:
            *value_data = kb_config.sleep_timeout - 1;
            break;
        case id_caps_indicator_type:
            *value_data = kb_config.caps_indicator_type;
            break;
        case id_sleep_mode:
            *value_data = kb_config.sleep_mode;
            break;

        case id_side_light_mode:
            *value_data = kb_config.side_mode;
            break;
        case id_side_light_speed:
            *value_data = kb_config.side_speed;
            break;
        case id_side_light_color:
            *value_data = kb_config.side_color;
            break;
        case id_side_light_brightness:
            *value_data = kb_config.side_light;
            break;

#ifdef SIDE_SEPARATE
        case id_right_side_light_mode:
            *value_data = kb_config.right_side_mode;
            break;
        case id_right_side_light_speed:
            *value_data = kb_config.right_side_speed;
            break;
        case id_right_side_light_color:
            *value_data = kb_config.right_side_color;
            break;
        case id_right_side_light_brightness:
            *value_data = kb_config.right_side_light;
            break;
#endif

        case id_battery_indicator_brightness:
            *value_data = kb_config.battery_indicator_brightness;
            break;
        case id_toggle_custom_keys_highlight:
            *value_data = kb_config.toggle_custom_keys_highlight;
            break;
        case id_toggle_detect_numlock_state:
            *value_data = kb_config.detect_numlock_state;
            break;

        case id_battery_indicator_numeric:
            *value_data = kb_config.battery_indicator_numeric;
            break;
        case id_toggle_socd_indicator:
            *value_data = kb_config.show_socd_indicator;
            break;
    }
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);

    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id)

        {
            case id_custom_set_value: {
                via_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                save_config_to_eeprom();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    // Return the unhandled state
    *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}
#endif
