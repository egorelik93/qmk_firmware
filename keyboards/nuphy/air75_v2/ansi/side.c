// Copyright 2023 Ryodeushii (@ryodeushii)
// SPDX-License-Identifier: GPL-2.0-or-later
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

#include "side.h"
#include <stdint.h>
#include "config.h"
#include "kb_util.h"
#include "ansi.h"
#include "color.h"
#include "host.h"
#include "rgb_matrix.h"
#include "timer.h"
#include "side_table.h"
#include "ws2812.h"
#include "mcu_pwr.h"

// clang-format on

// TODO
uint8_t  low_bat_level        = 15; // 20

// TODO
uint8_t  sys_light            = 3;

bool    flush_side_leds = false;
uint8_t side_play_point = 0;

rgb_t current_rgb = {.r = 0x00, .g = 0x00, .b = 0x00};
rgb_led_t side_leds[SIDE_LED_NUM] = {0};

bool f_charging = 1;

uint8_t low_bat_blink_cnt = 6;
bool    do_refresh        = false;

uint8_t  rgb_color            = 0;
uint8_t  rgb_start_led        = 0;
uint8_t  rgb_end_led          = 0;
uint32_t rgb_show_time        = 0;
uint32_t rgb_indicator_timer  = 0;

#define LEFT_LED_TOP(i) i
#define RIGHT_LED_TOP(i) (11 - i)

#if RF_SIDE == LEFT_SIDE
    #define RF_SIDE_TOP(i) LEFT_LED_TOP(i)
    #define SET_RF_SIDE_RGB set_left_rgb
#else
    #define RF_SIDE_TOP(i) RIGHT_LED_TOP(i)
    #define SET_RF_SIDE_RGB set_right_rgb
#endif

#if BAT_SIDE == LEFT_SIDE
    #define BAT_SIDE_TOP(i) LEFT_LED_TOP(i)
    #define SET_BAT_SIDE_RGB set_left_rgb
#else
    #define BAT_SIDE_TOP(i) RIGHT_LED_TOP(i)
    #define SET_BAT_SIDE_RGB set_right_rgb
#endif

#if SYS_SIDE == LEFT_SIDE
    #define SYSTEM_SIDE_TOP(i) i
    #define SET_SYS_SIDE_RGB set_left_rgb
#else
    #define SYSTEM_SIDE_TOP(i) (11 - i)
    #define SET_SYS_SIDE_RGB set_right_rgb
#endif

#define r_temp current_rgb.r
#define g_temp current_rgb.g
#define b_temp current_rgb.b

void side_ws2812_setleds(rgb_led_t *ledarray, uint16_t leds);
void rgb_matrix_update_pwm_buffers(void);

#ifdef SIDE_SEPARATE
extern uint8_t right_side_play_point;

void right_side_led_loop(void);
void set_right_side_rgb(uint8_t r, uint8_t g, uint8_t b);
#endif

// Copied from old nuphy code. Check if side RGB has values set.
bool is_side_rgb_off(void) {
    for (int i = 0; i < SIDE_LED_NUM; i++) {
        if ((side_leds[i].r != 0) || (side_leds[i].g != 0) || (side_leds[i].b != 0)) {
            return false;
        }
    }
    return true;
}

// TODO
bool is_side_ws2812_off(void) {
    return is_side_rgb_off();
}

/**
 * @brief  side leds set color value.
 * @param  i: index of side_leds[].
 * @param  ...
 */
void side_rgb_set_color(int i, uint8_t r, uint8_t g, uint8_t b) {
    r >>= 2, g >>= 2, b >>= 2; // this is necessary apparently for this driver.
    if (side_leds[i].r != r || side_leds[i].g != g || side_leds[i].b != b) {
        flush_side_leds = true;
    }
    side_leds[i].r = r;
    side_leds[i].g = g;
    side_leds[i].b = b;
}

// TODO
void side_ws2812_set_color(int i, uint8_t r, uint8_t g, uint8_t b) {
    side_rgb_set_color(i, r, g, b);
}

/**
 * @brief Set all side LED colour
 */
void side_rgb_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
#ifdef SIDE_SEPARATE
    side_rgb_set_color_left(r, g, b);
#else
    for (uint8_t i = 0; i < SIDE_LED_NUM; i++) {
        side_rgb_set_color(i, r, g, b);
    }
#endif
}

void side_rgb_set_color_left(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < SIDE_LINE; i++) {
        side_rgb_set_color(i, r, g, b);
    }
}

void side_rgb_set_color_right(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = SIDE_LINE; i < SIDE_LED_NUM; i++) {
        side_rgb_set_color(i, r, g, b);
    }
}

// TODO
void side_ws2812_set_color_strip(uint8_t side, uint8_t r, uint8_t g, uint8_t b) {
    // side = 1 => left
    // side = 2 => right
    // side = 3 => both
    uint8_t start = 0;
    uint8_t end   = SIDE_LED_NUM;
    if (side == LEFT_SIDE)  { end = end - SIDE_LINE; }
    if (side == RIGHT_SIDE) { start = start + SIDE_LINE; }

    for (uint8_t i = start; i < end; i++) {
        side_ws2812_set_color(i, r, g, b);
    }
}

