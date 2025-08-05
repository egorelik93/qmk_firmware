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

#include <stdbool.h>
#include <stdint.h>
#include "kb_util.h"
#include "ansi.h"
#include "usb_main.h"
#include "mcu_pwr.h"
#include "config.h"
#include "eeconfig.h"
#include "color.h"
#include "settings.h"
#include "host.h"
#include "layer_names.h"
#include "quantum/keymap_introspection.h"

kb_config_t     kb_config;
led_index_t     led_idx;
DEV_INFO_STRUCT dev_info = {
    .rf_battery = 100,
    .link_mode  = LINK_USB,
    .rf_state   = RF_IDLE,
};
bool f_bat_hold        = 0;
bool f_sys_show        = 0;
bool f_sleep_show      = 0;
bool f_send_channel    = 0;
bool f_dial_sw_init_ok = 0;
bool f_rf_sw_press     = 0;
uint8_t f_rf_dfu_press    = 0;
bool f_dev_reset_press = 0;
bool f_rgb_test_press  = 0;
uint8_t f_caps_word_tg    = 0;
uint8_t f_numlock_press = 0;
uint8_t f_gmode_reset_press = 0;
bool f_bat_num_show    = 0;
bool f_debounce_press_show   = 0;
bool f_debounce_release_show = 0;
bool f_sleep_timeout_show    = 0;
bool game_mode_enable    = 0;
bool rgb_power_save      = 0;

bool f_sleep_now       = 0;

uint8_t        rgb_required          = 0;
uint8_t        rf_blink_cnt          = 0;
uint8_t        rf_sw_temp            = 0;
uint8_t        host_mode             = 0;
uint16_t       rf_linking_time       = 0;
uint16_t       rf_link_show_time     = 0;
uint32_t       no_act_time           = 0;
uint16_t       dev_reset_press_delay = 0;
uint16_t       rf_sw_press_delay     = 0;
uint16_t       rgb_test_press_delay  = 0;
uint16_t       rgb_led_last_act      = 0;
uint16_t       side_led_last_act     = 0;
host_driver_t *m_host_driver         = 0;
rgb_t          bat_pct_rgb           = {.r = 0x80, .g = 0x80, .b = 0x00};

uint16_t       link_timeout            = T_MIN;

uint32_t       eeprom_update_timer     = 0;
bool           user_update             = 0;
bool           rgb_update              = 0;

// TODO
char           debounce_algo[3][20]    = { "sym_defer_pk", "asym_eager_defer_pk", "sym_eager_pr" };

extern host_driver_t      rf_host_driver;

/**
 * @brief  Initialize GPIO.
 */
void gpio_init(void) {
    /* power on all LEDs */
    pwr_rgb_led_on();
    pwr_side_led_on();

    /* set side LED pin output low */
    gpio_set_pin_output_push_pull(DRIVER_SIDE_PIN);
    gpio_write_pin_low(DRIVER_SIDE_PIN);

#if (WORK_MODE == THREE_MODE)
    /* config RF module pin */
    gpio_set_pin_output_push_pull(NRF_WAKEUP_PIN);
    gpio_write_pin_high(NRF_WAKEUP_PIN);
    gpio_set_pin_input_high(NRF_TEST_PIN);

    /* reset RF module */
    gpio_set_pin_output_push_pull(NRF_RESET_PIN);
    gpio_write_pin_low(NRF_RESET_PIN);
    wait_ms(50);
    gpio_write_pin_high(NRF_RESET_PIN);

    /* connection mode switch pin */
    gpio_set_pin_input_high(DEV_MODE_PIN);
#endif
    /* config keyboard OS switch pin */
    gpio_set_pin_input_high(SYS_MODE_PIN);

    // TODO: From ryodeushii - but not convinced this is right
    /*
    // open power
    setPinOutput(DC_BOOST_PIN);
    writePinHigh(DC_BOOST_PIN);

    setPinOutput(DRIVER_LED_CS_PIN);
    writePinLow(DRIVER_LED_CS_PIN);

    setPinOutput(DRIVER_SIDE_CS_PIN);
    writePinLow(DRIVER_SIDE_CS_PIN);*/
}

/**
 * @brief  long press key process.
 */
