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

#include "communication.h"
#include "configs/config_relay.h"

#include "bricklib2/hal/system_timer/system_timer.h"

#include "xmc_gpio.h"

Relay relay;

static inline uint32_t XMC_GPIO_GetOutputLevel(XMC_GPIO_PORT_t *const port, const uint8_t pin) {
  return (((port->OUT) >> pin) & 0x1U);
}

void relay_set_led(const uint8_t channel, const bool state) {
	switch(channel) {
		case 0: state ? XMC_GPIO_SetOutputLow(RELAY0_LED_PIN) : XMC_GPIO_SetOutputHigh(RELAY0_LED_PIN); break;
		case 1: state ? XMC_GPIO_SetOutputLow(RELAY1_LED_PIN) : XMC_GPIO_SetOutputHigh(RELAY1_LED_PIN); break;
		default: break; // Error?
	}
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

void relay_tick(void) {
	for(uint8_t i = 0; i < RELAY_NUM; i++) {
		if(relay.monoflop_running[i]) {
			if(system_timer_is_time_elapsed_ms(relay.monoflop_start[i], relay.monoflop_time[i])) {
				relay_set_value(i, !relay_get_value(i));
				relay.monoflop_running[i] = false;
				relay.monoflop_callback[i] = true;
			}
		}

		// Manage channel LEDs
		switch(relay.channel_led_config[i]) {
			case INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_OFF: {
				relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_OFF;
				relay_set_led(i, false);

				break;
			}

			case INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_ON: {
				relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_ON;
				relay_set_led(i, true);

				break;
			}

			case INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_SHOW_HEARTBEAT: {
				relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_HEARTBEAT;

				if(i == 0) {
					led_flicker_tick(&relay.channel_led_flicker_state[i], system_timer_get_ms(), RELAY0_LED_PIN);
				} else {
					led_flicker_tick(&relay.channel_led_flicker_state[i], system_timer_get_ms(), RELAY1_LED_PIN);
				}

				break;
			}

			case INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS: {
				relay.channel_led_flicker_state[i].config = LED_FLICKER_CONFIG_OFF;
				relay_set_led(i, relay_get_value(i));

				break;
			}

			default: break;
		}
	}
}

void relay_init(void) {
	const XMC_GPIO_CONFIG_t relay_gpio_config = {
		.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
	};
	XMC_GPIO_Init(RELAY0_PIN, &relay_gpio_config);
	XMC_GPIO_Init(RELAY1_PIN, &relay_gpio_config);

	const XMC_GPIO_CONFIG_t relay_led_gpio_config = {
		.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
		.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH,
	};
	XMC_GPIO_Init(RELAY0_LED_PIN, &relay_led_gpio_config);
	XMC_GPIO_Init(RELAY1_LED_PIN, &relay_led_gpio_config);

	relay.monoflop_callback[0] = false;
	relay.monoflop_callback[1] = false;
	relay.monoflop_running[0] = false;
	relay.monoflop_running[1] = false;
	relay.monoflop_time[0] = 0;
	relay.monoflop_time[1] = 0;
	relay.monoflop_start[0] = 0;
	relay.monoflop_start[1] = 0;

	relay.channel_led_config[0] = INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;
	relay.channel_led_config[1] = INDUSTRIAL_DUAL_AC_RELAY_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS;

	relay.channel_led_flicker_state[0].config = LED_FLICKER_CONFIG_OFF;
	relay.channel_led_flicker_state[1].config = LED_FLICKER_CONFIG_OFF;
}
