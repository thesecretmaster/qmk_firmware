#pragma once
#define STARTUP_SONG SONG(PLANCK_SOUND)

#ifdef AUDIO_ENABLE
    // #define STARTUP_SONG SONG(NO_SOUND)
    #define STARTUP_SONG SONG(PLANCK_SOUND)

    /* #define DEFAULT_LAYER_SONGS { SONG(QWERTY_SOUND), \
                                     SONG(COLEMAK_SOUND), \
                                     SONG(DVORAK_SOUND) \
                                   }
    */
#endif

/*
 * MIDI options
 */

/* enable basic MIDI features:
   - MIDI notes can be sent when in Music mode is on
*/

#define MIDI_BASIC

/* enable advanced MIDI features:
   - MIDI notes can be added to the keymap
   - Octave shift and transpose
   - Virtual sustain, portamento, and modulation wheel
   - etc.
*/
//#define MIDI_ADVANCED

/* override number of MIDI tone keycodes (each octave adds 12 keycodes and allocates 12 bytes) */
//#define MIDI_TONE_KEYCODE_OCTAVES 2

// Most tactile encoders have detents every 4 stages
#define ENCODER_RESOLUTION 4

#define AUDIO_INIT_DELAY
// https://kevl.in/2021/02/1000hz-qmk-polling/
// https://beta.docs.qmk.fm/developing-qmk/qmk-reference/config_options
#define USB_POLLING_INTERVAL_MS 1
#define QMK_KEYS_PER_SCAN 4
#define TAPPING_TOGGLE 2
#define COMBO_COUNT 2

// Extend the length of the RAISE + VIM BIND -> LGUI combo. Not _needed_ but nice QOL. Extends it to TAPPING_TERM.
// #define COMBO_MUST_HOLD_MODS
// Actually, make all the combos infinitely long. Could be dangerous, but with just the GAMER_ARROWS and LGUI combos, shouldn't be an issue.
// Again, only a QOL improvement so you can remove this.
#define COMBO_NO_TIMER

// STOLEN FROM PPY
//#define MK_VARIANT MK_TYPE_KINETIC

//#define MK_KINETIC_MOUSE_MAXS 30
//#define MK_KINETIC_MOUSE_ACCN 7
//#define MK_KINETIC_MOUSE_FRIC 16
//#define MK_KINETIC_MOUSE_DRAG 2
//
//#define MK_KINETIC_WHEEL_MAXS 2
//#define MK_KINETIC_WHEEL_ACCN 6

// MY CHANGES

#define MOUSEKEY_INTERVAL 16
#define MOUSEKEY_MAX_SPEED 4
#define MOUSEKEY_DELAY 200
