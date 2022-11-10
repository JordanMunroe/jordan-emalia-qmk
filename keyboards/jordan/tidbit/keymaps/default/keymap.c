/* Copyright 2021 Jay Greco
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

#include <stdio.h>
#include "jordan/tidbit/tidbit.h"
#include "jordan/tidbit/config.h"
#include "bongo.h"

#define KEYBOARD_CENTER_COL 4

enum layers
{
    _BASE = 0,
    _FUNC
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_PSLS, KC_PAST, KC_PMNS,
        KC_VOLD, KC_VOLU, KC_P7, KC_P8, KC_P9, KC_PPLS,
        KC_MPRV, KC_MNXT, KC_P4, KC_P5, KC_P6, KC_PPLS,
        KC_LEFT, KC_RGHT, KC_P1, KC_P2, KC_P3, KC_PENT,
        KC_TRNS, KC_TRNS, KC_P0, KC_P0, KC_PDOT, KC_PENT),

    [_FUNC] = LAYOUT(
        ___, ___, ___,
        ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___,
        ___, ___, ___, ___, ___, ___),
};

oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    oled_set_brightness(255);
    return OLED_ROTATION_180;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    if (record->event.pressed)
    {
        enum tap_sides dir = Left;
        if (record->event.key.col >= KEYBOARD_CENTER_COL)
            dir = Right;
        bongo_tap(dir);
    }

    return true;
}

bool oled_task_user(void)
{
    draw_bongo(false);

    return false;
}
