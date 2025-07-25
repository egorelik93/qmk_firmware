// Copyright 2024  Evgeny Kapusta (@ryodeushii)
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

#include "ansi.h"
#include "keycodes.h"
#include QMK_KEYBOARD_H

#define O_2_VI LT(2, KC_O)
#define EVIL_U2D LT(2, KC_U)
#define EVIL_Y LT(2, KC_Y)

// clang-format off

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// layer gaming - activated by mac switch, since I don't use mac
[0] = LAYOUT_75_ansi(
    KC_ESC,     KC_F1,     KC_F2,       KC_F3,     KC_F4,      KC_F5,       KC_F6,     KC_F7,      KC_F8,       KC_F9,     KC_F10,     KC_F11,     KC_F12,      KC_MUTE,    KC_INS,     KC_DEL,
    KC_GRV,     KC_1,      KC_2,        KC_3,      KC_4,       KC_5,        KC_6,      KC_7,       KC_8,        KC_9,      KC_0,       KC_MINS,    KC_EQL,                  KC_BSPC,    KC_HOME,
    KC_TAB,     KC_Q,      KC_W,        KC_E,      KC_R,       KC_T,        KC_Y,      KC_U,       KC_I,        KC_O,      KC_P,       KC_LBRC,    KC_RBRC,                 KC_BSLS,    KC_PGUP,
  CTL_T(KC_F13),KC_A,      KC_S,        KC_D,      KC_F,       KC_G,        KC_H,      KC_J,       KC_K,        KC_L,      KC_SCLN,    KC_QUOT,                             KC_ENT,     KC_PGDN,
    KC_LSFT,               KC_Z,        KC_X,      KC_C,       KC_V,        KC_B,      KC_N,       KC_M,        KC_COMM,   KC_DOT,     KC_SLSH,                 KC_RSFT,    KC_UP,      KC_END,
    KC_LGUI,    MO(5),      KC_LALT,                                         KC_SPC,                             KC_RALT,   MO(3),      KC_RCTL,                 KC_LEFT,    KC_DOWN,    KC_RGHT),

// layer Mac Fn
[1] = LAYOUT_75_ansi(
    _______,    KC_F1,      KC_F2,      KC_F3,     KC_F4,      KC_F5,       KC_F6,     KC_F7,      KC_F8,       KC_F9,     KC_F10,     KC_F11,     KC_F12,      SYS_PRT,    _______,    _______,
    _______,    LNK_BLE1,   LNK_BLE2,   LNK_BLE3,  LNK_RF,     _______,     _______,   _______,    _______,     _______,   _______,    _______,    _______,                 _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,DEBOUNCE_PRESS_DEC,    DEBOUNCE_PRESS_SHOW,    DEBOUNCE_PRESS_INC,_______,    _______,    DEV_RESET,_______,BAT_SHOW,_______,
TOG_CAPS_IND,    SLEEP_TIMEOUT_DEC,    SLEEP_TIMEOUT_SHOW,    SLEEP_TIMEOUT_INC,_______,    _______,DEBOUNCE_RELEASE_DEC,    DEBOUNCE_RELEASE_SHOW,    DEBOUNCE_RELEASE_INC,_______,    _______,    _______,    _______,    _______,
    _______,                _______,    _______,   _______,    _______,     BAT_NUM,   _______,    MO(4),       RGB_SPD,   RGB_SPI,    _______,                 _______,    _______,    _______,
    _______,    _______,    _______,                                        _______,                            _______,   MO(1),      _______,                 RM_NEXT,    _______,    RM_HUEU),

// layer win
[2] = LAYOUT_75_ansi(
    KC_ESC,     KC_F1,     KC_F2,       KC_F3,     KC_F4,      KC_F5,       KC_F6,     KC_F7,      KC_F8,       KC_F9,     KC_F10,     KC_F11,     KC_F12,      KC_MUTE,    KC_INS,     KC_DEL,
    KC_GRV,     KC_1,      KC_2,        KC_3,      KC_4,       KC_5,        KC_6,      KC_7,       KC_8,        KC_9,      KC_0,       KC_MINS,    KC_EQL,                  KC_BSPC,    KC_HOME,
    KC_TAB,     KC_Q,      KC_W,        KC_E,      KC_R,       KC_T,        EVIL_Y,    EVIL_U2D,   KC_I,        O_2_VI,   KC_P,       KC_LBRC,    KC_RBRC,                 KC_BSLS,    KC_PGUP,
  CTL_T(KC_F13),KC_A,      KC_S,        KC_D,      KC_F,       KC_G,        KC_H,      KC_J,       KC_K,        KC_L,      KC_SCLN,    KC_QUOT,                             KC_ENT,     KC_PGDN,
    KC_LSFT,               KC_Z,        KC_X,      KC_C,       KC_V,        KC_B,      KC_N,       KC_M,        KC_COMM,   KC_DOT,     KC_SLSH,                 KC_RSFT,    KC_UP,      KC_END,
    KC_LGUI,    MO(5),      KC_LALT,                                         KC_SPC,                             KC_RALT,   MO(3),      KC_RCTL,                 KC_LEFT,    KC_DOWN,    KC_RGHT),