void long_press_key(void) {
    static uint32_t long_press_timer = 0;

    if (timer_elapsed32(long_press_timer) < 100) {
        return;
    }
    long_press_timer = timer_read32();

    // Open a new RF device
    if (f_rf_sw_press) {
#if (WORK_MODE == THREE_MODE)
        rf_sw_press_delay++;
        if (rf_sw_press_delay >= RF_LONG_PRESS_DELAY) {
            f_rf_sw_press = 0;

            dev_info.link_mode   = rf_sw_temp;
            dev_info.rf_channel  = rf_sw_temp;
            dev_info.ble_channel = rf_sw_temp;

            uint8_t timeout = 5;
            while (timeout--) {
                uart_send_cmd(CMD_NEW_ADV, 0, 1);
                wait_ms(20);
                uart_receive_pro();
                if (f_rf_new_adv_ok) {
                    break;
                }
            }
        }
#endif
    } else {
        rf_sw_press_delay = 0;
    }

    // The device is restored to factory settings
    if (f_dev_reset_press) {
        dev_reset_press_delay++;
        if (dev_reset_press_delay >= DEV_RESET_PRESS_DELAY) {
            f_dev_reset_press = 0;

            if (dev_info.link_mode != LINK_USB) {
                if (dev_info.link_mode != LINK_RF_24) {
                    dev_info.link_mode   = LINK_BT_1;
                    dev_info.ble_channel = LINK_BT_1;
                    dev_info.rf_channel  = LINK_BT_1;
                }
            } else {
                dev_info.ble_channel = LINK_BT_1;
                dev_info.rf_channel  = LINK_BT_1;
            }

            uart_send_cmd(CMD_SET_LINK, 10, 10);
            wait_ms(500);
            uart_send_cmd(CMD_CLR_DEVICE, 10, 10);

            void device_reset_show(void);
            void device_reset_init(void);

            eeconfig_init();
            device_reset_show();
            device_reset_init();
            // TODO
            eeconfig_update_rgb_matrix_default();

            if (dev_info.sys_sw_state == SYS_SW_MAC) {
                default_layer_set(1 << 0);
                keymap_config.nkro = 0;
            } else {
                default_layer_set(1 << 2);
                keymap_config.nkro = 1;
            }
        }
    } else {
        dev_reset_press_delay = 0;
    }

    // Enter the RGB test mode
    if (f_rgb_test_press) {
        rgb_test_press_delay++;
        if (rgb_test_press_delay >= RGB_TEST_PRESS_DELAY) {
            f_rgb_test_press = 0;
            rgb_test_show();
        }
    } else {
        rgb_test_press_delay = 0;
    }

    // NumLock Press
    if (f_numlock_press) {
        f_numlock_press++;
        if (f_numlock_press > MICRO_PRESS_DELAY) {
            tap_code(KC_NUM);
            f_numlock_press = 0;
        }
    } else {
        f_numlock_press = 0;
    }

    // Trigger Game Mode Reset
    if (f_gmode_reset_press) {
        f_gmode_reset_press++;
        if (f_gmode_reset_press > MEDIUM_PRESS_DELAY) {
            //game_config_reset(1);
            game_mode_tweak();
            f_gmode_reset_press = 0;
        }
    } else {
        f_gmode_reset_press = 0;
    }

    // Toggle Caps Word
    if (f_caps_word_tg) {
        f_caps_word_tg++;
        if (f_caps_word_tg > SMALL_PRESS_DELAY) {
            kb_config.caps_word_enable = !kb_config.caps_word_enable;
            f_caps_word_tg = 0;
#ifndef NO_DEBUG
            dprintf("caps_word_state: %s\n", kb_config.caps_word_enable ? "ON" : "OFF");
#endif
            // TODO
            signal_rgb_led(kb_config.caps_word_enable * 3, led_idx.KC_CAPS, UINT8_MAX, CAPS_WORD_IDLE_TIMEOUT);
        }
    } else {
        f_caps_word_tg = 0;
    }

    // Toggle RF Delay
    if (f_rf_dfu_press) {
        f_rf_dfu_press++;
        if (f_rf_dfu_press > MEDIUM_PRESS_DELAY) {
            f_rf_dfu_press = 0;
#ifndef NO_DEBUG
            dprintf("RF DFU Mode Enabled\n");
#endif
            uart_send_cmd(CMD_RF_DFU, 10, 20);
            // TODO
            signal_rgb_led(9, led_idx.RF_DFU, UINT8_MAX, UINT16_MAX);
        }
    } else {
        f_rf_dfu_press = 0;
    }
}