void clear_rgb(void) {
    rgb_matrix_set_color_all(RGB_OFF);
    rgb_matrix_update_pwm_buffers();
}

/**
 * @brief  refresh side leds.
 */
void side_rgb_refresh(void) {
    if (!is_side_rgb_off()) {
        side_led_last_act = 0;
        pwr_side_led_on(); // power on side LED before refresh
    }
    if (!flush_side_leds) return;
    side_led_last_act = 0;
    side_ws2812_setleds(side_leds, SIDE_LED_NUM);
    flush_side_leds = false;
}

// TODO
void side_ws2812_refresh(void) {
    side_rgb_refresh();
}

// TODO: Do we need  this? Current caller is commented out.
void signal_sleep(void) {
    uint8_t r, g, b;
    r = 0x00; g = 0x00; b = 0x80;
    if (dev_info.link_mode == LINK_RF_24) {
        g = 0x80;
        b = 0x00;
    }
    pwr_side_led_on();
    wait_ms(10);
    side_ws2812_set_color_strip(LEFT_SIDE + RIGHT_SIDE, r, g, b);
    side_ws2812_refresh();
    wait_ms(140);
}

/**
 * @brief  Adjusting the brightness of side lights.
 * @param  bright: 0 - decrease, 1 - increase.
 * @note  save to eeprom.
 */
void side_light_control(uint8_t dir) {
    if (dir) {
        if (kb_config.side_light == SIDE_BRIGHT_MAX) {
            return;
        }
        kb_config.side_light++;
    } else {
        if (kb_config.side_light == 0) {
            return;
        }
        kb_config.side_light--;
    }
    save_config_to_eeprom();

    #ifndef NO_DEBUG
        dprintf("side matrix light_control [NOEEPROM]: %d\n", kb_config.side_light);
    #endif
}

/**
 * @brief  Adjusting the speed of side lights.
 * @param  fast: 0 - decrease, 1 - increase.
 * @note  save to eeprom.
 */
void side_speed_control(uint8_t dir) {
    if (dir) {
        if (kb_config.side_speed == 0) {
            return;
        }
        kb_config.side_speed--;
    } else {
        if (kb_config.side_speed == SIDE_SPEED_MAX) {
            return;
        }
        kb_config.side_speed++;
    }
    save_config_to_eeprom();
#ifndef NO_DEBUG
    dprintf("side matrix speed_control [NOEEPROM]: %d\n", kb_config.side_speed);
#endif
}

/**
 * @brief  Switch to the next color of side lights.
 * @param  color: 0 - prev, 1 - next.
 * @note  save to eeprom.
 */
void side_colour_control(uint8_t dir) {
    if (kb_config.side_mode != SIDE_WAVE) {
        if (kb_config.side_rgb) {
            kb_config.side_rgb    = 0;
            kb_config.side_colour = 0;
        }
    }
    if (dir) {
        if (kb_config.side_rgb) {
            kb_config.side_rgb    = 0;
            kb_config.side_colour = game_mode_enable; // 0
        } else {
            kb_config.side_colour++;
            if (kb_config.side_colour >= SIDE_COLOUR_MAX) {
                kb_config.side_rgb    = 1;
                kb_config.side_colour = game_mode_enable; // 0
            }
        }
    } else {
        if (kb_config.side_rgb) {
            kb_config.side_rgb    = 0;
            kb_config.side_colour = SIDE_COLOUR_MAX - 1;
        } else {
            kb_config.side_colour--;
            if (kb_config.side_colour >= SIDE_COLOUR_MAX) {
                kb_config.side_rgb    = 1;
                kb_config.side_colour = game_mode_enable; // 0
            }
        }
    }
    save_config_to_eeprom();
#ifndef NO_DEBUG
    dprintf("side matrix colour_control [NOEEPROM]: %d rgb: %d\n", kb_config.side_colour, kb_config.side_rgb);
#endif
}

/**
 * @brief  Change the color mode of side lights.
 * @param  dir: 0 - prev, 1 - next.
 * @note  save to eeprom.
 */
void side_mode_control(uint8_t dir) {
    if (dir) {
        kb_config.side_mode++;
        if (kb_config.side_mode > SIDE_OFF) {
            kb_config.side_mode = 0;
        }
    } else {
        if (kb_config.side_mode > 0) {
            kb_config.side_mode--;
        } else {
            kb_config.side_mode = SIDE_OFF;
        }
    }
    side_play_point = 0;
    save_config_to_eeprom();
#ifndef NO_DEBUG
    dprintf("side matrix mode_control [NOEEPROM]: %d\n", kb_config.side_mode);
#endif
}

/**
 * @brief  side_rgb_brightness.
 * @param light_temp:
 */