// layer win Fn
[3] = LAYOUT_75_ansi(
    _______,    KC_BRID,    KC_BRIU,    KC_CALC,   SOCDOFF,    SOCDTOG,     _______,   KC_MPRV,    KC_MPLY,     KC_MNXT,   KC_MUTE,    KC_VOLD,    KC_VOLU,     KC_PSCR,    _______,    _______,
    _______,    LNK_BLE1,   LNK_BLE2,   LNK_BLE3,  LNK_RF,     _______,     _______,   _______,    _______,     _______,   _______,    _______,    _______,                 _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,DEBOUNCE_PRESS_DEC,    DEBOUNCE_PRESS_SHOW,    DEBOUNCE_PRESS_INC,_______,    _______,DEV_RESET,_______,BAT_SHOW,   _______,
TOG_CAPS_IND,    SLEEP_TIMEOUT_DEC,    SLEEP_TIMEOUT_SHOW,    SLEEP_TIMEOUT_INC,_______,    _______,DEBOUNCE_RELEASE_DEC,   DEBOUNCE_RELEASE_SHOW,    DEBOUNCE_RELEASE_INC,_______,    _______,    _______,    _______,    _______,
    _______,                _______,    _______,   _______,    _______,     BAT_NUM,   _______,    MO(4),       RGB_SPD,   RGB_SPI,    _______,                 _______,    _______,    _______,
    _______,    _______,    _______,                                        _______,                            _______,   MO(3),      _______,                 RM_NEXT,    _______,    RM_HUEU),

// layer 4
[4] = LAYOUT_75_ansi(
    _______,    RM_VALD,    RM_VALU,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,    _______,    _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,                _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     SLEEP_MODE,             TOG_USB_SLP,_______, 
    _______,    _______,    KB_SLP,     DB_TOGG,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,                             _______,    _______,
    _______,                LINK_TO,    _______,   RGB_TEST,   _______,     _______,   _______,    _______,     SIDE_SPD,  SIDE_SPI,   _______,                 _______,    SIDE_VAI,   _______,
    _______,    _______,    _______,                                        _______,                            _______,   MO(4),      _______,                 SIDE_MOD,   SIDE_VAD,   SIDE_HUI),

// layer 5
[5] = LAYOUT_75_ansi(
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,    _______,    _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,                _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,                _______,    _______,
    _______,    _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,                             _______,    _______,
    _______,                _______,    KC_CUT,    KC_COPY,    KC_PASTE,    _______,   _______,    _______,     _______,   _______,    _______,                 _______,    KC_PGUP,    _______,
    _______,    _______,    _______,                                        _______,                            _______,   _______,    _______,                 MS_WHLL,    KC_PGDN,    MS_WHLR)
};

static bool o_prefix_active = false;
static bool u_prefix_active = false;
static bool y_prefix_active = false;
static bool vi_command_sent = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool any_vi_prefix_active = o_prefix_active || u_prefix_active || y_prefix_active;

    switch (keycode) {
        case O_2_VI:
            if (record->event.pressed && !any_vi_prefix_active) {
                // While examples also check record->tap.interrupted,
                // for letter keys I do not want to accidentally hold
                if (!record->tap.count || record->tap.interrupted) {
                    o_prefix_active = true;
                    return false;
                }
            } else {
                o_prefix_active = false;
            }
        case EVIL_U2D:
            if (record->event.pressed && !any_vi_prefix_active) {
                if (!record->tap.count || record->tap.interrupted) {
                    u_prefix_active = true;
                    return false;
                }
            } else {
                u_prefix_active = false;
            }
        case EVIL_Y:
            if (record->event.pressed && !any_vi_prefix_active) {
                if (!record->tap.count || record->tap.interrupted) {
                    y_prefix_active = true;
                    return false;
                }
            } else {
                y_prefix_active = false;
            }
        default:
            if (any_vi_prefix_active
                && record->event.pressed
                && !vi_command_sent) {

                tap_code(KC_F14);

                if (u_prefix_active) {
                    tap_code(KC_D);
                }
                if (y_prefix_active) {
                    tap_code(KC_Y);
                }
            }
    }
    return true;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_T(KC_F13):
            return 100;
        case O_2_VI:
        case EVIL_Y:
        case EVIL_U2D:
            return 500;
        default:
            return TAPPING_TERM;
    }
}