// RF repeat incase key break doesn't register properly...
void rf_repeat_key_break(void) {
    if (dev_info.link_mode == LINK_USB) return;
    for (uint8_t i = 0; i < 10; i++) {
        uart_send_report_repeat();
        wait_ms(5);
        uart_receive_pro();
        wait_ms(5);
    }
}

/**
 * @brief  Release all keys, clear keyboard report.
 */
void break_all_key(void) {
    // bool nkro_temp = keymap_config.nkro;

    // break current keyboard mode
    clear_weak_mods();
    clear_mods();
    clear_keyboard(); // this already sends the report.
    // break nkro key
    wait_ms(10);
    // break byte key
    rf_repeat_key_break();

    // break the other keyboard mode
    // probably not necessary, commenting out for now.
    /*
    keymap_config.nkro = !keymap_config.nkro;
    clear_keyboard();
    wait_ms(10);

    keymap_config.nkro = nkro_temp;
    */
    void clear_report_buffer_and_queue(void);
    clear_report_buffer_and_queue();
}

/**
 * @brief  switch device link mode.
 * @param mode : link mode
 */
void switch_dev_link(uint8_t mode) {
    if (mode > LINK_USB) {
        return;
    }
    no_act_time = 0;

    break_all_key();

    dev_info.link_mode = mode;

    dev_info.rf_state = RF_IDLE;
    f_send_channel    = 1;

    if (mode == LINK_USB) {
        host_mode = HOST_USB_TYPE;
        host_set_driver(m_host_driver);
        rf_link_show_time = 0;
    } else {
        host_mode = HOST_RF_TYPE;
        host_set_driver(&rf_host_driver);
    }
}

/**
 * @brief  scan dial switch.
 */
void dial_sw_scan(void) {
    uint8_t         dial_scan       = 0;
    static uint8_t  dial_save       = 0xf0;
    static uint8_t  debounce        = 0;
    static uint32_t dial_scan_timer = 0;
    static bool     f_first         = true;

    if (!f_first) {
        if (timer_elapsed32(dial_scan_timer) < 20) {
            return;
        }
    }
    dial_scan_timer = timer_read32();

#if (WORK_MODE == THREE_MODE)
    gpio_set_pin_input_high(DEV_MODE_PIN);
#endif
    gpio_set_pin_input_high(SYS_MODE_PIN);

#if (WORK_MODE == THREE_MODE)
    if (gpio_read_pin(DEV_MODE_PIN)) {
        dial_scan |= 0X01;
    }
#endif
    if (gpio_read_pin(SYS_MODE_PIN)) {
        dial_scan |= 0X02;
    }

    if (dial_save != dial_scan) {
        break_all_key();

        no_act_time     = 0;
        rf_linking_time = 0;

        dial_save         = dial_scan;
        debounce          = 25;
        f_dial_sw_init_ok = 0;
        return;
    } else if (debounce) {
        debounce--;
        return;
    }

#if (WORK_MODE == THREE_MODE)
    if (dial_scan & 0x01) {
        if (dev_info.link_mode != LINK_USB) {
            switch_dev_link(LINK_USB);
        }
    } else {
        if (dev_info.link_mode != dev_info.rf_channel) {
            switch_dev_link(dev_info.rf_channel);
        }
    }
#endif

    if (dial_scan & 0x02) {
        if (dev_info.sys_sw_state != SYS_SW_MAC) {
            f_sys_show = 1;
            default_layer_set(1 << 0);
            dev_info.sys_sw_state = SYS_SW_MAC;
            break_all_key();
            keymap_config.nkro = 0;
            reset_led_idx();
        }
    } else {
        if (dev_info.sys_sw_state != SYS_SW_WIN) {
            f_sys_show = 1;
            default_layer_set(1 << 2);
            dev_info.sys_sw_state = SYS_SW_WIN;
            break_all_key();
            keymap_config.nkro = 1;
            reset_led_idx();
        }
    }

    if (f_dial_sw_init_ok == 0) {
        f_dial_sw_init_ok = 1;
        f_first           = false;

#if (WORK_MODE == THREE_MODE)
        if (dev_info.link_mode != LINK_USB) {
            host_set_driver(&rf_host_driver);
        }
#endif
    }
}

