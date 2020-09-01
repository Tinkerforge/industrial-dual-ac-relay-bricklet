/* industrial-dual-ac-relay-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * relay.c: Relay driver
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

#include "relay.h"

#include "configs/config_relay.h"

#include "bricklib2/hal/system_timer/system_timer.h"

#include "xmc_gpio.h"

static inline uint32_t XMC_GPIO_GetOutputLevel(XMC_GPIO_PORT_t *const port, const uint8_t pin) {
  return (((port->OUT) >> pin) & 0x1U);
}

bool relay_get_value(const uint8_t channel) {
	switch(channel) {
		case 0: return XMC_GPIO_GetOutputLevel(RELAY0_PIN);
		case 1: return XMC_GPIO_GetOutputLevel(RELAY1_PIN);
		default: break; // Error?
	}

	return 0;
}

void relay_set_value(const uint8_t channel, const bool state) {
	switch(channel) {
		case 0: state ? XMC_GPIO_SetOutputHigh(RELAY0_PIN) : XMC_GPIO_SetOutputLow(RELAY0_PIN); break;
		case 1: state ? XMC_GPIO_SetOutputHigh(RELAY1_PIN) : XMC_GPIO_SetOutputLow(RELAY1_PIN); break;
		default: break; // Error?
	}
}

void relay_tick(Relay *relay) {
	for(uint8_t i = 0; i < RELAY_NUM; i++) {
		if(relay->monoflop_running[i]) {
			if(system_timer_is_time_elapsed_ms(relay->monoflop_start[i], relay->monoflop_time[i])) {
				relay_set_value(i, !relay_get_value(i));
				relay->monoflop_running[i] = false;
				relay->monoflop_callback[i] = true;
			}
		}
	}
}

void relay_init(Relay *relay) {
	const XMC_GPIO_CONFIG_t relay_gpio_config = {
		.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};

	XMC_GPIO_Init(RELAY0_PIN, &relay_gpio_config);
	XMC_GPIO_Init(RELAY1_PIN, &relay_gpio_config);

	relay->monoflop_callback[0] = false;
	relay->monoflop_callback[1] = false;
	relay->monoflop_running[0] = false;
	relay->monoflop_running[1] = false;
	relay->monoflop_time[0] = 0;
	relay->monoflop_time[1] = 0;
	relay->monoflop_start[0] = 0;
	relay->monoflop_start[1] = 0;
}
