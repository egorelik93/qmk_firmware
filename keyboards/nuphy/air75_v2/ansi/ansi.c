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
#include "redefine.h"
#include "version.h"

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

// TODO: adi, Not currently using this.
// char            socd_type[4][14] = { "disabled", "cancellation", "exclusion", "nullification" };

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

// TODO: I think one of the strategies doesn't have an equivalent in socd_cleaner.
/*bool process_record_socd(uint16_t keycode, keyrecord_t *record) {
    if (kb_config.socd_mode == 0) { return true; }
    uint8_t socd_array[] = { SOCD_KEYS };
    for (uint8_t idx = 0; idx < sizeof_array(socd_array); ++idx) {
        if ( keycode != socd_array[idx] ) { continue; }

        if (idx % 2 == 0) {
            left_pressed = record->event.pressed;
            idx++;
        } else {
            right_pressed = record->event.pressed;
            idx--;
        }

        if (record->event.pressed) {
            if (right_pressed + left_pressed > 2) {
                unregister_code(socd_array[idx]);
                if (kb_config.socd_mode == 3) { return false; }
            }
        } else {
            if (right_pressed + left_pressed > 2) {
                if (kb_config.socd_mode >= 2) { register_code(socd_array[idx]); }
            }
        }
        return true;
    }
    return true;
}*/

// TODO
/* check if we should early return */
bool process_record_early(uint16_t keycode, keyrecord_t *record) {

    switch (keycode) {
        case SIDE_MOD:
        case SIDE_SPI:
        case SIDE_SPD:
#ifdef SIDE_SEPARATE
        case RIGHT_SIDE_MOD:
        case RIGHT_SIDE_SPI:
        case RIGHT_SIDE_SPD:
#endif
        case SLEEP_MODE:
        case SLEEP_TIMEOUT_INC:
        case SLEEP_TIMEOUT_DEC:
        case SLEEP_TIMEOUT_SHOW:
        case CAPS_WORD:
            if (game_mode_enable) { return false; }
            return true;

        case BAT_SHOW:
        case KB_SLP:
        //case SLEEP_NOW:
            if (game_mode_enable) { return false; }
            return true;

        case QK_RGB_MATRIX_SPEED_UP:
        case QK_RGB_MATRIX_SPEED_DOWN:
            if (game_mode_enable) { return false; }
            return true;

        default:
            return true;
    }
}