/**
 * @brief  power on scan dial switch.
 */
void dial_sw_fast_scan(void) {
    uint8_t dial_scan_dev  = 0;
    uint8_t dial_scan_sys  = 0;
    uint8_t dial_check_dev = 0;
    uint8_t dial_check_sys = 0;
    uint8_t debounce       = 0;

#if (WORK_MODE == THREE_MODE)
    gpio_set_pin_input_high(DEV_MODE_PIN);
#endif
    gpio_set_pin_input_high(SYS_MODE_PIN);

    // Debounce to get a stable state
    for (debounce = 0; debounce < 10; debounce++) {
        dial_scan_dev = 0;
        dial_scan_sys = 0;
#if (WORK_MODE == THREE_MODE)
        if (gpio_read_pin(DEV_MODE_PIN)) {
            dial_scan_dev = 0x01;
        } else {
            dial_scan_dev = 0;
        }
#endif
        if (gpio_read_pin(SYS_MODE_PIN)) {
            dial_scan_sys = 0x01;
        } else {
            dial_scan_sys = 0;
        }
        if ((dial_scan_dev != dial_check_dev) || (dial_scan_sys != dial_check_sys)) {
            dial_check_dev = dial_scan_dev;
            dial_check_sys = dial_scan_sys;
            debounce       = 0;
        }
        wait_ms(1);
    }

#if (WORK_MODE == THREE_MODE)
    // RF link mode
    if (dial_scan_dev) {
        if (dev_info.link_mode != LINK_USB) {
            switch_dev_link(LINK_USB);
        }
    } else {
        if (dev_info.link_mode != dev_info.rf_channel) {
            switch_dev_link(dev_info.rf_channel);
        }
    }
#endif

    // Win or Mac
    if (dial_scan_sys) {
        if (dev_info.sys_sw_state != SYS_SW_MAC) {
            break_all_key();
            default_layer_set(1 << 0);
            dev_info.sys_sw_state = SYS_SW_MAC;
            keymap_config.nkro    = 0;
            reset_led_idx();
        }
    } else {
        if (dev_info.sys_sw_state != SYS_SW_WIN) {
            break_all_key();
            default_layer_set(1 << 2);
            dev_info.sys_sw_state = SYS_SW_WIN;
            keymap_config.nkro    = 1;
            reset_led_idx();
        }
    }
}

// TODO
/**
 * @brief  timer process.
 */
void timer_pro(void) {
    static uint32_t interval_timer = 0;
    static uint16_t missing_time   = 0;
    static uint16_t adjust_time    = 0;
    static bool     f_first        = true;

    if (f_first) {
        f_first        = false;
        interval_timer = timer_read32();
        m_host_driver  = host_get_driver();
    }

    // step 10ms
    if (timer_elapsed32(interval_timer) < TIMER_STEP) {
        return;
    }

    if (!game_mode_enable && no_act_time >= 1000 ) {
        missing_time += (timer_elapsed32(interval_timer) - 10);
        if (missing_time >= 100 && no_act_time % 3000 != 0) {
            adjust_time  = missing_time / 10;
            missing_time = 0;
        }
    }

    interval_timer = timer_read32();

    if (rf_link_show_time < RF_LINK_SHOW_TIME) {
        rf_link_show_time++;
    }

    if (no_act_time < UINT32_MAX) {
        no_act_time += adjust_time + 1;
    }

#if (WORK_MODE == THREE_MODE)
    if (rf_linking_time < UINT16_MAX) {
        rf_linking_time += adjust_time + 1;
    }
#endif

    if (rgb_led_last_act < UINT16_MAX) {
        rgb_led_last_act += adjust_time + 1;
    }

    if (side_led_last_act < UINT16_MAX) {
        side_led_last_act += adjust_time + 1;
    }

    adjust_time = 0;
}