void side_rgb_brightness(uint8_t light_temp) {
    uint16_t temp;

    temp   = (light_temp)*r_temp + r_temp;
    r_temp = temp >> 8;

    temp   = (light_temp)*g_temp + g_temp;
    g_temp = temp >> 8;

    temp   = (light_temp)*b_temp + b_temp;
    b_temp = temp >> 8;

}

// TODO
/*void set_sys_light(void) {
    sys_light = kb_config.side_light > 5 ? 1 : (3 - kb_config.side_light / 2);
    current_rgb.r = current_rgb.r / sys_light;
    current_rgb.g = current_rgb.g / sys_light;
    current_rgb.b = current_rgb.b / sys_light;
}*/

/**
 * @brief  set left side leds.
 * @param  ...
 */
void set_left_rgb(uint8_t r, uint8_t g, uint8_t b, bool system) {
    uint8_t bright = kb_config.side_light;
    if (bright == 0) {
        if (system) {
            bright = 1;
        } else {
            return;
        }
    }
    r_temp = r;
    g_temp = g;
    b_temp = b;
    side_rgb_brightness(side_light_tab[bright]);

    side_rgb_set_color_left(r_temp, g_temp, b_temp);
}

/**
 * @brief  set right side leds.
 * @param  ...
 */
void set_right_rgb(uint8_t r, uint8_t g, uint8_t b, bool system) {
    uint8_t bright = kb_config.side_light;
    if (bright == 0) {
        if (system) {
            bright = 1;
        } else {
            return;
        }
    }
    r_temp = r;
    g_temp = g;
    b_temp = b;
    side_rgb_brightness(side_light_tab[bright]);

    side_rgb_set_color_right(r_temp, g_temp, b_temp);
}

/**
 * @brief Determine to trend up or down the breath tab table.
 */
bool breath_tab_trend(bool trend, uint8_t playpoint) {
    if (playpoint == 0) {
        return 1;
    }
    if (playpoint == BREATHE_TAB_LEN - 1) {
        return 0;
    }
    return trend;
}

/**
 * @brief  set left side leds.
 */
void sys_sw_led_show(void) {
    static uint32_t sys_show_timer = 0;
    static bool     sys_show_flag  = false;

    if (f_sys_show) {
        f_sys_show     = false;
        sys_show_timer = timer_read32();
        sys_show_flag = true;
    }

    if (sys_show_flag) {
        if (dev_info.sys_sw_state == SYS_SW_MAC) {
            r_temp = 0x80;
            g_temp = 0x80;
            b_temp = 0x80;
        } else {
            r_temp = 0x00;
            g_temp = 0x00;
            b_temp = 0x80;
        }
        if (timer_elapsed32(sys_show_timer) >= (3000 - 50)) {
#if (WORK_MODE == USB_MODE)
            if (timer_elapsed32(sys_show_timer) <= 4000)
                SET_SYS_SIDE_RGB(r_temp, g_temp, b_temp, true);
            else
                sys_show_flag = false;
#else
            sys_show_flag = false;
#endif
            return;
        }
        if ((timer_elapsed32(sys_show_timer) / 500) % 2 == 0) {
            SET_SYS_SIDE_RGB(r_temp, g_temp, b_temp, true);
        } else {
            SET_SYS_SIDE_RGB(0x00, 0x00, 0x00, false);
        }
    }
}

/**
 * @brief  sleep_sw_led_show.
 */
void sleep_sw_led_show(void) {
    static uint32_t sleep_show_timer     = 0;
    static bool     sleep_show_flag      = false;

    if (f_sleep_show) {
        f_sleep_show     = false;
        sleep_show_timer = timer_read32();
        sleep_show_flag  = true;
    }

    if (sleep_show_flag) {
        r_temp = 0x00;
        g_temp = 0x00;
        b_temp = 0x00;
        switch (kb_config.sleep_mode) {
            case SLEEP_MODE_OFF:
                r_temp = 0x80;
                break;
            case SLEEP_MODE_LIGHT:
                r_temp = 0x80;
                g_temp = 0x40;
                break;
            case SLEEP_MODE_DEEP:
                g_temp = 0x80;
                break;
        }

        if (kb_config.usb_sleep_toggle) {
            b_temp = 0x40;
        }
        
        if (timer_elapsed32(sleep_show_timer) >= (3000 - 50)) {
            sleep_show_flag = false;
            return;
        }
        if ((timer_elapsed32(sleep_show_timer) / 500) % 2 == 0) {
            SET_BAT_SIDE_RGB(r_temp, g_temp, b_temp, true);
        } else {
            SET_BAT_SIDE_RGB(0x00, 0x00, 0x00, false);
        }
    }
}

/**
 * @brief  sys_led_show.
 */
