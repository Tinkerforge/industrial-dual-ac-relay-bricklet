/* industrial-dual-ac-relay-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"
#include "relay.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/hal/system_timer/system_timer.h"
#include "bricklib2/protocols/tfp/tfp.h"

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_SET_VALUE: return set_value(message);
		case FID_GET_VALUE: return get_value(message, response);
		case FID_SET_CHANNEL_LED_CONFIG: return set_channel_led_config(message);
		case FID_GET_CHANNEL_LED_CONFIG: return get_channel_led_config(message, response);
		case FID_SET_MONOFLOP: return set_monoflop(message);
		case FID_GET_MONOFLOP: return get_monoflop(message, response);
		case FID_SET_SELECTED_VALUE: return set_selected_value(message);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_value(const SetValue *data) {
	relay_set_value(0, data->channel0);
	relay_set_value(1, data->channel1);

	relay.monoflop_running[0] = false;
	relay.monoflop_running[1] = false;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_value(const GetValue *data, GetValue_Response *response) {
	response->header.length = sizeof(GetValue_Response);
	response->channel0      = relay_get_value(0);
	response->channel1      = relay_get_value(1);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_channel_led_config(const SetChannelLEDConfig *data) {
	if(data->channel > 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	relay.channel_led_config[data->channel] = data->config;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_channel_led_config(const GetChannelLEDConfig *data, GetChannelLEDConfig_Response *response) {
	if(data->channel > 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetChannelLEDConfig_Response);
	response->config        = relay.channel_led_config[data->channel];

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_monoflop(const SetMonoflop *data) {
	if(data->channel > 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	relay.monoflop_start[data->channel]   = system_timer_get_ms();
	relay.monoflop_time[data->channel]    = data->time;
	relay.monoflop_running[data->channel] = true;
	relay_set_value(data->channel, data->value);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_monoflop(const GetMonoflop *data, GetMonoflop_Response *response) {
	if(data->channel > 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length  = sizeof(GetMonoflop_Response);
	response->value          = relay_get_value(data->channel);
	response->time           = relay.monoflop_time[data->channel];

	if(relay.monoflop_running[data->channel]) {
		response->time_remaining = relay.monoflop_time[data->channel] - (uint32_t)(system_timer_get_ms() - relay.monoflop_start[data->channel]);
	} else {
		response->time_remaining = 0;
	}

	// Sanity check time remaining
	if(response->time_remaining > response->time) {
		response->time_remaining = 0;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_selected_value(const SetSelectedValue *data) {
	if(data->channel > 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	relay_set_value(data->channel, data->value);

	relay.monoflop_running[data->channel] = false;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

bool handle_monoflop_done_callback(void) {
	static bool is_buffered = false;
	static MonoflopDone_Callback cb;

	if(!is_buffered) {
		if(relay.monoflop_callback[0]) {
			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(MonoflopDone_Callback), FID_CALLBACK_MONOFLOP_DONE);
			cb.channel = 0;
			cb.value = relay_get_value(0);
			relay.monoflop_callback[0] = false;
		} else if(relay.monoflop_callback[1]) {
			tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(MonoflopDone_Callback), FID_CALLBACK_MONOFLOP_DONE);
			cb.channel = 1;
			cb.value = relay_get_value(1);
			relay.monoflop_callback[1] = false;
		} else {
			return false;
		}
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(MonoflopDone_Callback));
		is_buffered = false;

		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
