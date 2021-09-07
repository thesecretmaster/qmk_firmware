/* Copyright 2015-2017 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "keymap_steno.h"
#include "muse.h"


enum planck_layers {
  _QWERTY,
  _PLOVER,
  _VIM_BINDS,
  _NUMPAD,
  _GAMER_ARROWS,
  _MC_OVERRIDES,
  _NUMBAR,
  _LOWER,
  _RAISE,
  _ADJUST
};

enum planck_keycodes {
  QWERTY = SAFE_RANGE,
  PLOVER,
  LAYER_RESET,
  MC_LAYOUT,
  ARROWS_CTRL,
  SHIFT_CAPS
};

// IDEAS:
// DONE - Have a modifier layer that moves the vim arrows to the vim place
// - Real numpad
// - Bind ctrl to held esc

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

// [_QWERTY] = LAYOUT_planck_grid(
//     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
// )
[_MC_OVERRIDES] = LAYOUT_planck_grid(
    _______,  _______, _______,      _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______,      _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,  _______, _______,      _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LCTRL, _______, OSL(_NUMBAR), KC_SPC,  _______, _______, _______, _______, _______, _______, _______, LAYER_RESET
),
[_PLOVER] = LAYOUT_planck_grid(
  STN_N1,      STN_N2,  STN_N3,  STN_N4,  STN_N5,  STN_N6,  STN_N7,  STN_N8,  STN_N9,  STN_NA,  STN_NB,  STN_NC ,
  STN_FN,      STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1, STN_ST3, STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR ,
  XXXXXXX,     STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2, STN_ST4, STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR ,
  LAYER_RESET, XXXXXXX, XXXXXXX, STN_A,   STN_O,   XXXXXXX, XXXXXXX, STN_E,   STN_U,   STN_PWR, STN_RE1, STN_RE2
),
// [_PLOVER] = LAYOUT_planck_grid(
//     KC_1,        KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1,    KC_1   ,
//     XXXXXXX,     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
//     XXXXXXX,     KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
//     LAYER_RESET, XXXXXXX, XXXXXXX, KC_C,    KC_V,    XXXXXXX, XXXXXXX, KC_N,    KC_M,    XXXXXXX, XXXXXXX, XXXXXXX
// ),

[_VIM_BINDS] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, KC_PGDN, KC_PGUP, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),
[_NUMBAR] = LAYOUT_planck_grid(
    _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

[_GAMER_ARROWS] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_UP,   _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT
),

[_NUMPAD] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, KC_7,    KC_8, KC_9,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_4,    KC_5, KC_6,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_1,    KC_2, KC_3,    KC_DOT,  _______,
    _______, _______, _______, _______, _______, _______, _______, _______, KC_0, _______, _______, _______
),
/* Qwerty
 * ,-----------------------------------------------------------------------------------.
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Esc  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Brite| Ctrl | Alt  | GUI  |Lower |    Space    |Raise | Left | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_planck_grid(
    KC_TAB,                KC_Q,    KC_W,    KC_E,       KC_R,         KC_T,           KC_Y,    KC_U,        KC_I,    KC_O,    KC_P,    KC_BSPC,
    KC_ESC,                KC_A,    KC_S,    KC_D,       KC_F,         KC_G,           KC_H,    KC_J,        KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    /* MT(MOD_LSFT, KC_CAPS) */ SHIFT_CAPS, KC_Z,    KC_X,    KC_C,       KC_V,         KC_B,           KC_N,    KC_M,        KC_COMM, KC_DOT,  KC_SLSH, KC_ENT ,
    ARROWS_CTRL,           KC_LGUI, KC_LALT, MO(_LOWER), MO(_RAISE),   MO(_VIM_BINDS), KC_SPC,  TT(_NUMPAD), XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU
),