void sys_led_show(void) {
    // TODO: debug rf_led to know how to detect num_lock
    uint8_t caps_key_led_idx = get_led_index(3, 0);
    bool    showCapsLock     = false;

    // ryodeushii
    /*
    if (dev_info.link_mode == LINK_USB) {
        showCapsLock = host_keyboard_led_state().caps_lock;
    } else {
        showCapsLock = dev_info.rf_led & 0x02;
    }*/

    // JinCao seems to have removed the USB condition (above), which was in the original.
    // ryodeushii added the else.

    showCapsLock = host_keyboard_led_state().caps_lock;
    if (showCapsLock) {
        switch (kb_config.caps_indicator_type) {
            case CAPS_INDICATOR_SIDE:
                set_left_rgb(0x00, 0x80, 0x80, true); // highlight top-left side led to indicate caps lock enabled state
                break;
            case CAPS_INDICATOR_UNDER_KEY:
                user_set_rgb_color(caps_key_led_idx, 0, 0x80, 0x80); // 63 is CAPS_LOCK position
                break;
            case CAPS_INDICATOR_BOTH:
                set_left_rgb(0x00, 0x80, 0x80, true);                   // highlight top-left side led to indicate caps lock enabled state
                user_set_rgb_color(caps_key_led_idx, 0, 0x80, 0x80); // 63 is CAPS_LOCK position
                break;
            case CAPS_INDICATOR_OFF:
            default:
                break;
        }
    }

    // TODO
    /*if (kb_config.numlock_state == 1 && host_keyboard_led_state().num_lock) {
        current_rgb.r = 0x80, current_rgb.g = 0x80, current_rgb.b = 0x80;
        set_sys_light();
        side_ws2812_set_color_strip(led_side, current_rgb.r, current_rgb.g, current_rgb.b);
    }*/
}

/**
 * @brief  light_point_playing.
 * @param trend:
 * @param step:
 * @param len:
 * @param point:
 */
void light_point_playing(uint8_t trend, uint8_t step, uint8_t len, uint8_t *point) {
    if (trend) {
        *point += step;
        if (*point > 254 && len == 0) { *point = 0; }
        else if (*point >= len) {
            *point -= len;
        }
    } else {
        *point -= step;
        if (*point < 1 && len == 0) { *point = 255; }
        else if (*point >= len) {
            *point = len - (255 - *point) - 1;
        }
    }
}

/**
 * @brief  side_wave_mode_show.
 */
static void side_wave_mode_show(void) {
    uint8_t play_index;
    if (kb_config.side_rgb) {
        // TODO: Original is 3, ryodeushii set to 1, adi 6
        light_point_playing(0, 1, FLOW_COLOUR_TAB_LEN, &side_play_point);
    } else {
        light_point_playing(0, 2, WAVE_TAB_LEN, &side_play_point);
    }

    play_index = side_play_point;
    for (int i = 0; i < SIDE_LINE; i++) {
        if (kb_config.side_rgb) {
            r_temp = flow_rainbow_colour_tab[play_index][0];
            g_temp = flow_rainbow_colour_tab[play_index][1];
            b_temp = flow_rainbow_colour_tab[play_index][2];

            // Original uses 24, ryodeushii 8, adi 32
            light_point_playing(1, 24, FLOW_COLOUR_TAB_LEN, &play_index);
        } else {
            r_temp = colour_lib[kb_config.side_colour][0];
            g_temp = colour_lib[kb_config.side_colour][1];
            b_temp = colour_lib[kb_config.side_colour][2];

            light_point_playing(1, 12, WAVE_TAB_LEN, &play_index);
            side_rgb_brightness(wave_data_tab[play_index]);
        }

        side_rgb_brightness(side_light_tab[kb_config.side_light]);

#ifdef SIDE_SEPARATE
        side_rgb_set_color(side_led_index_tab[i][0], r_temp, g_temp, b_temp);
#else
        for (int j = 0; j < 2; j++) {
            side_rgb_set_color(side_led_index_tab[i][j], r_temp, g_temp, b_temp);
        }
#endif
    }
}

/**
 * @brief  side_spectrum_mode_show.
 */
static void side_spectrum_mode_show(void) {
    light_point_playing(1, 1, FLOW_COLOUR_TAB_LEN, &side_play_point);

    r_temp = flow_rainbow_colour_tab[side_play_point][0];
    g_temp = flow_rainbow_colour_tab[side_play_point][1];
    b_temp = flow_rainbow_colour_tab[side_play_point][2];

    side_rgb_brightness(side_light_tab[kb_config.side_light]);

    side_rgb_set_color_all(r_temp, g_temp, b_temp);
}

/**
 * @brief  side_breathe_mode_show.
 */
static void side_breathe_mode_show(void) {
    static uint8_t play_point = 0;
    static bool    trend      = 1;
    light_point_playing(trend, 1, BREATHE_TAB_LEN, &play_point);
    trend = breath_tab_trend(trend, play_point);

    r_temp = colour_lib[kb_config.side_colour][0];
    g_temp = colour_lib[kb_config.side_colour][1];
    b_temp = colour_lib[kb_config.side_colour][2];

    side_rgb_brightness(breathe_data_tab[play_point]);
    side_rgb_brightness(side_light_tab[kb_config.side_light]);

    side_rgb_set_color_all(r_temp, g_temp, b_temp);
}

/**
 * @brief  side_static_mode_show.
 */
