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
  _LOWER,
  _RAISE,
  _VIM_BINDS,
  _NUMPAD,
  _GAMER_ARROWS,
  _MC_OVERRIDES,
  _NUMBAR,
  _ADJUST
};

enum planck_keycodes {
  QWERTY = SAFE_RANGE,
  PLOVER,
  LAYER_RESET,
  MC_LAYOUT,
  SHIFT_CAPS,
  GAMER_ARROW_ON,
  GAMER_ARROW_OFF,
  HOLD,
  MO_VIM_BINDS,
  MO_RAISE
};

#ifndef COMBO
// we need to do more initialization later, but we do reuse the default combo stuff sowe'll defined COMBO and TSM_COMBO
// #define TSM_COMBO
// same definition as in combo file https://github.com/qmk/qmk_firmware/blob/2c5d66987da4c7d5857cab8b09c4f852b4d8e4d9/quantum/process_keycode/process_combo.h
#define COMBO_END 0
typedef uint16_t tsm_combo_bitmap;
typedef struct {
    const uint16_t  *keys;
    uint16_t         keycode;
    tsm_combo_bitmap keys_bitmap;
    bool             active;
} combo_t;
#define COMBO(ck, ca) \
    { .active = false, .keys_bitmap = 0x0, .keys = &(ck)[0], .keycode = (ca) }
static tsm_combo_bitmap active_keys_bitmap = 0x0;

static uint16_t *active_keys_list;
static uint16_t active_keys_list_length;
#endif

const uint16_t PROGMEM gamer_arrow_combo[] = {KC_MUTE, KC_VOLD, KC_VOLU, COMBO_END};
const uint16_t PROGMEM lgui_combo[] = {KC_LALT, MO(_RAISE), COMBO_END};
const uint16_t PROGMEM lgui_space_combo[] = {MO(_RAISE), KC_SPC, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
   COMBO(gamer_arrow_combo, GAMER_ARROW_ON),
   COMBO(lgui_combo, KC_LGUI),
   COMBO(lgui_space_combo, LGUI(KC_SPC))
};

#ifdef TSM_COMBO
static bool key_combos_contains_before(uint16_t keycode, uint8_t max_idx) {
   uint8_t idx = 0;
   for (uint8_t i = 0; i < COMBO_COUNT; i++) {
      const uint16_t *key = key_combos[i].keys;
      while (*key != COMBO_END) {
         if (idx >= max_idx) {
            return false;
         } else if (*key == keycode) {
            return true;
         }
         idx++;
         key += 1;
      }
   }
   return false;
}

void keyboard_post_init_user(void) {
   // First we calculate the size of the state machine (number of unique keycodes)
   uint8_t gidx = 0;
   for (int i = 0; i < COMBO_COUNT; i++) {
      const uint16_t *key = key_combos[i].keys;
      while (*key != COMBO_END) {
         if (!key_combos_contains_before(*key, gidx))
            active_keys_list_length += 1;
         key += 1;
         gidx += 1;
      }
   }
   // Then we allocate our state machine
   active_keys_list = malloc(sizeof(uint16_t) * active_keys_list_length);
   // Then we initialize it!
   uint8_t sm_idx = 0;
   gidx = 0;
   for (int i = 0; i < COMBO_COUNT; i++) {
      const uint16_t *key = key_combos[i].keys;
      while (*key != COMBO_END) {
         if (!key_combos_contains_before(*key, gidx)) {
            active_keys_list[sm_idx] = *key;
            sm_idx += 1;
         }
         key += 1;
         gidx += 1;
      }
   }
   // Initialize the per-combo bitmaps
   for (int i = 0; i < COMBO_COUNT; i++) {
      const uint16_t *key = key_combos[i].keys;
      while (*key != COMBO_END) {
         int key_idx = -1;
         for (int j = 0; j < active_keys_list_length; j++)
           if (active_keys_list[j] == *key)
             key_idx = j;
         if (key_idx >= 0)
           key_combos[i].keys_bitmap |= 0x1 << key_idx;
         key += 1;
      }
   }
}

#define COMBO_KEY_POS ((keypos_t){.col = 254, .row = 254})
static void tsm_send_keyrecord(uint16_t keycode, bool pressed) {
  uprintf("Sending key %d as %s\n", keycode, pressed ? "true" : "false");
  keyrecord_t record = {
      .event =
          {
              .key     = COMBO_KEY_POS,
              .time    = timer_read() | 1,
              .pressed = pressed,
          },
      .keycode = keycode,
  };
#ifndef NO_ACTION_TAPPING
  action_tapping_process(record);
#else
  process_record(&record);
#endif
}

