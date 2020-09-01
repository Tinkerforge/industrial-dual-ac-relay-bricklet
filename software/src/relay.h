/* industrial-dual-ac-relay-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * relay.h: Relay driver
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef RELAY_H
#define RELAY_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/utility/led_flicker.h"

#define RELAY_NUM 2

typedef struct {
	bool monoflop_callback[RELAY_NUM];
	bool monoflop_running[RELAY_NUM];
	uint32_t monoflop_time[RELAY_NUM];
	uint32_t monoflop_start[RELAY_NUM];

	uint8_t channel_led_config[RELAY_NUM];
	LEDFlickerState channel_led_flicker_state[RELAY_NUM];
} Relay;

extern Relay relay;

bool relay_get_value(const uint8_t channel);
void relay_set_value(const uint8_t channel, const bool value);

void relay_tick(void);
void relay_init(void);

#endif
