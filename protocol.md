# Intelli-Chilli Binary Device Protocol

Protocol between the Argo Gateway and the Slow Cooker device.

0  - Ack: Acknowledgement packet
1  - Ping: status ping.
2  - Set Cook Delay: Sets the number of minutes to start the cook in.
3  - Set Cook Time: Sets the number of minutes the cook should run for.
4  - Set Cook Temp: Temperature set point.
5  - Start Cook: 
6  - Turn Off: turn off.
7  - Turn On: turn on. 
8  - Reset: Resets device.
9  - Request State: Returns full state.
10 - Status Event: 


## Base Protocol

Length - 8bit
Command Type - 8bit
Payload - [0-200]
CRC - 8bit

## Commands

### Ping

Ping sent to the device, device responds with an ack.

Payload: None

### Set Clock Delay

Sets the number of minutes to delay the start of the cooking.
Can range from 0 to 720 (12 hours).

Default: 0
Payload: Delay - 16bit 

### Sets Cook Time

Sets the number of minutes the cook should run for, then turn off.
Can range from 0 to 720 (12hours).

Payload: Time - 16bit

### Sets Cook Temp

Sets the cook temp in celcius. From ?? to ??.

Payload: Temp - 8bit

### Start Cook

Using the parameters above starts the cook.

Payload : None

### Turn Off

Turn the cooking off and reset the devices settings. If
already off does nothing. No commands besides turn on
are accepted.

Payload: None

### Turn On

Turns on the device, allows communication to begin. If already on
does nothing.

Payload: None

### Reset

Resets the device completely, equivalent to unplugging the device and plugging
it in.

Payload: None

### Request State

Returns all current settings from device.

Payload: None
Return Payload: Delay Time - 16bit (time in minutes)
                Delay Time Left - 16bit (time in minutes)
                Cook Time - 16bit (time in minutes)
                Cook Time Left - 16bit (time in minutes)
                Cook Temp - 8bit (temp in celcius)
                Current Temp - 8bit (temp in celcius)
                Lid State - 8bit (0=closed,1=open)

### Status Event

Event status, this is only sent from the device.

Payload: Status Code - 8bit
         Status Payload - []

#### Events

0: Powered On - Payload: None. When device comes online.
1: Cook Started - Payload: None. When delay ends and cook time begins.
2: Cook End - Payload: None. When cooktime ends.
3: Lid State - Payload: State - 8bit(0=closed,1=open). Sent when lid is openned or closed.






