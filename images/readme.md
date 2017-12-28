## Arduino Sensor pcb board

I have powered the Arduino from a 5 volt supply and not from the Raspberry Pi's gpio as it may overload the Pi's current rating.

I have powered the 10dof chip directly from the arduino it should draw no more than 20 milliamps so should be fine.

As the output from the sensor is 5v I have used  a voltage divider circuit on all input pins on the Arduino.
The resistors I used are (r1= 1K ohms) and (r2 = 2.2k ohms)   r1 is connected to the sensor, r2 to ground and the point where they connect to the Arduino.
The output from this is 3.438v and is within speck of the arduino, but you could use (r1 = 13.7kOhms and r2 = 26.7KOhms) which would give you 3.304 volts and is probably a  better solution.
