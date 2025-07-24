// Copyright 2023 Persama (@Persama)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "ansi.h"
#include "rgb_matrix.h"
#include "side.h"
#include "side_table.h"
#include "timer.h"
#include "kb_util.h"

extern uint8_t       r_temp, g_temp, b_temp;
extern void          side_rgb_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
extern void          side_rgb_set_color_right(uint8_t red, uint8_t green, uint8_t blue);
extern void          light_point_playing(uint8_t trend, uint8_t step, uint8_t len, uint8_t *point);
extern void          side_rgb_brightness(uint8_t light_temp);


uint8_t  right_side_play_point = 0;

void right_side_light_control(uint8_t brighten) {
    if (brighten) {
        if (kb_config.right_side_light == SIDE_BRIGHT_MAX) {
            return;
        }
        kb_config.right_side_light++;
    } else {
        if (kb_config.right_side_light == 0) {
            return;
        }
        kb_config.right_side_light--;
    }
    save_config_to_eeprom();
}

void right_side_speed_control(uint8_t fast) {
    if (fast) {
        if (kb_config.right_side_speed == 0) {
            return;
        }
        kb_config.right_side_speed--;
    } else {
        if (kb_config.right_side_speed == SIDE_SPEED_MAX) {
            return;
        }
        kb_config.right_side_speed++;
    }
    save_config_to_eeprom();
}

void right_side_colour_control(uint8_t dir) {
    if (kb_config.right_side_mode != SIDE_WAVE) {
        if (kb_config.right_side_rgb) {
            kb_config.right_side_rgb   = 0;
            kb_config.right_side_colour = 0;
        }
    }

    if (dir) {
        if (kb_config.right_side_rgb) {
            kb_config.right_side_rgb   = 0;
            kb_config.right_side_colour = 0;
        } else {
            kb_config.right_side_colour++;
            if (kb_config.right_side_colour >= SIDE_COLOUR_MAX) {
                kb_config.right_side_rgb   = 1;
                kb_config.right_side_colour = 0;
            }
        }
    } else {
        if (kb_config.right_side_rgb) {
            kb_config.right_side_rgb   = 0;
            kb_config.right_side_colour = SIDE_COLOUR_MAX - 1;
        } else {
            kb_config.right_side_colour--;
            if (kb_config.right_side_colour > SIDE_COLOUR_MAX) {
                kb_config.right_side_rgb   = 1;
                kb_config.right_side_colour = 0;
            }
        }
    }
    save_config_to_eeprom();
}

void right_side_mode_control(uint8_t dir) {
    if (dir) {
        kb_config.right_side_mode++;
        if (kb_config.right_side_mode > SIDE_OFF) {
            kb_config.right_side_mode = 0;
        }
    } else {
        if (kb_config.right_side_mode > 0) {
            kb_config.right_side_mode--;
        } else {
            kb_config.right_side_mode = SIDE_OFF;
        }
    }
    right_side_play_point = 0;
    save_config_to_eeprom();
}

static void right_side_wave_mode_show(void) {
    uint8_t play_index;
    if (kb_config.right_side_rgb) {
        // TODO: Original is 3, ryodeushii set to 1
        light_point_playing(0, 1, FLOW_COLOUR_TAB_LEN, &right_side_play_point);
    } else {
        light_point_playing(0, 2, WAVE_TAB_LEN, &right_side_play_point);
    }

    play_index = right_side_play_point;
    for (int i = 0; i < RIGHT_SIDE_LINE; i++) {
        if (kb_config.right_side_rgb) {
            r_temp = flow_rainbow_colour_tab[play_index][0];
            g_temp = flow_rainbow_colour_tab[play_index][1];
            b_temp = flow_rainbow_colour_tab[play_index][2];

            light_point_playing(1, 5, FLOW_COLOUR_TAB_LEN, &play_index);
        } else {
            r_temp = colour_lib[kb_config.right_side_colour][0];
            g_temp = colour_lib[kb_config.right_side_colour][1];
            b_temp = colour_lib[kb_config.right_side_colour][2];

            light_point_playing(1, 12, WAVE_TAB_LEN, &play_index);
            side_rgb_brightness(wave_data_tab[play_index]);
        }

        side_rgb_brightness(side_light_tab[kb_config.right_side_light]);

        side_rgb_set_color(side_led_index_tab[i][1], r_temp, g_temp, b_temp);
    }
}

static void right_side_spectrum_mode_show(void) {
    light_point_playing(1, 1, FLOW_COLOUR_TAB_LEN, &right_side_play_point);

    r_temp = flow_rainbow_colour_tab[right_side_play_point][0];
    g_temp = flow_rainbow_colour_tab[right_side_play_point][1];
    b_temp = flow_rainbow_colour_tab[right_side_play_point][2];

    side_rgb_brightness(side_light_tab[kb_config.right_side_light]);

    side_rgb_set_color_right(r_temp, g_temp, b_temp);
}

static void right_side_breathe_mode_show(void) {
    static uint8_t play_point = 0;
    static bool    trend      = 1;
    light_point_playing(trend, 1, BREATHE_TAB_LEN, &play_point);
    trend = breath_tab_trend(trend, play_point);
    r_temp = colour_lib[kb_config.right_side_colour][0];
    g_temp = colour_lib[kb_config.right_side_colour][1];
    b_temp = colour_lib[kb_config.right_side_colour][2];

    side_rgb_brightness(breathe_data_tab[play_point]);
    side_rgb_brightness(side_light_tab[kb_config.right_side_light]);

    side_rgb_set_color_right(r_temp, g_temp, b_temp);
}

static void right_side_static_mode_show(void) {
    if (right_side_play_point >= SIDE_COLOUR_MAX) {
        right_side_play_point = 0;
    }

    r_temp = colour_lib[kb_config.right_side_colour][0];
    g_temp = colour_lib[kb_config.right_side_colour][1];
    b_temp = colour_lib[kb_config.right_side_colour][2];

    side_rgb_brightness(side_light_tab[kb_config.right_side_light]);

    side_rgb_set_color_right(r_temp, g_temp, b_temp);
}

static void right_side_off_mode_show(void) {
    r_temp = 0x00;
    g_temp = 0x00;
    b_temp = 0x00;

    side_rgb_set_color_right(r_temp, g_temp, b_temp);
}

void right_side_led_loop(void) {
    static uint32_t right_side_update_time  = 0;
    extern bool do_refresh;

    // right_side_mode & right_side_speed should always be valid...
    // refresh side LED animation based on speed.
    uint8_t update_interval = side_speed_tab[kb_config.right_side_mode][kb_config.right_side_speed];
    if (timer_elapsed32(right_side_update_time) >= update_interval) {
        right_side_update_time = timer_read32();
        do_refresh       = true;
        switch (kb_config.right_side_mode) {
            case SIDE_WAVE:
                right_side_wave_mode_show();
                break;
            case SIDE_MIX:
                right_side_spectrum_mode_show();
                break;
            case SIDE_BREATH:
                right_side_breathe_mode_show();
                break;
            case SIDE_STATIC:
                right_side_static_mode_show();
                break;
            case SIDE_OFF:
                right_side_off_mode_show();
                break;
        }
    }
}