/* Lower
 * ,-----------------------------------------------------------------------------------.
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   _  |   +  |   {  |   }  |  |   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO ~ |ISO | | Home | End  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      | Next | Vol- | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_LOWER] = LAYOUT_planck_grid(
    KC_TILD, _______, _______, _______, _______, _______, _______, _______, KC_LBRC, KC_RBRC, _______, _______,
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   _______, KC_LPRN, KC_RPRN, _______, _______,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_LCBR, KC_RCBR, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

/* Raise
 * ,-----------------------------------------------------------------------------------.
 * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   -  |   =  |   [  |   ]  |  \   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO # |ISO / |Pg Up |Pg Dn |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      | Next | Vol- | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_RAISE] = LAYOUT_planck_grid(
    _______, KC_TILD, _______, KC_EXLM, KC_GRV,  _______, _______, KC_ASTR, KC_CIRC, KC_PERC, _______, _______,
    _______, _______, KC_AMPR, KC_PIPE, KC_UNDS, _______, _______, KC_EQL,  KC_MINS, KC_PLUS, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_HASH, KC_AT,   KC_DLR,  KC_BSLS, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

/* Adjust (Lower + Raise)
 *                      v------------------------RGB CONTROL--------------------v
 * ,-----------------------------------------------------------------------------------.
 * |      | Reset|Debug | RGB  |RGBMOD| HUE+ | HUE- | SAT+ | SAT- |BRGTH+|BRGTH-|  Del |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |MUSmod|Aud on|Audoff|AGnorm|AGswap|Qwerty|Colemk|Dvorak|Plover|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Voice-|Voice+|Mus on|Musoff|MIDIon|MIDIof|TermOn|TermOf|      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_ADJUST] = LAYOUT_planck_grid(
    _______, RESET,   DEBUG,   RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD,  RGB_VAI,   RGB_VAD, KC_DEL ,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, QWERTY,  PLOVER,   MC_LAYOUT, _______, _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  TERM_ON, TERM_OFF, _______,   _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,   _______, _______
)

};

#ifdef AUDIO_ENABLE
  float plover_song[][2]     = SONG(PLOVER_SOUND);
  float plover_gb_song[][2]  = SONG(PLOVER_GOODBYE_SOUND);
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

static float arrows_ctrl_song_up[][2] = SONG(E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),);
static float arrows_ctrl_song_down[][2] = SONG(E__NOTE(_E7), E__NOTE(_A6), ED_NOTE(_E6),);

static float shift_caps_song_up[][2] = SONG(E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),);
static float shift_caps_song_down[][2] = SONG(E__NOTE(_E7), E__NOTE(_A6), ED_NOTE(_E6),);

static uint16_t arrows_ctrl_timer;

static uint16_t shift_caps_tracker[4] = {0,0,0,0};
static uint8_t shift_caps_idx = 0;
static bool shift_caps_active = false;
static uint16_t shift_caps_exit_timer;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case MC_LAYOUT:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          stop_all_notes();
          PLAY_SONG(plover_song);
        #endif
        layer_clear();
        layer_on(_QWERTY);
        layer_on(_MC_OVERRIDES);
      }
      return false;
    case PLOVER:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          stop_all_notes();
          PLAY_SONG(plover_song);
        #endif
        layer_clear();
        layer_on(_QWERTY);
        layer_on(_PLOVER);
      }
      return false;
    case LAYER_RESET:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
        PLAY_SONG(plover_gb_song);
        #endif
        layer_clear();
        layer_on(_QWERTY);
      }
      return false;
    case ARROWS_CTRL:
      if (record->event.pressed) {
         arrows_ctrl_timer = timer_read();
         register_code(KC_LCTRL);
      } else {
         unregister_code(KC_LCTRL);
         if (timer_elapsed(arrows_ctrl_timer) < TAPPING_TERM) {
            layer_invert(_GAMER_ARROWS);
            if (layer_state_is(_GAMER_ARROWS)) {
               PLAY_SONG(arrows_ctrl_song_up);
            } else {
               PLAY_SONG(arrows_ctrl_song_down);
            }
         }
      }
      return false;
    case SHIFT_CAPS:
      if (record->event.pressed) {
         register_code(KC_LSFT);
      } else {
         unregister_code(KC_LSFT);
      }
      if (shift_caps_active) {
         if (record->event.pressed) {
            shift_caps_exit_timer = timer_read();
         } else if (timer_elapsed(shift_caps_exit_timer) < TAPPING_TERM) {
            shift_caps_active = false;
            register_code(KC_CAPS);
            unregister_code(KC_CAPS);
            PLAY_SONG(shift_caps_song_down);
         }
      } else {
         if (shift_caps_idx == 0 && record->event.pressed) {
            shift_caps_tracker[0] = timer_read();
            shift_caps_idx = 1;
         } else if (record->event.pressed == !(shift_caps_idx % 2)) {
            if (timer_elapsed(shift_caps_tracker[0]) > TAPPING_TERM * shift_caps_idx) {
               shift_caps_idx = 0;
            } else if (timer_elapsed(shift_caps_tracker[shift_caps_idx - 1]) > TAPPING_TERM) {
               shift_caps_idx = 0;
            } else {
               shift_caps_tracker[shift_caps_idx] = timer_read();
               shift_caps_idx += 1;
            }
            if (shift_caps_idx == 4) {
               shift_caps_idx = 0;
               shift_caps_active = true;
               register_code(KC_CAPS);
               unregister_code(KC_CAPS);
               PLAY_SONG(shift_caps_song_up);
            }
         }
      }
      return false;
  }
  return true;
}

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

bool encoder_update(bool clockwise) {
  if (muse_mode) {
    if (IS_LAYER_ON(_RAISE)) {
      if (clockwise) {
        muse_offset++;
      } else {
        muse_offset--;
      }
    } else {
      if (clockwise) {
        muse_tempo+=1;
      } else {
        muse_tempo-=1;
      }
    }
  } else {
    if (clockwise) {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_DOWN);
      #else
        tap_code(KC_PGDN);
      #endif
    } else {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_UP);
      #else
        tap_code(KC_PGUP);
      #endif
    }
  }
    return true;
}

void dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0: {
#ifdef AUDIO_ENABLE
            static bool play_sound = false;
#endif
            if (active) {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_song); }
#endif
                layer_on(_ADJUST);
            } else {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_gb_song); }
#endif
                layer_off(_ADJUST);
            }
#ifdef AUDIO_ENABLE
            play_sound = true;
#endif
            break;
        }
        case 1:
            if (active) {
                muse_mode = true;
            } else {
                muse_mode = false;
            }
    }
}

void matrix_init_user() {
  steno_set_mode(STENO_MODE_BOLT); // or STENO_MODE_GEMINI
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
  switch (keycode) {
    case MO(_RAISE):
    case MO(_LOWER):
      return false;
    default:
      return true;
  }
}
