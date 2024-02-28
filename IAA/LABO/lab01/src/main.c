/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2019 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * hello_world.c - App layer application of a simple hello world debug print every
 *   2 seconds.
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "FreeRTOS.h"
#include "task.h"

#define DEBUG_MODULE "HELLOWORLD"
#include "debug.h"

#include "commander.h"
static void setVelocitySetpoint(setpoint_t *setpoint, float vx,
float vy, float z, float yawrate) {
setpoint->mode.z = modeAbs;
setpoint->position.z = z;
setpoint->mode.yaw = modeVelocity;
setpoint->attitudeRate.yaw = yawrate;
setpoint->mode.x = modeVelocity;
setpoint->mode.y = modeVelocity;
setpoint->velocity.x = vx;
setpoint->velocity.y = vy;
setpoint->velocity_body = true;
}
void appMain() {
setpoint_t s;
setVelocitySetpoint(&s, 0, 0, 0.5, 0);
// wait for the initialization to complete
vTaskDelay(M2T(5000));
while (1) {
commanderSetSetpoint(&s, 3);
vTaskDelay(M2T(10));
}
}