/* qmk process record */
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }

    if (!process_record_early(keycode, record)) {
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
            if (game_mode_enable) { return false; }
            if (record->event.pressed) {
                f_rf_dfu_press = 1;
            } else {
                if (f_rf_dfu_press) {
                    f_rf_dfu_press = 0;
                    kb_config.rf_delay_step = (kb_config.rf_delay_step + 1) % 5;
                    save_config_to_eeprom();
#ifndef NO_DEBUG
                    dprintf("rf_delay: %d\n", kb_config.rf_delay_step * 200 + 80);
#endif
                    signal_rgb_led(kb_config.rf_delay_step * 2, led_idx.RF_DFU, UINT8_MAX, 3000);
                }
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
                rgb_matrix_update_pwm_buffers();
            }
            return false;
#endif
        case MAC_TASK:
            if (record->event.pressed) {
                /*if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    tap_code(KC_MCTL);
                } else {
                    tap_code(KC_CALC);
                }*/
                host_consumer_send(0x029F);
            } else {
                host_consumer_send(0);
            }
            return false;

        case MAC_SEARCH:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_SPACE);
                    // wait_ms(50);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_LGUI);
                    unregister_code(KC_SPACE);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_F);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_F);
                    unregister_code(KC_LCTL);
                }
            }
            return false;

        case MAC_VOICE:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    host_consumer_send(0xcf);
                } else {
                    tap_code(KC_F5);
                }
            } else if (dev_info.sys_sw_state == SYS_SW_MAC) {
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
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    host_system_send(0x9b);
                }
            } else if (dev_info.sys_sw_state == SYS_SW_MAC) {
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
                if (dev_info.sys_sw_state == SYS_SW_MAC) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_3);
                    // wait_ms(50);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_3);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                } else {
                    tap_code(KC_PSCR);
                }
            }
            return false;

        case MAC_PRTA:
            if (record->event.pressed) {
                if (dev_info.sys_sw_state == SYS_SW_WIN) {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_S);
                    //wait_ms(50);
                    wait_ms(TAP_CODE_DELAY);
                    unregister_code(KC_S);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LGUI);
                }

                else {
                    register_code(KC_LGUI);
                    register_code(KC_LSFT);
                    register_code(KC_4);
                    //wait_ms(50);
                    wait_ms(TAP_CODE_DELAY);
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
                /*if (get_highest_layer(layer_state) == M_LAYER || keycode == WIN_LOCK) {
                    keymap_config.no_gui = !keymap_config.no_gui;
                    signal_rgb_led(!keymap_config.no_gui * 3, led_idx.KC_LGUI, UINT8_MAX, 3000);
                    return false;
                }*/
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

        case NUMLOCK_INS:
            if (record->event.pressed) {
                f_numlock_press = 1;
                if (get_mods() & MOD_MASK_CSA) {
                    tap_code(KC_INS);
                    f_numlock_press = 0;
                }
            } else if (f_numlock_press) {
                f_numlock_press = 0;
                tap_code(KC_INS);
            }
            return false;

        case NUMLOCK_IND:
            if (record->event.pressed) {
                kb_config.numlock_state = (kb_config.numlock_state + 1) % (3 - game_mode_enable);
                if (!game_mode_enable) {
                    save_config_to_eeprom();
                }
            }
            return false;

        // TODO: I thought QMK has its own caps-word, but either way not a feature
        // I'm interested in.
        /*
        case CAPS_WORD:
            f_caps_word_tg = record->event.pressed;
            save_config_to_eeprom();
            return false;

        case KC_LSFT:
            if (!record->event.pressed) {
                if ((!kb_config.caps_word_enable || game_mode_enable) && is_caps_word_on()) { caps_word_off(); }
            }
            return true;
        */

        // TODO: I implemnted my own version of this in the custom branch, using KB_SLP
        /*
            case SLEEP_NOW:
            if (USB_ACTIVE) { return false; }
            if (record->event.pressed) {
                wait_ms(100);
                if (kb_config.sleep_mode == 0) { return true; }
                else {
                    f_goto_sleep     = 1;
                    f_goto_deepsleep = 1;
                    no_act_time      = 100;
                    break_all_key();
                }
            }
            return false; */

        case LINK_TO:
            if (record->event.pressed) {
                uint16_t mask = LINK_TIMEOUT ^ LINK_TIMEOUT_ALT;
                kb_config.rf_link_timeout ^= mask; // XOR swap
                save_config_to_eeprom();
            }
            return false;

        // TODO: Apparently an alias for QK_RGB_MATRIX_VALUE_UP
        /*case RM_VALU: // ensure LED powers on with brightness increase
            if (record->event.pressed) {
                pwr_rgb_led_on();
            }
            return true;*/

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
#ifndef NO_DEBUG
                dprintf("debounce press:      %dms\n", kb_config.debounce_press_ms);
#endif
            }
            return false;

        case DEBOUNCE_PRESS_DEC:
            if (record->event.pressed) {
                adjust_debounce(0, false);
#ifndef NO_DEBUG
                dprintf("debounce press:      %dms\n", kb_config.debounce_press_ms);
#endif
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
#ifndef NO_DEBUG
                dprintf("debounce release:      %dms\n", kb_config.debounce_release_ms);
#endif
            }
            return false;

        case DEBOUNCE_RELEASE_DEC:
            if (record->event.pressed) {
                adjust_debounce(0, true);
#ifndef NO_DEBUG
                dprintf("debounce release:      %dms\n", kb_config.debounce_release_ms);
#endif
            }
            return false;

        // TODO
        case DEBOUNCE_T:
            if (record->event.pressed) {
                debounce_type();
            }
            return false;

        // TODO: I think this is sleep timeout, but not sure.
        /*case SLEEP_D:
        case SLEEP_I:
            if (kb_config.sleep_mode == 0) { return true; }
            if (record->event.pressed) {
                uint8_t dir = keycode % SLEEP_D;
                kb_config.light_sleep  = step_helper(dir, kb_config.light_sleep);
#ifndef NO_DEBUG
                dprintf("light sleep time:    %dmin\n", kb_config.light_sleep);
#endif
            }
            return false;*/

        case SLEEP_TIMEOUT_SHOW:
            if (record->event.pressed) {
                f_sleep_timeout_show = !f_sleep_timeout_show;
            }
            return false;

        case SLEEP_TIMEOUT_INC:
            if (record->event.pressed) {
                adjust_sleep_timeout(1);
#ifndef NO_DEBUG
                dprintf("sleep timeout:    %lumin\n", get_sleep_timeout());
#endif
            }
            return false;

        case SLEEP_TIMEOUT_DEC:
            if (record->event.pressed) {
                adjust_sleep_timeout(0);
#ifndef NO_DEBUG
                dprintf("sleep timeout:    %lumin\n", get_sleep_timeout());
#endif
            }
            return false;
        case TOG_BAT_IND_NUM:
            if (record->event.pressed) {
                kb_config.battery_indicator_numeric = !kb_config.battery_indicator_numeric;
                save_config_to_eeprom();
            }
            return false;

        case GAME_MODE:
            if (record->event.pressed) {
                f_gmode_reset_press = 1;
            } else {
                if (f_gmode_reset_press) {
                    f_gmode_reset_press = 0;
                    game_mode_enable = !game_mode_enable;
                    game_mode_tweak();
                }
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
// TODO
/*#ifndef NO_DEBUG
                dprintf("SOCD:    %s(%d)\n", socd_type[kb_config.socd_mode], kb_config.socd_mode);
#endif
*/
            }
            return false;
        case FW_VERSION:
            if (record->event.pressed) {
                SEND_STRING(CFW_VERSION);
            }
            return false;

        case QK_RGB_MATRIX_VALUE_UP:
            if (record->event.pressed) {
                // TODO: older, from the code for RM_VALU.
                //       ensure LED powers on with brightness increase
                pwr_rgb_led_on();
                rgb_matrix_increase_val_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_VALUE_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_val_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_MODE_NEXT:
            if (record->event.pressed) {
                // TODO: adi, not interested in separate game mode settings
                /*if (game_mode_enable) {
                    rgb_matrix_step_game_mode(1);
                    save_rgb_config();
                    return false;
                }*/
                rgb_matrix_step_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_MODE_PREVIOUS:
            if (record->event.pressed) {
                // TODO: adi, not interested in separate game mode settings
                /*if (game_mode_enable) {
                    rgb_matrix_step_game_mode(0);
                    save_rgb_config();
                    return false;
                }*/
                rgb_matrix_step_reverse_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_HUE_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_hue_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_HUE_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_hue_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_SPEED_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_speed_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_SPEED_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_speed_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_SATURATION_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_sat_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_SATURATION_DOWN:
            if (record->event.pressed) {
                rgb_matrix_decrease_sat_noeeprom();
                save_rgb_config();
            }
            return false;

        case QK_RGB_MATRIX_TOGGLE:
            if (record->event.pressed) {
                rgb_matrix_toggle_noeeprom();
                if (!game_mode_enable) {
                    save_rgb_config();
                }
            }
            return false;

        default:
            return true;
    }
    return true;
}

void post_process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifndef NO_DEBUG
        case DB_TOGG:
            dprintf("Keyboard: %s @ QMK: %s | BUILD: %s (%s)\n", QMK_KEYBOARD, QMK_VERSION, QMK_BUILDDATE, QMK_GIT_HASH);
            break;
#endif
        default:
            break;
    }
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

    // low power mode
    power_save();

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
    // mcu_timer6_init();

