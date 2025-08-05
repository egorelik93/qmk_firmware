/*
Copyright 2023 @ Nuphy <https://nuphy.com/> & @jincao1

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

#include "side.h"

// data tables lengths
#define LIGHT_TAB_LEN 101
#define WAVE_TAB_LEN 112
#define BREATHE_TAB_LEN 64
#define FLOW_COLOUR_TAB_LEN 224

extern const uint8_t breathe_data_tab[BREATHE_TAB_LEN];
extern const uint8_t wave_data_tab[WAVE_TAB_LEN];
extern const uint8_t flow_rainbow_colour_tab[FLOW_COLOUR_TAB_LEN][3];
extern const uint8_t colour_lib[9][3];
extern const uint8_t side_speed_tab[5][5];
extern const uint8_t side_light_tab[6];
extern const uint8_t side_led_index_tab[SIDE_LINE][2];
extern const uint8_t bat_percent_tab[6][3];
