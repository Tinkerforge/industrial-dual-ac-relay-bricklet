#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

# Turn relays alternating on/off 10 times with 1 second delay
for i in 0 1 2 3 4; do
	sleep 1
	tinkerforge call industrial-dual-ac-relay-bricklet $uid set-value true false
	sleep 1
	tinkerforge call industrial-dual-ac-relay-bricklet $uid set-value false true
done
