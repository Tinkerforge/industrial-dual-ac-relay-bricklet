using System;
using System.Threading;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletIndustrialDualACRelay idacr =
		  new BrickletIndustrialDualACRelay(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Turn relays alternating on/off 10 times with 1 second delay
		for(int i = 0; i < 5; i++)
		{
			Thread.Sleep(1000);
			idacr.SetValue(true, false);
			Thread.Sleep(1000);
			idacr.SetValue(false, true);
		}

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
