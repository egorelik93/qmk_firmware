
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

#pragma once

#include <stdbool.h>
#include <stdint.h>

// clang-format off
#define SIDE_BRIGHT_MAX 5
#define SIDE_SPEED_MAX 4
#define SIDE_COLOUR_MAX 8
#define RIGHT_SIDE_LINE 6

#define SIDE_LINE 6
#define SIDE_LED_NUM (RIGHT_SIDE_LINE + SIDE_LINE)
// #define SIDE_LED_NUM 12

#define SIDE_LEFT 0
#define SIDE_RIGHT SIDE_LINE

// clang-format on

/* side rgb mode */
enum {
    SIDE_WAVE = 0,
    SIDE_MIX,
    SIDE_STATIC,
    SIDE_BREATH,
    SIDE_OFF,
} side_effects;

// from side_right.c
#define STARRY_INDEX_LEN (160)
#define FIREWORK_INDEX_LEN (158)
#define STARRY_DATA_LEN 96
#define TIDE_DATA_LEN 120

// from side.c
#define RF_LED_LINK_PERIOD 500
#define RF_LED_PAIR_PERIOD 250
#define CHARGING_SHIFT 1
#define RFLINK_SHIFT 0
#define CHARGING_BREATHE 1

#define RFLINK_BLINK 1
#define LOW_BAT_BLINK_PERIOD 500

bool breath_tab_trend(bool trend, uint8_t playpoint);

void side_rgb_set_color_left(uint8_t r, uint8_t g, uint8_t b);
void side_rgb_set_color_right(uint8_t r, uint8_t g, uint8_t b);