static void tsm_combo_activate(int combo_idx) {
  tsm_send_keyrecord(key_combos[combo_idx].keycode, true);
  const uint16_t *key = key_combos[combo_idx].keys;
  while (*key != COMBO_END) {
    tsm_send_keyrecord(*key, false);
    key += 1;
  }
}

static void tsm_combo_deactivate(int combo_idx, uint16_t keycode) {
  // Unpress the combo keycode
  // Re-press everything except for keycode
  tsm_send_keyrecord(key_combos[combo_idx].keycode, false);
  const uint16_t *key = key_combos[combo_idx].keys;
  bool keycode_ignored = false;
  while (*key != COMBO_END) {
    if (!keycode_ignored && *key != keycode) {
      keycode_ignored = true;
    } else {
    tsm_send_keyrecord(*key, true);
}
    key += 1;
  }
}

static bool tsm_combo_run(uint16_t keycode, keyrecord_t *record) {
   // Check if we have a live combo right now
   if (record->event.key.col == COMBO_KEY_POS.col && record->event.key.row == COMBO_KEY_POS.row)
     return false;
   // Check if the keycode is a key that's used in combos
   int key_idx = -1;
   for (int i = 0; i < active_keys_list_length; i++) {
      if (active_keys_list[i] == keycode) {
         key_idx = i;
         break;
      }
   }
   // If the key isn't used in any combos, skip!
   if (key_idx >= 0) {
      uprintf("BEFORE: %X\n", active_keys_bitmap);
      if (record->event.pressed) {
         // Add the keycode to the list of pressed keys
         active_keys_bitmap |= (0x1 << key_idx);
         // checkable_keys is all of the currently pressed keys WITHOUT the keys that have been consumed
         tsm_combo_bitmap checkable_keys = active_keys_bitmap;
         for (int i = 0; i < COMBO_COUNT; i++)
            if (key_combos[i].active)
               checkable_keys &= ~(key_combos[i].keys_bitmap);
         // check if the remaining consumeable keypressed form a combo, if so activate the combo and "comsume" the presses in that combo
         for (int i = 0; i < COMBO_COUNT; i++) {
            uprintf("Combo %d: %X\n", i, key_combos[i].keys_bitmap);
            if ((checkable_keys & key_combos[i].keys_bitmap) == key_combos[i].keys_bitmap) {
               // ACTIVATE THE COMBO
               uprintf("Activating combo %d\n", i);
               active_keys_bitmap &= ~key_combos[i].keys_bitmap;
               key_combos[i].active = true;
               tsm_combo_activate(i);
               break;
             }
         }
      } else {
         if ((active_keys_bitmap & (0x1 << key_idx)) != 0x0) {
            // If it's only pressed and not consumed, we can just unpress it
            active_keys_bitmap &= ~(0x1 << key_idx);
         } else {
            // If it's consumed, we need to find which combo consumed it (there can only be one) and then release the combo and release the key
            for (int i = 0; i < COMBO_COUNT; i++) {
               if (key_combos[i].active && (key_combos[i].keys_bitmap & (0x1 << key_idx)) != 0) {
                  // Move all of the keys back from the combo to the main list ("consumed" -> "pressed")
                  active_keys_bitmap |= key_combos[i].keys_bitmap;
                  // Unpress the one key
                  active_keys_bitmap &= ~(0x1 << key_idx);
                  // Mark the combo as inactive
                  key_combos[i].active = false;
                  // DEACTIVATE THE COMBO
                  uprintf("Deactivating combo %d\n", i);
                  tsm_combo_deactivate(i, keycode);
                  break;
               }
            }
         }
      }
    uprintf("AFTER: %X\n", active_keys_bitmap);
   }
   return true;
}
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_MC_OVERRIDES] = LAYOUT_planck_grid(
    _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    KC_LSFT,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    LAYER_RESET, _______, _______, _______, KC_SPC,  _______, KC_LALT, _______, _______, _______, _______, _______
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
    _______, _______, KC_MS_BTN2, KC_MS_U, KC_MS_BTN1, _______, _______, KC_PGDN, KC_PGUP, _______, _______, _______,
    _______, _______, KC_MS_L,    KC_MS_D, KC_MS_R,    _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, _______,
    _______, _______, _______,    _______, _______,    _______, KC_HOME, KC_END,  _______, _______, _______, _______,
    _______, _______, _______,    _______, _______,    _______, _______, _______, _______, _______, _______, _______
),
[_NUMBAR] = LAYOUT_planck_grid(
    _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

[_GAMER_ARROWS] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, _______, _______,         _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,         _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,         _______, KC_UP,   _______,
    _______, _______, _______, _______, _______, _______, _______, _______, GAMER_ARROW_OFF, KC_LEFT, KC_DOWN, KC_RGHT
),

