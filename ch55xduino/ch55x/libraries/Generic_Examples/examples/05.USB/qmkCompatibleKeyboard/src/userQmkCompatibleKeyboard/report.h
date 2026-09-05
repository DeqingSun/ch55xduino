#pragma once

/* Consumer Page (0x0C)
 *
 * See https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf#page=75
 */
enum consumer_usages {
    // 15.5 Display Controls
    SNAPSHOT        = 0x065,
    BRIGHTNESS_UP   = 0x06F, // https://www.usb.org/sites/default/files/hutrr41_0.pdf
    BRIGHTNESS_DOWN = 0x070,
    // 15.7 Transport Controls
    TRANSPORT_RECORD       = 0x0B2,
    TRANSPORT_FAST_FORWARD = 0x0B3,
    TRANSPORT_REWIND       = 0x0B4,
    TRANSPORT_NEXT_TRACK   = 0x0B5,
    TRANSPORT_PREV_TRACK   = 0x0B6,
    TRANSPORT_STOP         = 0x0B7,
    TRANSPORT_EJECT        = 0x0B8,
    TRANSPORT_RANDOM_PLAY  = 0x0B9,
    TRANSPORT_STOP_EJECT   = 0x0CC,
    TRANSPORT_PLAY_PAUSE   = 0x0CD,
    // 15.9.1 Audio Controls - Volume
    AUDIO_MUTE     = 0x0E2,
    AUDIO_VOL_UP   = 0x0E9,
    AUDIO_VOL_DOWN = 0x0EA,
    // 15.15 Application Launch Buttons
    AL_CC_CONFIG       = 0x183,
    AL_EMAIL           = 0x18A,
    AL_CALCULATOR      = 0x192,
    AL_LOCAL_BROWSER   = 0x194,
    AL_LOCK            = 0x19E,
    AL_CONTROL_PANEL   = 0x19F,
    AL_ASSISTANT       = 0x1CB,
    AL_KEYBOARD_LAYOUT = 0x1AE,
    // 15.16 Generic GUI Application Controls
    AC_NEW                         = 0x201,
    AC_OPEN                        = 0x202,
    AC_CLOSE                       = 0x203,
    AC_EXIT                        = 0x204,
    AC_MAXIMIZE                    = 0x205,
    AC_MINIMIZE                    = 0x206,
    AC_SAVE                        = 0x207,
    AC_PRINT                       = 0x208,
    AC_PROPERTIES                  = 0x209,
    AC_UNDO                        = 0x21A,
    AC_COPY                        = 0x21B,
    AC_CUT                         = 0x21C,
    AC_PASTE                       = 0x21D,
    AC_SELECT_ALL                  = 0x21E,
    AC_FIND                        = 0x21F,
    AC_SEARCH                      = 0x221,
    AC_HOME                        = 0x223,
    AC_BACK                        = 0x224,
    AC_FORWARD                     = 0x225,
    AC_STOP                        = 0x226,
    AC_REFRESH                     = 0x227,
    AC_BOOKMARKS                   = 0x22A,
    AC_NEXT_KEYBOARD_LAYOUT_SELECT = 0x29D,
    AC_DESKTOP_SHOW_ALL_WINDOWS    = 0x29F,
    AC_SOFT_KEY_LEFT               = 0x2A0
};