static void side_static_mode_show(void) {
    if (side_play_point >= SIDE_COLOUR_MAX) {
        side_play_point = 0;
    }

    r_temp = colour_lib[kb_config.side_colour][0];
    g_temp = colour_lib[kb_config.side_colour][1];
    b_temp = colour_lib[kb_config.side_colour][2];

    side_rgb_brightness(side_light_tab[kb_config.side_light]);

    side_rgb_set_color_all(r_temp, g_temp, b_temp);
}

/**
 * @brief  side_off_mode_show.
 */
static void side_off_mode_show(void) {
    r_temp = 0x00;
    g_temp = 0x00;
    b_temp = 0x00;

    side_rgb_set_color_all(r_temp, g_temp, b_temp);
}

void bat_charging_breathe(void) {
    static uint32_t interval_timer = 0;
    static uint8_t  play_point     = 0;
    static bool trend           = 1;

    if (timer_elapsed32(interval_timer) > 10) {
        interval_timer = timer_read32();
        light_point_playing(trend, 1, BREATHE_TAB_LEN, &play_point);
        trend = breath_tab_trend(trend, play_point);
    }

    r_temp = 0x80;
    g_temp = 0x40;
    b_temp = 0x00;
    side_rgb_brightness(breathe_data_tab[play_point]);
    SET_BAT_SIDE_RGB(r_temp, g_temp, b_temp, true);
}

void bat_charging_design(uint8_t init, uint8_t r, uint8_t g, uint8_t b) {
    static uint32_t interval_timer = 0;
    static uint16_t show_mask      = 0x00;
    static bool     f_move_trend   = 0;
    uint16_t        bit_mask       = 1;
    uint8_t         i;

    if (timer_elapsed32(interval_timer) > 100) {
        interval_timer = timer_read32();

        if (f_move_trend) {
            show_mask >>= 1;
            if (show_mask == 0x1f >> (SIDE_LINE - init)) f_move_trend = 0;
        } else {
            show_mask <<= 1;
            show_mask |= 1;
            if (show_mask == 0x7f) f_move_trend = 1;
        }

        // TODO: adi
        // side_ws2812_set_color_strip(my_side, RGB_OFF);
    }

    for (i = 0; i < SIDE_LINE; i++) {
        if (show_mask & bit_mask) {
            side_rgb_set_color(BAT_SIDE_TOP(i), r, g, b);
        } else {
            side_rgb_set_color(BAT_SIDE_TOP(i), 0x00, 0x00, 0x00);
        }
        bit_mask <<= 1;
    }
}

void rf_show_blink(void) {
    extern uint8_t  rf_blink_cnt;
    static uint32_t interval_timer = 0;
    uint16_t        show_period;

    if (rf_blink_cnt) {
        if (dev_info.rf_state == RF_PAIRING)
            show_period = RF_LED_PAIR_PERIOD;

        else
            show_period = RF_LED_LINK_PERIOD;

        if (timer_elapsed32(interval_timer) > (show_period >> 1)) {
            r_temp = 0x00;
            g_temp = 0x00;
            b_temp = 0x00;
        }

        if (timer_elapsed32(interval_timer) >= show_period) {
            rf_blink_cnt--;
            interval_timer = timer_read32();
        }
    } else {
        interval_timer = timer_read32();
    }

    SET_RF_SIDE_RGB(r_temp, g_temp, b_temp, true);
}

void rf_show_design(uint8_t r, uint8_t g, uint8_t b) {
    static uint32_t interval_timer = 0;
    static uint16_t show_mask      = 0x04;
    uint16_t        show_mask_temp = 0;
    uint16_t        show_priod;

    uint16_t bit_mask = 1;

    if (dev_info.rf_state == RF_PAIRING)
        show_priod = 100;
    else
        show_priod = 200;

    if (timer_elapsed32(interval_timer) > show_priod) {
        interval_timer = timer_read32();

        show_mask_temp = (show_mask << 1) | (show_mask >> 1);
        show_mask_temp |= 0x04;
        show_mask |= show_mask_temp;

        if (show_mask == 0x7f) show_mask = 0x0;
    }

    for (uint8_t i = 0; i < SIDE_LINE; i++) {
        if (show_mask & bit_mask) {
            side_rgb_set_color(RF_SIDE_TOP(i), r, g, b);
        } else {
            side_rgb_set_color(RF_SIDE_TOP(i), 0x00, 0x00, 0x00);
        }
        bit_mask <<= 1;
    }
}

/**
 * @brief  rf_led_show.
 */
