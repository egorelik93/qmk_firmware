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

#include "color.h"
#include "quantum_keycodes.h"

enum custom_keycodes {
    RF_DFU = QK_KB_0,
    LNK_USB,
    LNK_RF,
    LNK_BLE1,
    LNK_BLE2,
    LNK_BLE3,

    MAC_TASK,   // KC_CALC
    MAC_SEARCH, // KC_LCTL + KC_F
    MAC_VOICE,  // F5
    MAC_DND,
    MAC_CONSOLE,
    MAC_PRT,    // PrintScreen
    MAC_PRTA,   // PrintScreen

    SIDE_VAI,
    SIDE_VAD,
    SIDE_MOD,
    SIDE_HUI,
    SIDE_SPI,
    SIDE_SPD,

    DEV_RESET,
    SLEEP_MODE,
    BAT_SHOW,
    BAT_NUM,
    RGB_TEST,
    LINK_TO,
    KB_SLP,

    WIN_LOCK,
    SHIFT_GRV,

    RIGHT_SIDE_VAI,
    RIGHT_SIDE_VAD,
    RIGHT_SIDE_MOD,
    RIGHT_SIDE_HUI,
    RIGHT_SIDE_SPI,
    RIGHT_SIDE_SPD,
    TOG_USB_SLP,
    TOG_CAPS_IND,
    DEBOUNCE_PRESS_INC,
    DEBOUNCE_PRESS_DEC,
    DEBOUNCE_PRESS_SHOW,
    SLEEP_TIMEOUT_INC,
    SLEEP_TIMEOUT_DEC,
    SLEEP_TIMEOUT_SHOW,
    MAC_GLOBE,
    DEBOUNCE_RELEASE_INC,
    DEBOUNCE_RELEASE_DEC,
    DEBOUNCE_RELEASE_SHOW,
    TOG_BAT_IND_NUM,
    SOCDON,
    SOCDOFF,
    SOCDTOG,
    FW_VERSION,

    NUMLOCK_INS,
    NUMLOCK_IND,
    CAPS_WORD,
    SLEEP_NOW,
    //SLEEP_D,
    //SLEEP_I,

    DEBOUNCE_T,

    GAME_MODE,
};

#define SYS_PRT G(S(KC_3))
#define MAC_PRTA G(S(KC_4))
#define WIN_PRTA G(S(KC_S))
#define MAC_SEARCH G(KC_SPC)
#define MAC_LOCK G(C(KC_Q))