[_NUMPAD] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, KC_7,    KC_8, KC_9,    _______, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_4,    KC_5, KC_6,    KC_PMNS, _______,
    _______, _______, _______, _______, _______, _______, _______, KC_1,    KC_2, KC_3,    KC_PPLS, _______,
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
    KC_TAB,     KC_Q, KC_W,       KC_E,     KC_R,    KC_T,       KC_Y,   KC_U,        KC_I,           KC_O,    KC_P,    KC_BSPC,
    KC_ESC,     KC_A, KC_S,       KC_D,     KC_F,    KC_G,       KC_H,   KC_J,        KC_K,           KC_L,    KC_SCLN, KC_QUOT,
    SHIFT_CAPS, KC_Z, KC_X,       KC_C,     KC_V,    KC_B,       KC_N,   KC_M,        KC_COMM,        KC_DOT,  KC_SLSH, KC_ENT ,
    KC_LGUI,    HOLD, MO(_LOWER), KC_LCTRL, KC_LALT, MO(_RAISE), KC_SPC, TT(_NUMPAD), MO(_VIM_BINDS), KC_MUTE, KC_VOLD, KC_VOLU
),

[_LOWER] = LAYOUT_planck_grid(
    _______, _______, _______, _______, _______, _______, _______, _______, KC_LBRC, KC_RBRC, _______, _______,
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   _______, KC_LPRN, KC_RPRN, _______, _______,
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_LCBR, KC_RCBR, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
),

[_RAISE] = LAYOUT_planck_grid(
    _______, _______, KC_AT,   KC_EXLM, KC_GRV,  _______, _______, KC_ASTR, KC_CIRC, KC_PERC, _______, _______,
    _______, KC_LCBR, KC_LPRN, KC_RPRN, KC_UNDS, KC_PIPE, _______, KC_EQL,  KC_MINS, KC_PLUS, _______, _______,
    _______, KC_RCBR, KC_LBRC, KC_RBRC, KC_TILD, KC_AMPR, _______, KC_HASH, KC_AT,   KC_DLR,  KC_BSLS, _______,
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
    _______, RESET,   DEBUG,   RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD,  RGB_VAI,         RGB_VAD,           _______,
    _______, _______, MU_MOD,  AU_ON,   AU_OFF,  AG_NORM, AG_SWAP, QWERTY,  PLOVER,   MC_LAYOUT,       _______,           _______,
    _______, MUV_DE,  MUV_IN,  MU_ON,   MU_OFF,  MI_ON,   MI_OFF,  TERM_ON, TERM_OFF, MAGIC_HOST_NKRO, MAGIC_UNHOST_NKRO, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,         _______,           _______
)

};

#ifdef AUDIO_ENABLE
  float plover_song[][2]     = SONG(PLOVER_SOUND);
  float plover_gb_song[][2]  = SONG(PLOVER_GOODBYE_SOUND);
#endif

// layer_state_t layer_state_set_user(layer_state_t state) {
//   return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
// }

static float arrows_ctrl_song_up[][2] = SONG(E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),);
static float arrows_ctrl_song_down[][2] = SONG(E__NOTE(_E7), E__NOTE(_A6), ED_NOTE(_E6),);

static float shift_caps_song_up[][2] = SONG(E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),);
static float shift_caps_song_down[][2] = SONG(E__NOTE(_E7), E__NOTE(_A6), ED_NOTE(_E6),);

void set_capslock_state(bool newstate) {
   if (host_keyboard_led_state().caps_lock != newstate) {
      register_code(KC_CAPS);
      unregister_code(KC_CAPS);
   }
}