/**
 * @brief  load eeprom data.
 */
void load_eeprom_data(void) {
    load_config_from_eeprom();
    if (kb_config.default_brightness_flag != 0x45) {
        kb_config_reset();
    }
}

void call_update_eeprom_data(bool* eeprom_update_init) {
    *eeprom_update_init = 1;
    eeprom_update_timer = 0;
}

// TODO
/**
 * @brief User config update to eeprom with delay
 */
void delay_update_eeprom_data(void) {
    if (eeprom_update_timer == 0) {
        if (user_update || rgb_update) { eeprom_update_timer = timer_read32(); }
        return;
    }
    if (timer_elapsed32(eeprom_update_timer) < (1000 * 30)) { return; }
    if (user_update) {
        save_config_to_eeprom_now();
    }

    user_update         = 0;
#ifndef NO_DEBUG
    dprint("Updating EEPROM: kb_config\n");
#endif

    if (rgb_update) {
        eeconfig_force_flush_rgb_matrix();
        rgb_update          = 0;
#ifndef NO_DEBUG
        dprint("Updating EEPROM:  rgb_config\n");
#endif
    }
    eeprom_update_timer = 0;
}

// TODO
void game_mode_tweak(void)
{
    if (game_mode_enable) {
        if (eeprom_update_timer != 0) {
            eeprom_update_timer = 0;
            if (user_update) {
                save_config_to_eeprom_now();
                user_update = 0;
#ifndef NO_DEBUG
                dprint("Updating EEPROM: user_config\n");
#endif
            }
            if (rgb_update) {
                eeconfig_force_flush_rgb_matrix();
                rgb_update          = 0;
#ifndef NO_DEBUG
                dprint("Updating EEPROM:  rgb_config\n");
#endif
            }
        }

        pwr_rgb_led_on();
        // TODO: adi, not interested in separate game mode rgb
        /*rgb_matrix_mode_noeeprom(kb_config.game_rgb_mod);
        rgb_matrix_config.hsv.v    = kb_config.game_rgb_val;
        rgb_matrix_config.hsv.h    = kb_config.game_rgb_hue;
        rgb_matrix_config.hsv.s    = kb_config.game_rgb_sat;*/
        kb_config.side_mode   = 2;
        kb_config.side_rgb    = 0;
        if (kb_config.numlock_state != 0) { kb_config.numlock_state = 1; }
    } else {
        rgb_matrix_reload_from_eeprom();
        load_config_from_eeprom();
    }

#ifndef NO_DEBUG
    dprintf("debounce press:   %dms\n", kb_config.debounce_press_ms);
    dprintf("debounce release: %dms\n", kb_config.debounce_release_ms);
    dprintf("debounce type:    %s\n", debounce_algo[kb_config.debounce_type]);
#endif
    pwr_rgb_led_on();
    signal_rgb_led(game_mode_enable * 3, led_idx.KC_G, UINT8_MAX, 2000);
}

/**
 * @brief User config to default setting.
 */
void kb_config_reset(void) {
    // JinCao version:
    /* first power on, set rgb matrix brightness off */
    // rgb_matrix_sethsv(255, 255, 0);

    // ryodeushii
    rgb_matrix_enable();
    rgb_matrix_mode(RGB_MATRIX_DEFAULT_MODE);
    rgb_matrix_set_speed(255 - RGB_MATRIX_SPD_STEP * 2);
    rgb_matrix_sethsv(RGB_DEFAULT_COLOR, 255, RGB_MATRIX_MAXIMUM_BRIGHTNESS - RGB_MATRIX_VAL_STEP * 2);

    // Original, which I prefer:
    /* upon first power on, set RGB matrix brightness to middle level */
    //rgb_matrix_sethsv(255, 255, RGB_MATRIX_MAXIMUM_BRIGHTNESS - RGB_MATRIX_VAL_STEP * 2);

    kb_config_init();
    // mark config as initiated
    kb_config.default_brightness_flag = 0x45;
    keymap_config.no_gui = 0;

    user_config_override();

    save_config_to_eeprom_now();
}

/**
 * @brief  Show battery percentage LEDs
 */