void rf_led_show(void) {
#if (WORK_MODE == THREE_MODE)
    static bool flag_power_on = 1;
#endif

    if (dev_info.link_mode == LINK_RF_24) {
        r_temp = 0x00;
        g_temp = 0x80;
        b_temp = 0x00;
    } else if (dev_info.link_mode == LINK_USB) {
        r_temp = 0x80;
        g_temp = 0x80;
        b_temp = 0x00;
#if (WORK_MODE == THREE_MODE)
        if (flag_power_on && (rf_link_show_time < RF_LINK_SHOW_TIME)) return;
#endif
    } else {
        r_temp = 0x00;
        g_temp = 0x00;
        b_temp = 0x80;
    }

#if (WORK_MODE == THREE_MODE)
    flag_power_on = 0;
#endif
    if (rf_blink_cnt) {
#if (RFLINK_SHIFT)
        rf_show_design(r_temp, g_temp, b_temp);
#else
        rf_show_blink();
#endif
    } else if (rf_link_show_time < RF_LINK_SHOW_TIME) {
        SET_RF_SIDE_RGB(r_temp, g_temp, b_temp, true);
    }

    // light up corresponding BT/RF key
    /*if (dev_info.link_mode <= LINK_BT_3) {
        uint8_t my_pos = dev_info.link_mode == LINK_RF_24 ? 4 : dev_info.link_mode;
        if (rf_link_show_time > RF_LINK_SHOW_TIME - 10) {
            rgb_matrix_set_color(led_idx.KC_GRV - my_pos, RGB_OFF);
        } else {
            uint8_t my_pos = dev_info.link_mode == LINK_RF_24 ? 4 : dev_info.link_mode;
            rgb_required = 1;
            rgb_matrix_set_color(led_idx.KC_GRV - my_pos, current_rgb.r, current_rgb.g, current_rgb.b);
        }
    }*/
}

void low_bat_show(void) {
    static uint32_t interval_timer = 0;

    r_temp = 0x80, g_temp = 0, b_temp = 0;

    if (low_bat_blink_cnt) {
        if (timer_elapsed32(interval_timer) > (LOW_BAT_BLINK_PERIOD >> 1)) {
            r_temp = 0x00;
            g_temp = 0x00;
            b_temp = 0x00;
        }

        if (timer_elapsed32(interval_timer) >= LOW_BAT_BLINK_PERIOD) {
            interval_timer = timer_read32();
            low_bat_blink_cnt--;
        }
    }

    SET_BAT_SIDE_RGB(r_temp, g_temp, b_temp, true);
}

/**
 * @brief  bat_num_led.
 */
void bat_num_led(uint8_t bat_percent) {
    uint8_t r, g, b;
    uint8_t bat_idx = bat_percent / 17;
    uint8_t bat_lvl = 1 + ((bat_percent - 1) / 10);

    r = bat_percent_tab[bat_idx][0];
    g = bat_percent_tab[bat_idx][1];
    b = bat_percent_tab[bat_idx][2];

    // set percent
    for (uint8_t i = 0; i < bat_lvl; i++) {
        rgb_matrix_set_color(29 - i, r, g, b);
    }
}

// Called by rgb_matrix_indicators_kb in original only.
void num_led_show(void) {
    static uint8_t num_bat_temp = 0;
    num_bat_temp                = dev_info.rf_battery;
    bat_num_led(num_bat_temp);
}

/**
 * @brief  bat_percent_led.
 */
void bat_percent_led(uint8_t bat_percent) {
    uint8_t bat_idx = bat_percent / 17;

#ifndef BATTERY_BRIGHTNESS_INDEPENDENT
    // Original
    uint8_t bright  = kb_config.side_light;
    if (bright < 1) {
        bright = 1;
    }
#endif

    r_temp = bat_percent_tab[bat_idx][0];
    g_temp = bat_percent_tab[bat_idx][1];
    b_temp = bat_percent_tab[bat_idx][2];
#ifndef BATTERY_BRIGHTNESS_INDEPENDENT
    // Original
    side_rgb_brightness(side_light_tab[bright]);
#endif

    // ryodeushii added battery_indicator_brightness as an independent setting, but I am
    // not a fan of how bright it gets.
    // I uncommented the original behavior, making the below a percentage of the set brightness.
    // NOTE: dim using kb_config.battery_indicator_brightness as percentage value
    r_temp = r_temp * kb_config.battery_indicator_brightness / 100;
    g_temp = g_temp * kb_config.battery_indicator_brightness / 100;
    b_temp = b_temp * kb_config.battery_indicator_brightness / 100;

    if (f_charging) {
        low_bat_blink_cnt = 6;
#if (CHARGING_SHIFT)
        bat_charging_design(bat_idx, r_temp, g_temp, b_temp);
#else
        bat_charging_breathe();
#endif
    }

    else if (bat_percent < 10) {
        low_bat_show();
    } else {
        low_bat_blink_cnt = 6;

        // TODO: Older version keeps the >> 2, not sure why. Could be my mistake.

        uint8_t i = 0;
        for (; i <= bat_idx; i++)
            side_rgb_set_color(BAT_SIDE_TOP(i), r_temp >> 2, g_temp >> 2, b_temp >> 2);

        for (; i < SIDE_LINE; i++)
            side_rgb_set_color(BAT_SIDE_TOP(i), 0, 0, 0);
    }
}


/**
 * @brief  bat_led_show.
 */
