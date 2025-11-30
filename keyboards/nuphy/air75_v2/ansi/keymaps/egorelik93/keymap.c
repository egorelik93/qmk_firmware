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

#include <stdint.h>
#include "quantum.h"
#include "ansi.h"
#include "keycodes.h"
#include QMK_KEYBOARD_H

#define DSLEEP_TIMEOUT_INC DEEP_SLEEP_TIMEOUT_INC
#define DSLEEP_TIMEOUT_DEC DEEP_SLEEP_TIMEOUT_DEC
#define DSLEEP_TIMEOUT_SHOW DEEP_SLEEP_TIMEOUT_SHOW

#define P_2_VI LT(2, KC_P)
#define EVIL_M2D LT(2, KC_M)
#define EVIL_Y LT(2, KC_Y)

// Tap Dance declarations
enum {
    TD_LGUI,
    TD_LCTL,
    TD_LALT,
    TD_RALT,
};

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
    KC_TAB,     KC_Q,      KC_W,        KC_E,      KC_R,       KC_T,        EVIL_Y,    KC_U,       KC_I,        KC_O,      P_2_VI,     KC_LBRC,    KC_RBRC,                 KC_BSLS,    KC_PGUP,
    TD(TD_LCTL),KC_A,      KC_S,        KC_D,      KC_F,       KC_G,        KC_H,      KC_J,       KC_K,        KC_L,      KC_SCLN,    KC_QUOT,                             KC_ENT,     KC_PGDN,
    KC_LSFT,               KC_Z,        KC_X,      KC_C,       KC_V,        KC_B,      KC_N,   EVIL_M2D,        KC_COMM,   KC_DOT,     KC_SLSH,                 KC_RSFT,    KC_UP,      KC_END,
    TD(TD_LGUI),MO(5),     TD(TD_LALT),                                     KC_SPC,                         TD(TD_RALT),   MO(3),      KC_RCTL,                 KC_LEFT,    KC_DOWN,    KC_RGHT),

// layer win Fn
[3] = LAYOUT_75_ansi(
    _______,    KC_BRID,    KC_BRIU,    KC_CALC,   SOCDOFF,    SOCDTOG,     _______,   KC_MPRV,    KC_MPLY,     KC_MNXT,   KC_MUTE,    KC_VOLD,    KC_VOLU,     KC_PSCR,    _______,    KC_BRK,
    _______,    LNK_BLE1,   LNK_BLE2,   LNK_BLE3,  LNK_RF,     _______,     _______,   _______,    _______,     _______,   _______,    _______,    _______,                 _______,    _______,
    _______,   DSLEEP_TIMEOUT_DEC,   DSLEEP_TIMEOUT_SHOW,   DSLEEP_TIMEOUT_INC,   _______,    _______,DEBOUNCE_PRESS_DEC,    DEBOUNCE_PRESS_SHOW,    DEBOUNCE_PRESS_INC,_______,    _______,DEV_RESET,_______,BAT_SHOW,   _______,
TOG_CAPS_IND,   SLEEP_TIMEOUT_DEC,    SLEEP_TIMEOUT_SHOW,    SLEEP_TIMEOUT_INC,_______,    _______,DEBOUNCE_RELEASE_DEC,   DEBOUNCE_RELEASE_SHOW,    DEBOUNCE_RELEASE_INC,_______,    _______,    _______,    _______,    _______,
    _______,                _______,    _______,   _______,    _______,     BAT_NUM,   _______,    MO(4),       RGB_SPD,   RGB_SPI,    _______,                 _______,    _______,    _______,
    _______,    _______,    _______,                                        _______,                            _______,   MO(3),      _______,                 RM_NEXT,    _______,    RM_HUEU),

// layer 4
[4] = LAYOUT_75_ansi(
    QK_BOOT,    RM_VALD,    RM_VALU,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,    _______,    _______,    _______,
    RF_DFU,     _______,    _______,    _______,   _______,    _______,     _______,   _______,    _______,     _______,   _______,    _______,     _______,                _______,    _______,
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

typedef struct {
    uint16_t tap;
    uint16_t hold;
    void (*double_tap_fn)(void);
    uint16_t held;
} tap_dance_double_tap_hold_t;

void tap_dance_double_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_double_tap_hold_t *tap_hold = (tap_dance_double_tap_hold_t *)user_data;

    if (state->pressed
/*#ifndef PERMISSIVE_HOLD
        && !state->interrupted
#else*/
    ) {
        if (state->count >= 2) {
            tap_code16(tap_hold->tap);
        }

        register_code16(tap_hold->hold);
        tap_hold->held = 2;
    } else {
        if (state->count == 2) {
            tap_hold->held = 3;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = 1;
        }
    }
}

