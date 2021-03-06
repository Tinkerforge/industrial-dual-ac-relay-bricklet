Imports System
Imports System.Threading
Imports Tinkerforge

Module ExampleSimple
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Industrial Dual AC Relay Bricklet

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim idacr As New BrickletIndustrialDualACRelay(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Turn relays alternating on/off 10 times with 1 second delay
        Dim i As Integer
        For i = 0 To 4
            Thread.Sleep(1000)
            idacr.SetValue(True, False)
            Thread.Sleep(1000)
            idacr.SetValue(False, True)
        Next i

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
