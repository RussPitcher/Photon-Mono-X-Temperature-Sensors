# Overview

This project adds three DS18B20 temperature sensors to an Anycubic Photon Mono X.  The three transistor-sized
sensors are simply taped in place with 1" kapton tape and routed up to the top of the Z-rail where the circuit
board is mounted. The breadboard circuit takes 12v power from the heater mounted on the other side of the rail
(https://www.thingiverse.com/thing:3442165). The 12v supply is stepped down to 3.3v for the ESP-01 using a small
buck converter. The sensors simply get connected to 3.3v, GND and GPIO2 with all sensors sharing common connections.
A 4.7k resistor should be placed between 3.3v and the data line to GPIO2 to pull up the voltage between reads of the sensors.

The only thing specific to that printer is the heater/sensor mount so the rest can be used anywhere.

A simple web page has also been included that can run on a 'real' web server to take the data from the sensors on the
ESP01 and present it nicely without putting any stress on such a small device.  Probably overkill, but I have a Synology
NAS sitting around and thought I may as well use it.


I currently have an odd intermittent problem where the sensors work just fine most of the time and then one or more of them
go AWOL.  I'm really not sure what's causing the issue and it's not consistent.  I think there may be bad crimps on the JST
connectors when things warm up so I might simply solder the wires to the board directly and see if that changes anything.