void tap_dance_double_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_double_tap_hold_t *tap_hold = (tap_dance_double_tap_hold_t *)user_data;

    switch (tap_hold->held) {
        case 1:
            unregister_code16(tap_hold->tap);
            tap_hold->held = 0;
            break;
        case 2:
            unregister_code16(tap_hold->hold);
            tap_hold->held = 0;
            break;
        case 3:
            (*tap_hold->double_tap_fn)();
            tap_hold->held = 0;
            break;
    }
}

#define ACTION_TAP_DANCE_DOUBLE_TAP_HOLD(tap, hold, double_tap)                                        \
    {                                                                               \
        .fn        = {NULL, tap_dance_double_tap_hold_finished, tap_dance_double_tap_hold_reset, NULL}, \
        .user_data = (void *)&((tap_dance_double_tap_hold_t){tap, hold, double_tap, 0}),               \
    }


void lgui_command_palatte(void) {
    register_mods(MOD_LGUI | MOD_LALT);
    tap_code(KC_SPC);
    unregister_mods(MOD_LGUI | MOD_LALT);
}

void lctl_double_tap(void) {
    tap_code(KC_F17);
}

void lalt_double_tap(void) {
    tap_code(KC_F15);
}

void ralt_double_tap(void) {
    tap_code(KC_F15);
}

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_LGUI] = ACTION_TAP_DANCE_DOUBLE_TAP_HOLD(KC_LGUI, KC_LGUI, &lgui_command_palatte),
    [TD_LCTL] = ACTION_TAP_DANCE_DOUBLE_TAP_HOLD(KC_F16, KC_LCTL, &lctl_double_tap),
    [TD_LALT] = ACTION_TAP_DANCE_DOUBLE_TAP_HOLD(KC_F13, KC_LALT, &lalt_double_tap),
    [TD_RALT] = ACTION_TAP_DANCE_DOUBLE_TAP_HOLD(KC_F13, KC_RALT, &ralt_double_tap),
};

#define EVIL_TAPPING_TERM 150

