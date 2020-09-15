#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletIndustrialDualACRelay;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $idacr = Tinkerforge::BrickletIndustrialDualACRelay->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Turn relays alternating on/off 10 times with 1 second delay
for (my $i = 0; $i < 5; $i++)
{
    sleep(1);
    $idacr->set_value(1, 0);
    sleep(1);
    $idacr->set_value(0, 1);
}

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