void bat_pct_led_kb(void) {
    uint8_t bat_percent = dev_info.rf_battery;

    if (bat_percent >= 100) {
        bat_percent = 100;
    }

    // JinCao version of logic
    /*
    uint8_t led_idx_tens = bat_percent / 10;
    uint8_t led_idx_ones = bat_percent % 10;

    // set F key for battery percentage tens (e.g, 10%)
    if (led_idx_tens > 0) {
        user_set_rgb_color(led_idx_tens, bat_pct_rgb.r, bat_pct_rgb.g, bat_pct_rgb.b);
    }

    // set number key for battery percentage ones (e.g., 5 in 15%)
    if (led_idx_ones == 0) {
        user_set_rgb_color(20, bat_pct_rgb.r, bat_pct_rgb.g, bat_pct_rgb.b);
    } else {
        user_set_rgb_color(30 - led_idx_ones, bat_pct_rgb.r, bat_pct_rgb.g, bat_pct_rgb.b);
    }*/

    uint8_t led_idx_tens = two_digit_decimals_led(bat_percent);
    uint8_t led_idx_ones = two_digit_ones_led(bat_percent);

    user_set_rgb_color(led_idx_tens, bat_pct_rgb.r, bat_pct_rgb.g, bat_pct_rgb.b);
    user_set_rgb_color(led_idx_ones, bat_pct_rgb.r, bat_pct_rgb.g, bat_pct_rgb.b);
}

/**
 * @brief toggle usb sleep on/off
 */
void toggle_usb_sleep(void) {
    f_sleep_show          = 1;
    kb_config.usb_sleep_toggle = !kb_config.usb_sleep_toggle;
    save_config_to_eeprom();
}

/**
 * @brief Toggle caps indication between side led / under key / off
 */
void toggle_caps_indication(void) {
    if (kb_config.caps_indicator_type == CAPS_INDICATOR_OFF) {
        kb_config.caps_indicator_type = CAPS_INDICATOR_SIDE; // set to initial state, when last state reached
    } else {
        kb_config.caps_indicator_type += 1;
    }

    save_config_to_eeprom();
}

/**
 * @brief Updates RGB value and sets current bat percentage.
 */
void update_bat_pct_rgb(uint8_t bat_percent) {
    static uint8_t  bat_pct          = 0;
    static uint32_t bat_per_debounce = 0;

    if (bat_percent > 100) {
        bat_percent = 100;
    }

    // no change, update timer and move on.
    if (bat_pct == bat_percent) {
        bat_per_debounce = timer_read32();
        return;
    }

    // update only when battery stabilizes. Battery level could blip.
    if (timer_elapsed32(bat_per_debounce) <= 1000) {
        return;
    }

    bat_pct             = bat_percent;
    dev_info.rf_battery = bat_percent;

    // 120 hue is green, 0 is red on a 360 degree wheel but QMK is a uint8_t
    // so it needs to convert to relative to 255 - so green is actually 85.
    uint8_t h = 85;
    if (bat_pct <= 20) {
        h = 0; // red
    } else if (bat_pct <= 40) {
        h = 21; // orange
    } else if (bat_pct <= 80) {
        h = 43; // yellow
    }

    hsv_t hsv = {
        .h = h,
        .s = 255,
        .v = 128, // 50% max brightness
    };

    bat_pct_rgb = hsv_to_rgb_nocie(hsv); // this results in same calculation as colour pickers.
}

/**
 * @brief Wrapper for rgb_matrix_set_color for sleep.c logic usage.
 */
void user_set_rgb_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    if (red || green || blue) {
        rgb_led_last_act = 0;
        pwr_rgb_led_on(); // turn on LEDs
    }
    rgb_matrix_set_color(index, red, green, blue);
}

// TODO: Do we need this
/*uint8_t step_helper(uint8_t dir, uint8_t value) {
    uint8_t step, my_color, end_led;
    if (value < 11 - dir) {
        step = 1;
    } else if (value < 31 - dir) {
        step = 2;
    } else if (value < 76 - dir) {
        step = 5;
    } else {
        step = 25;
    }

    if (dir) {
        if (value < 100) { value += step; }
    } else {
        if (value > 1)   { value -= step; }
    }

    if (value <= 10) {
        my_color = 3;
        end_led  = value - 1;
    } else if (value <= 30) {
        my_color = 2;
        end_led  = (value - 10) / 2 - 1;
    } else if (value <= 75) {
        my_color = 0;
        end_led  = (value - 30) / 5 - 1;
    } else {
        my_color = 7;
        end_led  = 9;
    }

    signal_rgb_led(my_color, led_idx.KC_F1, led_idx.KC_F1 + end_led, 3000);
    return value;
}*/