#if (WORK_MODE == THREE_MODE)
    rf_uart_init();
    wait_ms(500);
    rf_device_init();
#endif
    break_all_key();
    dial_sw_fast_scan();
    load_eeprom_data();

    reset_led_idx();

    // This is necessary because config.h turned DEBUG_MATRIX_SCAN_RATE on,
    // but this automatically turns debug_enable on, which isn't what we want.
#ifndef NO_DEBUG
    debug_enable   = false;
    // debug_matrix   = true;
    // debug_keyboard = true;
    // debug_mouse    = true;
#endif

    // TODO
    interrupt_source_init();

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

    led_show();

#ifndef NO_DEBUG
    user_debug();
#endif

    delay_update_eeprom_data();

    if (game_mode_enable) { return; }

    sleep_handle();

    // TODO
    // idle_enter_sleep();

}

void kb_config_init(void) {
    kb_config.init_flag                    = 0xA5;
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

    kb_config.caps_word_enable             = DEFAULT_CAPS_WORD_ENABLE;
    kb_config.numlock_state                = DEFAULT_NUMLOCK_STATE;
    kb_config.rf_delay_step                = DEFAULT_RF_DELAY_STEP;
    kb_config.debounce_type                = DEFAULT_DEBOUNCE_TYPE;
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
    //write_custom_config(&kb_config, 0, sizeof(kb_config));
    call_update_eeprom_data(&user_update);
}

void save_config_to_eeprom_now(void) {
    write_custom_config(&kb_config, 0, sizeof(kb_config));
}

void save_rgb_config(void) {
    // TODO: adi, not interested in separate game mode settings
    /*if (game_mode_enable) {
        save_config_to_eeprom();
    } else */{
        call_update_eeprom_data(&rgb_update);
    }
}

#ifdef VIA_ENABLE
void via_init_kb(void) {
    kb_config_init();
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
            kb_config.side_colour = *value_data;
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
            kb_config.right_side_colour = *value_data;
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
            *value_data = kb_config.side_colour;
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
            *value_data = kb_config.right_side_colour;
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