struct double_tap_manager {
   uint16_t timestamps[4];
   uint8_t  idx;
   uint16_t exit_timer;
};
static struct double_tap_manager shift_caps_tracker = {.timestamps = {0,0,0,0}, .idx = 0};
void double_tap_check(bool is_pressed, struct double_tap_manager *manager, uint16_t kc) {
      if (is_pressed) {
         register_code(kc);
      } else {
         unregister_code(kc);
      }
      if (host_keyboard_led_state().caps_lock) {
         if (is_pressed) {
            manager->exit_timer = timer_read();
         } else if (timer_elapsed(manager->exit_timer) < TAPPING_TERM) {
            set_capslock_state(false);
            PLAY_SONG(shift_caps_song_down);
         }
      } else {
         if (timer_elapsed(manager->timestamps[0]) > TAPPING_TERM * manager->idx) {
           manager->idx = 0;
         }
         if (manager->idx == 0 && is_pressed) {
            manager->timestamps[0] = timer_read();
            manager->idx = 1;
         } else if ((is_pressed ? 0 : 1) == (manager->idx % 2)) {
           if (timer_elapsed(manager->timestamps[manager->idx - 1]) > TAPPING_TERM) {
               manager->idx = 0;
            } else {
               manager->timestamps[manager->idx] = timer_read();
               manager->idx += 1;
            }
            if (manager->idx == 4) {
               manager->idx = 0;
               set_capslock_state(true);
               PLAY_SONG(shift_caps_song_up);
            }
         }
      }
}
#define HELD_MAX 10
static bool held = false;
static int held_count = 0;
struct held_key {
   keyrecord_t record;
   uint16_t keycode;
};
static struct held_key held_buffer[HELD_MAX];
void release_held_keys(void) {
  for (int i = 0; i < held_count; i++) {
     unregister_code(held_buffer[i].keycode);
  }
  held_count = 0;
  held = false;
}
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (held && keycode <= KC_0 && keycode >= KC_A) {
     release_held_keys();
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tsm_combo_run(keycode, record);
  if (held && !record->event.pressed && keycode < QK_FUNCTION_MAX) {
     // memcpy(&held_buffer[held_count].record, record, sizeof(keyrecord_t));
     held_buffer[held_count].keycode = keycode;
     held_count++;
     return false;
  }
  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case HOLD:
      if (record->event.pressed) {
        if (held) {
           release_held_keys();
        } else {
           held = true;
        }
      }
      return false;
    case MC_LAYOUT:
      if (record->event.pressed) {
        #ifdef AUDIO_ENABLE
          stop_all_notes();
          PLAY_SONG(shift_caps_song_up);
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
    case GAMER_ARROW_ON:
      if (record->event.pressed) {
         layer_on(_GAMER_ARROWS);
         PLAY_SONG(arrows_ctrl_song_up);
      }
      return false;
    case GAMER_ARROW_OFF:
      if (record->event.pressed) {
         layer_off(_GAMER_ARROWS);
         PLAY_SONG(arrows_ctrl_song_down);
      }
      return false;
    case SHIFT_CAPS:
      double_tap_check(record->event.pressed, &shift_caps_tracker, KC_LSFT);
      return false;
  }
  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  rgblight_config_t rgblight_config;
  switch(get_highest_layer(state)) {
     // case _CAPSLOCK:
     //   rgblight_enable_noeeprom();
     //   rgblight_sethsv_noeeprom(HSV_BLUE);
     //   break;
     case _NUMPAD:
       rgblight_enable_noeeprom();
       rgblight_sethsv_noeeprom(HSV_RED);
       break;
     case _GAMER_ARROWS:
       rgblight_enable_noeeprom();
       rgblight_sethsv_noeeprom(HSV_PURPLE);
       break;
     case _PLOVER:
       rgblight_enable_noeeprom();
       rgblight_sethsv_noeeprom(HSV_GREEN);
       break;
     case _MC_OVERRIDES:
       rgblight_enable_noeeprom();
       rgblight_sethsv_noeeprom(HSV_RED);
       break;
     default:
       //Read RGB Light State
       rgblight_config.raw = eeconfig_read_rgblight();
       //If enabled, set white
       if (rgblight_config.enable) {
         rgblight_sethsv_noeeprom(HSV_WHITE);
       } else { //Otherwise go back to disabled
         rgblight_disable_noeeprom();
       }
       break;
  }
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
  // return state;
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
  steno_set_mode(STENO_MODE_GEMINI); // or STENO_MODE_BOLT
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