// TODO
void power_save(void) {
    static uint16_t power_check_timer      = 0;
    if (timer_elapsed32(power_check_timer) < 5000) { return; }

    if (dev_info.rf_battery > low_bat_level && rgb_power_save == 1) {
        rgb_matrix_reload_from_eeprom();
        load_eeprom_data();
        rgb_power_save = 0;
    } else if ((rgb_matrix_config.hsv.v > 0 || kb_config.side_light > 1) && dev_info.rf_battery < low_bat_level) {
        rgb_matrix_config.hsv.v   = 0;
        kb_config.side_light      = 1;
        rgb_power_save            = 1;
    }
}

/**
 * @brief Handle LED power
 * @note Turn off LEDs if not used to save some power. This is ported
 *       from older Nuphy leaks.
 */
void led_power_handle(void) {
    static uint32_t interval = 0;

    uint16_t led_interval = rgb_required == 1 ? 10: 500;

    if (timer_elapsed32(interval) < led_interval || f_wakeup_prepare || game_mode_enable) // only check once in a while, less flickering for unhandled cases
        return;

    interval = timer_read32();

    if (rgb_led_last_act > 100) { // 10ms intervals
        if (rgb_matrix_is_enabled() && (rgb_matrix_get_val() != 0 || rgb_required > 0)) {
            pwr_rgb_led_on();
            rgb_required = 0;
        } else { // brightness is 0 or RGB off.
            pwr_rgb_led_off();
        }
    }

    if (side_led_last_act > 100) { // 10ms intervals
        if (kb_config.side_light == 0) {
            pwr_side_led_off();
        } else {
            pwr_side_led_on();
        }
    }
}

void toggle_sleep_mode(void) {
    if (kb_config.sleep_mode > SLEEP_MODE_OFF) {
        kb_config.sleep_mode--;
    } else {
        kb_config.sleep_mode = SLEEP_MODE_DEEP;
    }
    f_sleep_show = 1;
    save_config_to_eeprom();
}

void reset_led_idx(void) {
    led_idx.KC_CAPS  = get_led_idx(KC_CAPS);
    led_idx.KC_LGUI  = get_led_idx(KC_LGUI);

    uint16_t numlock_keys[4] = { KC_NUM, NUMLOCK_INS, KC_INS, KC_MINS };
    for (uint8_t i = 0; i < sizeof_array(numlock_keys); i++) {
        led_idx.KC_NUM  = get_led_idx(numlock_keys[i]);
        if (led_idx.KC_NUM != UINT8_MAX) { break; }
    }

    led_idx.KC_D     = get_led_idx(DEBOUNCE_T);
    led_idx.KC_G     = get_led_idx(GAME_MODE);
    led_idx.KC_F1    = get_led_idx(KC_F1);
    led_idx.RF_DFU   = get_led_idx(RF_DFU);
    led_idx.SOCD_TOG = get_led_idx(SOCDTOG);
    led_idx.KC_F12   = get_led_idx(KC_F12);
    led_idx.KC_GRV   = get_led_idx(KC_GRV);
}

uint8_t get_led_index(uint8_t row, uint8_t col) {
    return g_led_config.matrix_co[row][col];
}

uint8_t get_led_idx(uint16_t keycode) {
    uint8_t current_default = dev_info.sys_sw_state == SYS_SW_WIN ? WIN_BASE : MAC_BASE;
    uint8_t check_layers[3] = { current_default, (current_default + 1), M_LAYER };
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            for (uint8_t layer = 0; layer < 3; ++layer) {
                if (keycode_at_keymap_location(check_layers[layer], row, col) == keycode) {
                    return get_led_index(row, col);
                }
            }
        }
    }
    return UINT8_MAX;
}

/**
 * @brief get LED if for first digit from double digit number. Esc = 0
 */
