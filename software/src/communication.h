/* industrial-dual-ac-relay-bricklet
 * Copyright (C) 2020 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants


// Function and callback IDs and structs
#define FID_SET_VALUE 1
#define FID_GET_VALUE 2
#define FID_SET_MONOFLOP 3
#define FID_GET_MONOFLOP 4
#define FID_SET_SELECTED_VALUE 6

#define FID_CALLBACK_MONOFLOP_DONE 5

typedef struct {
	TFPMessageHeader header;
	bool channel0;
	bool channel1;
} __attribute__((__packed__)) SetValue;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetValue;

typedef struct {
	TFPMessageHeader header;
	bool channel0;
	bool channel1;
} __attribute__((__packed__)) GetValue_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
	uint32_t time;
} __attribute__((__packed__)) SetMonoflop;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetMonoflop;

typedef struct {
	TFPMessageHeader header;
	bool value;
	uint32_t time;
	uint32_t time_remaining;
} __attribute__((__packed__)) GetMonoflop_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
} __attribute__((__packed__)) MonoflopDone_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
} __attribute__((__packed__)) SetSelectedValue;


// Function prototypes
BootloaderHandleMessageResponse set_value(const SetValue *data);
BootloaderHandleMessageResponse get_value(const GetValue *data, GetValue_Response *response);
BootloaderHandleMessageResponse set_monoflop(const SetMonoflop *data);
BootloaderHandleMessageResponse get_monoflop(const GetMonoflop *data, GetMonoflop_Response *response);
BootloaderHandleMessageResponse set_selected_value(const SetSelectedValue *data);

// Callbacks
bool handle_monoflop_done_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 1
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_monoflop_done_callback, \


#endif
