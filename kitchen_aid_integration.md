# Integration with the Kitchen Aid Slow Cooker

Taking apart the the cooker was fairly straight forward. The only
tricky part was removing the rivited washers that hold the plastic base
to the aliminum body. We drilled out the rivit allowing us to take it apart.

There are two circuit boards within the cooker. One which is exposed in the 
plastic base contains the power and relay section of the design. The other
is underneath the buttons on the front which houses the brains.

In our design we are uninterested in the brains part because we will be replacing 
it with an Arduino. The power section is pretty straight forward.

1. 120VAC goes into the device from the wall plug.
2. 120VAC makes a T, one leg going to a 120VAC Transformer which takes it down to 10VAC
and the other leg going into the PCB.
  a. The hot leg of the path that goes into the PCB is passed directly to the hot leg of the heater.
  b. The netural leg of the path that goes into the PCB goes into the relay and is switched by the circuit.
This turns the heater on and off.
3. The 10AC from goes into a 5v bridge rectafier/cap then linear regulator 
and some other circuitry that creates a nice clean 5VDC for the brains on the
circuit board.
4. The brains the controls the relay by using PIN 3 on the connector.