uint8_t two_digit_decimals_led(uint8_t value) {
    if (value > 99) {
        return get_led_index(0, 0);
    }

    uint8_t dec = value / 10;

    uint8_t dec_led_idx = get_led_index(0, dec);

    return dec_led_idx;
}

/**
 * @brief get LED if for second digit from double digit number 0 = 0
 */
uint8_t two_digit_ones_led(uint8_t value) {
    if (value > 99) {
        return get_led_index(0, 0);
    }

    uint8_t ones = value % 10;
    if (ones == 0) {
        ones = 10;
    }
    uint8_t ones_led_idx = get_led_index(1, ones);

    return ones_led_idx;
}

void adjust_debounce(uint8_t dir, DEBOUNCE_EVENT debounce_event) {
#if DEBOUNCE > 0
    if (dir) {
        if (debounce_event == DEBOUNCE_PRESS && kb_config.debounce_press_ms < 99) {
            kb_config.debounce_press_ms += DEBOUNCE_STEP;
        } else if (debounce_event == DEBOUNCE_RELEASE && kb_config.debounce_release_ms < 99) {
            kb_config.debounce_release_ms += DEBOUNCE_STEP;
        }
    } else if (!dir) {
        if (debounce_event == DEBOUNCE_PRESS && kb_config.debounce_press_ms > 0) {
            kb_config.debounce_press_ms -= DEBOUNCE_STEP;
        } else if (debounce_event == DEBOUNCE_RELEASE && kb_config.debounce_release_ms > 0) {
            kb_config.debounce_release_ms -= DEBOUNCE_STEP;
        }
    }
    save_config_to_eeprom();
#endif
}

void adjust_sleep_timeout(uint8_t dir) {
    if (kb_config.sleep_mode) {
        if (kb_config.sleep_timeout > 1 && !dir) {
            kb_config.sleep_timeout -= SLEEP_TIMEOUT_STEP;
        } else if (kb_config.sleep_timeout < 60 && dir) {
            kb_config.sleep_timeout += SLEEP_TIMEOUT_STEP;
        }
        save_config_to_eeprom();
    }
}

uint32_t get_sleep_timeout(void) {
    if (kb_config.sleep_mode == SLEEP_MODE_OFF) return 0;

    uint8_t sleep_timeout = kb_config.sleep_timeout;
    // Shouldn't happen, but an incorrect eeprom size can wipe the setting.
    if (sleep_timeout == 0) {
        sleep_timeout = DEFAULT_SLEEP_TIMEOUT;
    }

    return sleep_timeout * 60 * 1000 / TIMER_STEP;
}

void link_mode_set(void) {
    dev_info.link_mode   = rf_sw_temp;
    dev_info.rf_channel  = rf_sw_temp;
    dev_info.ble_channel = rf_sw_temp;
    uart_send_cmd(CMD_SET_LINK, 10, 20);
}

// TODO: adi. One of the modes is not working.
void debounce_type(void) {
    //kb_config.debounce_type = (kb_config.debounce_type + 1) % 3;
    //save_config_to_eeprom();

#ifndef NO_DEBUG
    dprintf("debounce type: %s\n", debounce_algo[kb_config.debounce_type]);
#endif
    signal_rgb_led(kb_config.debounce_type == 1 ? 3 : kb_config.debounce_type, led_idx.KC_D, UINT8_MAX, 3000);
}

#ifndef NO_DEBUG
void user_debug(void) {
    static uint32_t last_print = 0;
    if (no_act_time == 0 || no_act_time == last_print || no_act_time % 3000 != 0) { return; }
    if (!USB_ACTIVE && debug_enable) {
        debug_enable = false;
        print("DEBUG: disabled.\n");
    }
    last_print = no_act_time;
    dprintf("no_act_time: %lds\n", no_act_time / 100);
}
#endif

void debug_show_led(uint8_t show) {
    if (show >= 100) {
        show = 100;
    }

    uint8_t led_idx_tens = two_digit_decimals_led(show);
    uint8_t led_idx_ones = two_digit_ones_led(show);

    user_set_rgb_color(led_idx_tens, 0x80, 0x00, 0x00);
    user_set_rgb_color(led_idx_ones, 0x80, 0x00, 0x00);
}
