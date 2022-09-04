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

#define NUM_BALLS 6

char velStr[64];

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = LAYOUT_tkl_ansi(KC_ESC, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR, KC_SLCK, KC_PAUS, KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_INS, KC_HOME, KC_PGUP, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL, KC_END, KC_PGDN, KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, KC_RGUI, MO(1), KC_RCTL, KC_LEFT, KC_DOWN, KC_RIGHT),

                                                              [1] = LAYOUT_tkl_ansi(KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_VOLD, KC_TRNS)};


typedef struct
{
    float x;
    float y;
    float vX;
    float vY;
}Ball;

Ball balls[NUM_BALLS];

float friction = 0.94;
float restitution = 0.9;
float gravity = 0.5;

float randF(float from, float to)
{
    float v = (float)rand() / (float)RAND_MAX;
    return from + (v * (to - from));
}

void kickBall(Ball *ball)
{
    float ang = randF(-135, -45);
    float pow = randF(3, 10);
    ball->vX += cos(ang * 3.14159 / 180) * pow;
    ball->vY -= sin(ang * 3.14159 / 180) * pow;
}

void clear_oled(void)
{
    // oled_clear(); // Doesn't work properly
    for (int I = 0; I < (128 * 64) / 8; I++)
    {
        oled_write_raw_byte(0, I);
    }
}

void updateBall(Ball *ball, float dt)
{
    bool grounded = false;
    ball->x += ball->vX * dt;
    ball->y += ball->vY * dt;

    if (ball->x < 1)
    {
        ball->x += 1-ball->x;
        ball->vX = -ball->vX * restitution;
        ball->vY *= friction;
    }

    if (ball->x >= 126)
    {
        ball->x -= ball->x-126;
        ball->vX = -ball->vX * restitution;
        ball->vY *= friction;
    }

    if (ball->y < 1)
    {
        ball->y += 1-ball->y;
        ball->vY = -ball->vY * restitution;
        ball->vX *= friction;
    }

    if (ball->y >= 62)
    {
        ball->y -= ball->y-62;
        ball->vY = -ball->vY * restitution;
        ball->vX *= friction;
        grounded = true;
    }

    if (grounded)
    {
        if (fabs(ball->vX) < 0.1 && fabs(ball->vY) <= gravity+0.1)
        {
            kickBall(ball);
        }
    }
    ball->vY += gravity * dt;
}

void update(float dt)
{
    for(int I=0; I < NUM_BALLS; I++)
    {
        updateBall(&balls[I], dt);
    }
}

void eraseBall(Ball ball)
{
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y, false);
    oled_write_pixel((uint8_t)ball.x - 1, (uint8_t)ball.y, false);
    oled_write_pixel((uint8_t)ball.x + 1, (uint8_t)ball.y, false);
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y - 1, false);
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y + 1, false);
}

void renderBall(Ball ball)
{
    // if(x > 1 && x < 127 && y > 1 && y < 63
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y, true);
    oled_write_pixel((uint8_t)ball.x - 1, (uint8_t)ball.y, true);
    oled_write_pixel((uint8_t)ball.x + 1, (uint8_t)ball.y, true);
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y - 1, true);
    oled_write_pixel((uint8_t)ball.x, (uint8_t)ball.y + 1, true);
}

void eraseBalls(void)
{
    for(int I=0; I < NUM_BALLS; I++)
    {
        eraseBall(balls[I]);
    }
}

void renderBalls(void)
{
    for(int I=0; I < NUM_BALLS; I++)
    {
        renderBall(balls[I]);
    }
}

void render(float dt)
{
    //clear_oled();
    //oled_clear();

    renderBalls();

    //int frame = (uint8_t)((timer_read() / 1000) % 4);

    // dtostrf( my_float, 16, 2, "%f\0" )
    // oled_write(dtostrf(fabs(vx) + (fabs(vy) - gravity), 3, 2, "%f"), false);
    //oled_write(dtostrf(dt, 3, 2, "%f"), false);
    // oled_write(dtostrf(frame, 1, 0, "%f"), false);
    // oled_render();
}

uint16_t frame_timer;
bool initialized = false;
bool oled_task_user(void)
{
    // Host Keyboard Layer Status
    // oled_write_P(PSTR("Hello, World"), false);

    // timer_read()

    if(!initialized)
    {
        for(int I=0; I < NUM_BALLS; I++)
        {
            balls[I].x = randF(0, 128);
            balls[I].y = randF(0, 64);
            balls[I].vX = 0;
            balls[I].vY = 0;
            kickBall(&balls[I]);
        }
        initialized = true;
    }

    float dt = timer_elapsed(frame_timer) / OLED_UPDATE_INTERVAL;
    frame_timer = timer_read();
    eraseBalls();
    update(dt);
    render(dt);
    // oled_write_raw_P(BAG_ANIM[0], false);
    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation)
{
    frame_timer = timer_read();;
    //kickBall();
    return OLED_ROTATION_180;
}
