# AutoArduino-Sketch
Sketch used with the [AutoArduino Android app](https://play.google.com/store/apps/details?id=com.joaomgcd.autoarduino)

This sketch provides the basis for the communication with the AutoArduino app and allows for the reading
and writing of both digital and analog pins as well as support for both stepper and servo motors. Care must
be taken in configuring the settings for the servo motor with rudimentary limits placed within the code based
on most servo motors.

Take note of the settings required to enable and use Ethernet connectivity. Bluetooth is supported and tested
with both HC-05 and HC-06 modules. Remember, the bluetooth module will likely need to be disconnected as you
load code to your Arduino as you'll likely find conflicts between the USB serial and the module.

Several examples and additional documentation can be found at [website](https://joaoapps.com/autoarduino/)
