/* Copyright 2019 COSEYFANNITUTTI
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

#include "jordan/mysterium/mysterium.h"
#include "jordan/mysterium/config.h"
#include <math.h>
#include <stdio.h>

char velStr[64];

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = LAYOUT_tkl_ansi(KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR, KC_SLCK, KC_PAUS, KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_INS, KC_HOME, KC_PGUP, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL, KC_END, KC_PGDN, KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, KC_RGUI, MO(1), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT),

                                                              [1] = LAYOUT_tkl_ansi(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLD, KC_TRNS)};

float x = 0;
float y = 0;
float vx = 0.4;
float vy = 0.4;

float friction = 0.97;
float restitution = 0.9;
float gravity = 0.5;

float randF(float from, float to)
{
    float v = (float)rand() / (float)RAND_MAX;
    return from + (v * (to - from));
}

void kickBall(void)
{
    float ang = randF(-135, -45);
    float pow = randF(3, 30);
    vx += cos(ang * 3.14159 / 180) * pow;
    vy -= sin(ang * 3.14159 / 180) * pow;
}

void clear_oled(void)
{
    // oled_clear(); // Doesn't work properly
    for (int I = 0; I < (128 * 64) / 8; I++)
    {
        oled_write_raw_byte(0, I);
    }
}

void update(float dt)
{
    bool grounded = false;
    x += vx * dt;
    y += vy * dt;

    if (x < 1)
    {
        x += 1-x;
        vx = -vx * restitution;
        vy *= friction;
    }

    if (x >= 126)
    {
        x -= x-126;
        vx = -vx * restitution;
        vy *= friction;
    }

    if (y < 1)
    {
        y += 1-y;
        vy = -vy * restitution;
        vx *= friction;
    }

    if (y >= 62)
    {
        y -= y-62;
        vy = -vy * restitution;
        vx *= friction;
        grounded = true;
    }

    if (grounded)
    {
        if (fabs(vx) < 0.1 && fabs(vy) <= gravity+0.1)
        {
            kickBall();
        }
    }
    vy += gravity * dt;
}

void renderBall(void)
{
    // if(x > 1 && x < 127 && y > 1 && y < 63
    oled_write_pixel((uint8_t)x, (uint8_t)y, true);
    oled_write_pixel((uint8_t)x - 1, (uint8_t)y, true);
    oled_write_pixel((uint8_t)x + 1, (uint8_t)y, true);
    oled_write_pixel((uint8_t)x, (uint8_t)y - 1, true);
    oled_write_pixel((uint8_t)x, (uint8_t)y + 1, true);
}

void render(float dt)
{
    clear_oled();
    //oled_clear();

    renderBall();

    //int frame = (uint8_t)((timer_read() / 1000) % 4);

    // dtostrf( my_float, 16, 2, "%f\0" )
    // oled_write(dtostrf(fabs(vx) + (fabs(vy) - gravity), 3, 2, "%f"), false);
    //oled_write(dtostrf(dt, 3, 2, "%f"), false);
    // oled_write(dtostrf(frame, 1, 0, "%f"), false);
    // oled_render();
}

uint16_t frame_timer;
bool oled_task_user(void)
{
    // Host Keyboard Layer Status
    // oled_write_P(PSTR("Hello, World"), false);

    // timer_read()


    float dt = timer_elapsed(frame_timer) / OLED_UPDATE_INTERVAL;
    frame_timer = timer_read();
    update(dt);
    render(dt);
    // oled_write_raw_P(BAG_ANIM[0], false);
    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    frame_timer = timer_read();;
    kickBall();
    return OLED_ROTATION_180;
}
