// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_industrial_dual_ac_relay.h"

#define UID "XYZ" // Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

void check(int rc, const char* msg);

void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);


static TF_IndustrialDualACRelay idacr;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_industrial_dual_ac_relay_create(&idacr, UID, hal), "create device object");

	// Turn relays alternating on/off 10 times with 1 second delay
	int i;
	for (i = 0; i < 5; ++i) {
		tf_hal_sleep_us(hal, 1000 * 1000);
		check(tf_industrial_dual_ac_relay_set_value(&idacr, true, false), "call set_value");
		tf_hal_sleep_us(hal, 1000 * 1000);
		check(tf_industrial_dual_ac_relay_set_value(&idacr, false, true), "call set_value");
	}
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
