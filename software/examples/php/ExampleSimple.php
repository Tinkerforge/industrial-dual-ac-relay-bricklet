<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletIndustrialDualACRelay.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletIndustrialDualACRelay;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

$ipcon = new IPConnection(); // Create IP connection
$idacr = new BrickletIndustrialDualACRelay(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Turn relays alternating on/off 10 times with 1 second delay
for($i = 0; $i < 5; $i++) {
    sleep(1);
    $idacr->setValue(TRUE, FALSE);
    sleep(1);
    $idacr->setValue(FALSE, TRUE);
}

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
