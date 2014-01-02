# Intelli-Chilli Node.js Client

## Example

```js

var Client = require('intelli-chilli-client');

var c = new Client({address : '192.168.1.33'});
c.returnState(function(err,state){
  //handle err

  console.log('The current temp is: ',state.currentTemp);
  c.close();
});

```


## Install

```
npm install intelli-chilli-client
```

## Methods

```js
var Client = require('intelli-chilli-client');
```

### Client(opts)

Client class must be supplied with a options object.

`opts.address` Ip address of device. Default : 127.0.0.1

`opts.port` Port to communicate with device. Default: 3000

`opts.timeout` Default timeout in ms when waiting for acks. Default 500

### Client.close()

Closes underlying dgram socket.

### Client.setCookTime(time,callback)

Sets the number of minutes the device should run for before turning off.
By default the time is set to zero meaning it will run until manually turned off.

`time` must be between 0 and 1440 (24hours).

**Note:**If currently cooking this command resets the amount of time on the clock. Therefore
it run for the amount of time given when the command is received.

### Client.setCookTemp(tempC,callback)

Sets the desired cook temperature in celsius.

`tempC` is between 0 and 120c.

### Client.startCook(callback)

Start the cook with the current settings. If cook time and cook temp are 
not set they use their defaults.

### Client.stopCook(callback)

Stops the current cook. Turns off heater. And resets cook time and temp to defaults.

### Client.resetDevice(callback)

Sends a reset command to the device to reboot it.

### Client.returnState(callback)

Returns the current state of the device.

Callback is a function(err,state) state is defined as:

`state.cookTime` The amount of time the device was set to cook for. 0 is indefinite.

`state.cookTimeLeft` The amount of time remaining in the cook.

`state.cookTemp` The desired cook temperature.

`state.currentTemp` The current temperature.

`state.lidState` 0=Lid is closed. 1=Lid is opened.

`state.cooking` True or false if the device is currently cooking.

`state.heaterOn` True or false if heater is currently powered on.

### Client.ping(callback)

Sends a ping packet to device and waits for an ack.
