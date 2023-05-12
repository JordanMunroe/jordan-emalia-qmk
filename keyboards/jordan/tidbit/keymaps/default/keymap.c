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

#include "print.h"
#include <stdio.h>
#include "jordan/tidbit/tidbit.h"
#include "jordan/tidbit/config.h"
#include "bongo.h"
#define KEYBOARD_CENTER_COL 4

uint16_t click_timer = 0;
bool enc_pressed = false;
enum layers
{
    _BASE = 0,
    _FUNC
};
enum custom_keycodes {
    QMK_ENC = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        QMK_ENC, KC_PAST, KC_PMNS,
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

bool oled_task_user(void)
{
    rgblight_setrgb(0, 255, 0);
    draw_bongo(false);

    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    rgblight_setrgb(255, 0, 255);
    oled_set_brightness(255);
    oled_on();
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

    if(keycode == QMK_ENC)
    {
        enc_pressed = record->event.pressed;
        if (enc_pressed)
        {
            click_timer = timer_read();
            return false;
        }
        else if(timer_elapsed(click_timer) < 500)
        {
            rgblight_step();
            return false;
        }
    }

    return true;
}


void keyboard_post_init_user(void)
{
  debug_enable=true;
  debug_matrix=true;
    //rgblight_setrgb(110, 0, 110);
}

bool encoder_update_user(uint8_t index, bool clockwise)
{
    if(index == 0)
    {
        dprintf("clockwise: %s\n", clockwise == true ? "yes" : "no");
        dprintf("enc_pressed: %s\n", enc_pressed == true ? "yes" : "no");
        int hue = rgblight_get_hue();
        int sat = rgblight_get_sat();

        if (clockwise)
        {
            if(enc_pressed)
            {
                sat += 3;
            }
            else
            {
                hue += 1;
            }
        }
        else
        {
            if(enc_pressed)
            {
                sat -= 3;
            }
            else
            {
                hue -= 1;
            }
        }
        if(hue < 0) hue += 360;
        if(hue > 360) hue -= 360;
        if(sat < 0) sat = 0;
        if(sat > 255) sat = 255;

        rgblight_sethsv(hue, sat, rgblight_get_val());
        return false;
    }

    return true;
}