void bat_led_show(void) {
    static bool bat_show_flag = true;

    static uint32_t bat_show_time    = 0;
    static uint32_t bat_sts_debounce = 0;
    static uint8_t  charge_state     = 0;
    static bool     f_init           = 1;

    if (dev_info.link_mode != LINK_USB) {
        extern uint16_t rf_link_show_time;
        if (rf_link_show_time < RF_LINK_SHOW_TIME) return;

        if (dev_info.rf_state != RF_CONNECT) return;
    }

    if (f_init) {
        f_init        = 0;
        bat_show_time = timer_read32();
        charge_state  = dev_info.rf_charge;
    }

    if (game_mode_enable) {
        if(dev_info.rf_battery < low_bat_level) {
            side_ws2812_set_color_strip(BAT_SIDE, 0x40, 0x00, 0x00);
        }
        return;
    }

    if (charge_state != dev_info.rf_charge) {
        if (timer_elapsed32(bat_sts_debounce) > 1000) {
            if (((charge_state & 0x01) == 0) && ((dev_info.rf_charge & 0x01) != 0)) {
                bat_show_flag = true;
                f_charging    = true;
                bat_show_time = timer_read32();
            }
            charge_state = dev_info.rf_charge;
        }
    } else {
        bat_sts_debounce = timer_read32();
        if (timer_elapsed32(bat_show_time) > 5000) {
            bat_show_flag = false;

            f_charging = false;
        }

        if (charge_state == 0x03) { // charging, not full?
            f_charging = true;
        } else if (!(charge_state & 0x01)) {
            f_charging = 0;
        }
    }

    uint8_t bat_percent = dev_info.rf_battery;

    if ((bat_percent < low_bat_level) && (!(charge_state & 0x01))) {
        bat_show_flag = true;
        bat_show_time = timer_read32();

        if (rgb_matrix_config.hsv.v > RGB_MATRIX_VAL_STEP) {
            rgb_matrix_config.hsv.v = RGB_MATRIX_VAL_STEP;
        }

        if (kb_config.side_light > 1) {
            kb_config.side_light = 1;
        }

#ifdef SIDE_SEPARATE
        if (kb_config.right_side_light > 1) {
            kb_config.right_side_light = 1;
        }
#endif
    }


    if (f_bat_hold || bat_show_flag) {
        bat_percent_led(bat_percent);
    }
}


/**
 * @brief  device_reset_show.
 */
void device_reset_show(void) {
    pwr_rgb_led_on();
    pwr_side_led_on();

    for (int blink_cnt = 0; blink_cnt < 3; blink_cnt++) {
        rgb_matrix_set_color_all(0x10, 0x10, 0x10);
        // ryodeushii does this one and below in a loop, but then
        // we lose the configured brightness
        /*for (int i = 0; i < SIDE_LED_NUM; i++) {
            // Original: 0x40 for these
            side_rgb_set_color(i, 0x10, 0x10, 0x10);
        }*/
        set_left_rgb(0x10, 0x10, 0x10, true);
        set_right_rgb(0x10, 0x10, 0x10, true);
        rgb_matrix_update_pwm_buffers();

        side_rgb_refresh();
        wait_ms(200);

        rgb_matrix_set_color_all(0x00, 0x00, 0x00);
        /*for (int i = 0; i < SIDE_LED_NUM; i++) {
            side_rgb_set_color(i, 0x00, 0x00, 0x00);
        }*/
        set_left_rgb(0x00, 0x00, 0x00, true);
        set_right_rgb(0x00, 0x00, 0x00, true);
        rgb_matrix_update_pwm_buffers();
        side_rgb_refresh();
        wait_ms(200);
    }
}


/**
 * @brief  device_reset_init.
 */
void device_reset_init(void) {
    game_mode_enable = 0;
    side_play_point = 0;
#ifdef SIDE_SEPARATE
    right_side_play_point = 0;
#endif

    // TODO
    rgb_matrix_enable_noeeprom();

    f_bat_hold      = false;

    kb_config_reset();
}


/**
 *      RGB test
 */
void rgb_test_show(void) {
    // open power control
    pwr_rgb_led_on();
    pwr_side_led_on();

    // clang-format off
    uint8_t colours[7][3] = {
        { 0xFF, 0x00, 0x00 },
        { 0x00, 0xFF, 0x00 },
        { 0x00, 0x00, 0xFF },
        { 0x80, 0x80, 0x80 },
        { 0x80, 0x80, 0x00 },
        { 0x80, 0x00, 0x80 },
        { 0x00, 0x80, 0x80 }
    };
    // clang-format on
    for (uint8_t i = 0; i < 7; i++) {
        uint8_t r = colours[i][0];
        uint8_t g = colours[i][1];
        uint8_t b = colours[i][2];

        rgb_matrix_set_color_all(r, g, b);
        rgb_matrix_update_pwm_buffers();
        set_left_rgb(r, g, b, true);
        set_right_rgb(r, g, b, true);
        side_rgb_refresh();
        wait_ms(500);
    }
}

void signal_rgb_led(uint8_t selected_color, uint8_t start_led, uint8_t end_led, uint16_t show_time) {
    rgb_color           = selected_color;
    rgb_start_led       = start_led;
    rgb_end_led         = end_led > RGB_MATRIX_LED_COUNT ? start_led : end_led;
    rgb_show_time       = show_time;
    rgb_indicator_timer = timer_read32();
}