static bool p_prefix_active = false;
static bool m_prefix_active = false;
static bool y_prefix_active = false;
static bool vi_command_incomplete = false;
static bool vi_command_sent = false;
static uint16_t p_timer = 0;
static uint16_t m_timer = 0;
static uint16_t y_timer = 0;
static uint16_t vi_mod_handling = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool any_vi_prefix_active = p_prefix_active || m_prefix_active || y_prefix_active;

    switch (keycode) {
        case P_2_VI:
            if (record->event.pressed && !any_vi_prefix_active) {
                // While examples also check record->tap.interrupted,
                // for letter keys I do not want to accidentally hold
                if (!record->tap.count) {
                    p_prefix_active = true;
                    p_timer = timer_read();
                    return false;
                }
            } else {
                bool no_process = p_prefix_active;
                if (p_prefix_active && !vi_command_sent) {
                    tap_code(KC_P);
                }

                p_timer = 0;
                p_prefix_active = false;

                if (no_process) {
                    vi_command_sent = false;
                    vi_command_incomplete = false;
                    return false;
                }
            }
        case EVIL_M2D:
            if (record->event.pressed && !any_vi_prefix_active) {
                if (!record->tap.count) {
                    m_prefix_active = true;
                    m_timer = timer_read();
                    return false;
                }
            } else {
                bool no_process = m_prefix_active;
                if (m_prefix_active && !vi_command_sent) {
                    tap_code(KC_M);
                }

                m_timer = 0;
                m_prefix_active = false;

                if (no_process) {
                    vi_command_sent = false;
                    vi_command_incomplete = false;
                    return false;
                }
            }
        case EVIL_Y:
            if (record->event.pressed && !any_vi_prefix_active) {
                if (!record->tap.count) {
                    y_prefix_active = true;
                    y_timer = timer_read();
                    return false;
                }
            } else {
                bool no_process = y_prefix_active;
                if (y_prefix_active && !vi_command_sent) {
                    tap_code(KC_Y);
                }

                y_timer = 0;
                y_prefix_active = false;

                if (no_process) {
                    vi_command_sent = false;
                    vi_command_incomplete = false;
                    return false;
                }
            }
        // This adds extra keys in my current usage, but leaving commented as an example.
        /*case TD(TD_LGUI): // list all tap dance keycodes with tap-hold configurations
            tap_dance_action_t *action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_double_tap_hold_t *tap_hold = (tap_dance_double_tap_hold_t *)action->user_data;
                if (action->state.count == 2) {
                    (*tap_hold->double_tap_fn)();
                }
            }*/
        case TD(TD_LCTL):
            if (vi_command_incomplete && record->event.pressed) {
                vi_mod_handling |= MOD_LCTL;
                return false;
            } else if ((vi_mod_handling & MOD_LCTL) && !record->event.pressed) {
                unregister_mods(MOD_LCTL);
                vi_mod_handling &= ~MOD_LCTL;
                return false;
            }
        case TD(TD_LALT):
            if (vi_command_incomplete && record->event.pressed) {
                vi_mod_handling |= MOD_LALT;
                return false;
            } else if ((vi_mod_handling & MOD_LALT) && !record->event.pressed) {
                unregister_mods(MOD_LALT);
                vi_mod_handling &= ~MOD_LALT;
                return false;
            }
        case TD(TD_RALT):
            if (vi_command_incomplete && record->event.pressed) {
                vi_mod_handling |= MOD_RALT;
                return false;
            } if ((vi_mod_handling & MOD_RALT) && !record->event.pressed) {
                unregister_mods(MOD_RALT);
                vi_mod_handling &= ~MOD_RALT;
                return false;
            }
        default:
            if (any_vi_prefix_active
                && record->event.pressed
                && !vi_command_incomplete) {

                uint16_t p_elapsed = timer_elapsed(p_timer);
                uint16_t m_elapsed = timer_elapsed(m_timer);
                uint16_t y_elapsed = timer_elapsed(y_timer);

                if ((p_prefix_active && p_elapsed >= EVIL_TAPPING_TERM) ||
                    (m_prefix_active && m_elapsed >= EVIL_TAPPING_TERM) ||
                    (y_prefix_active && y_elapsed >= EVIL_TAPPING_TERM)) {

                    tap_code(KC_F14);

                    if (m_prefix_active && timer_elapsed(m_timer) >= EVIL_TAPPING_TERM) {
                        tap_code(KC_D);
                    }
                    if (y_prefix_active && timer_elapsed(y_timer) >= EVIL_TAPPING_TERM) {
                        tap_code(KC_Y);
                    }

                    vi_command_sent = true;

                    if (IS_MODIFIER_KEYCODE(keycode)) {
                        vi_command_incomplete = true;
                    } else if (keycode == TD(TD_LCTL) || keycode == TD(TD_LALT) || keycode == TD(TD_RALT)) {
                        vi_command_incomplete = true;

                        if (keycode == TD(TD_LCTL)) {
                            vi_mod_handling |= MOD_LCTL;
                        }
                        if (keycode == TD(TD_LALT)) {
                            vi_mod_handling |= MOD_LALT;
                        }
                        if (keycode == TD(TD_RALT)) {
                            vi_mod_handling |= MOD_RALT;
                        }

                        return false;
                    }
                } else if (p_prefix_active && p_elapsed < EVIL_TAPPING_TERM) {
                    tap_code(KC_P);
                    p_timer = 0;
                    p_prefix_active = false;
                } else if (m_prefix_active && m_elapsed < EVIL_TAPPING_TERM) {
                    tap_code(KC_M);
                    m_timer = 0;
                    m_prefix_active = false;
                } else if (y_prefix_active && y_elapsed < EVIL_TAPPING_TERM) {
                    tap_code(KC_Y);
                    y_timer = 0;
                    y_prefix_active = false;
                }
            } else if (vi_command_incomplete
                       && !IS_MODIFIER_KEYCODE(keycode)
                       && record->event.pressed) {
                if (vi_mod_handling) {
                    register_mods(vi_mod_handling);
                }
                vi_command_incomplete = false;
            }
    }
    return true;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTL_T(KC_F13):
        case TD(TD_LCTL):
        case TD(TD_LALT):
        case TD(TD_RALT):
            return 200;
        default:
            return TAPPING_TERM;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_LCTL):
        case TD(TD_LALT):
        case TD(TD_RALT):
            // Immediately select the hold action when another key is tapped.
            return true;
        default:
            // Do not select the hold action when another key is tapped.
            return false;
    }
}

/*void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  debug_keyboard=true;
  debug_mouse=true;
}*/
