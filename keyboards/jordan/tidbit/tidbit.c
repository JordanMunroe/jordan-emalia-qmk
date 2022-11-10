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

#include QMK_KEYBOARD_H

bool numlock_set = false;

// Use Bit-C LED to show NUM LOCK status
bool led_update_kb(led_t led_state)
{
    bool res = led_update_user(led_state);
    if (res)
    {
        set_bitc_LED(led_state.num_lock ? LED_DIM : LED_OFF);
    }
    return res;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record)
{
    process_record_remote_kb(keycode, record);
    if (!process_record_user(keycode, record))
        return false;

    // Get the current NLCK status & set if not set.
    // Only do this once, in case user has a NLCK key
    // and wants to disable it later on.
    if (!numlock_set && record->event.pressed)
    {
        led_t led_state = host_keyboard_led_state();
        if (!led_state.num_lock)
        {
            register_code(KC_NLCK);
        }
        numlock_set = true;
    }

    switch (keycode)
    {
    case RESET:
        if (record->event.pressed)
        {
            set_bitc_LED(LED_DIM);
// rgblight_disable_noeeprom();
#ifdef OLED_ENABLE
            oled_off();
#endif
            bootloader_jump(); // jump to bootloader
        }
        return false;

    default:
        break;
    }

    return true;
}

void matrix_init_kb(void)
{
    set_bitc_LED(LED_OFF);
    matrix_init_remote_kb();
    matrix_init_user();
}

void matrix_scan_kb(void)
{
    matrix_scan_remote_kb();
    matrix_scan_user();
}

void keyboard_pre_init_kb(void)
{
    rgblight_disable();
}