void rgb_led_indicator(void) {
    if (rgb_show_time == 0) { return; }
    if (timer_elapsed32(rgb_indicator_timer) < rgb_show_time || rgb_show_time == UINT16_MAX) {
        current_rgb.r = colour_lib[rgb_color][0];
        current_rgb.g = colour_lib[rgb_color][1];
        current_rgb.b = colour_lib[rgb_color][2];

        rgb_required  = 2;
        for (uint8_t i = rgb_start_led; i <= rgb_end_led; i++) {
            rgb_matrix_set_color(i, current_rgb.r, current_rgb.g, current_rgb.b);
        }
    } else {
        for (uint8_t i = rgb_start_led; i <= rgb_end_led; i++) {
            rgb_matrix_set_color(i, RGB_OFF);
        }
        rgb_show_time       = 0;
        rgb_indicator_timer = 0;
    }
}


void caps_word_show(void) {
    static bool caps_word_rgb_on = 0;
    if (!is_caps_word_on() || game_mode_enable || !kb_config.caps_word_enable) {
        if (caps_word_rgb_on) {
            caps_word_rgb_on = 0;
            rgb_matrix_set_color(led_idx.KC_CAPS, RGB_OFF);
        }
        return;
    } else {
        rgb_required     = 2;
        caps_word_rgb_on = 1;
        rgb_matrix_set_color(led_idx.KC_CAPS, RGB_CYAN);
    }
}

void numlock_rgb_show(void) {
    static bool num_lock_rgb_on = 0;
    if (!host_keyboard_led_state().num_lock || kb_config.numlock_state != 2) {
        if (num_lock_rgb_on) {
            num_lock_rgb_on = 0;
            rgb_matrix_set_color(led_idx.KC_NUM, RGB_OFF);
        }
        return;
    } else {
        rgb_required    = 2;
        num_lock_rgb_on = 1;
        rgb_matrix_set_color(led_idx.KC_NUM, RGB_WHITE);
    }
}

// TODO: adi, only needed if game mode has separate rgb settings
/*void rgb_matrix_step_game_mode(uint8_t step) {
    if (step) {
        kb_config.game_rgb_mod++;
        if (kb_config.game_rgb_mod > RGB_MATRIX_CUSTOM_GAME_KEYS) { kb_config.game_rgb_mod = 1; }
        else if (kb_config.game_rgb_mod > 3) { kb_config.game_rgb_mod = RGB_MATRIX_CUSTOM_GAME_KEYS; }
    } else {
        kb_config.game_rgb_mod--;
        if (kb_config.game_rgb_mod > 3) { kb_config.game_rgb_mod = 3; }
        else if (kb_config.game_rgb_mod < 1) { kb_config.game_rgb_mod = RGB_MATRIX_CUSTOM_GAME_KEYS; }
    }

    rgb_matrix_mode_noeeprom(kb_config.game_rgb_mod);
}*/

/**
 * @brief  side_led_show.
 */
void side_led_show(void) {
    static uint32_t side_update_time  = 0;

    // side_mode & side_speed should always be valid...
    // refresh side LED animation based on speed.
    uint8_t update_interval = game_mode_enable ? 500 : side_speed_tab[kb_config.side_mode][kb_config.side_speed];
    if (timer_elapsed32(side_update_time) >= update_interval) {
        side_update_time = timer_read32();
        do_refresh       = true;
        switch (kb_config.side_mode) {
            case SIDE_WAVE:
                side_wave_mode_show();
                break;
            case SIDE_MIX:
                side_spectrum_mode_show();
                break;
            case SIDE_BREATH:
                side_breathe_mode_show();
                break;
            case SIDE_STATIC:
                side_static_mode_show();
                break;
            case SIDE_OFF:
                side_off_mode_show();
                break;
        }
    }

#ifdef SIDE_SEPARATE
    right_side_led_loop();
#endif
}

void realtime_led_process(void) {
    rgb_led_indicator();
    caps_word_show();
    numlock_rgb_show();
#if (WORK_MODE == THREE_MODE)
    bat_led_show();
    rf_led_show();
#endif
    sys_led_show();
    sys_sw_led_show();
    sleep_sw_led_show();
}

void led_show(void) {
    static uint32_t side_refresh_time = 0;
    static bool     flag_power_on     = 1;

    if (f_wakeup_prepare) { return; }

    if (flag_power_on) {
        if (!f_dial_sw_init_ok) return;
        flag_power_on = 0;
    }

    side_led_show();
    realtime_led_process();

    // Original is 30, JinCao 10, ryodeushii 50.
    // This only refreshes if LEDs change anyways. Fixes side LED not refreshing synchronously.
    if (do_refresh || timer_elapsed32(side_refresh_time) >= 10) {
        side_refresh_time = timer_read32();
        do_refresh        = false;
        side_rgb_refresh();
    }
}
