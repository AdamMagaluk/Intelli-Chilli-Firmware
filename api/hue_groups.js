//2c5ce9222d0363f3bd5766032a6821f
//192.168.1.2
var HueApi = require("node-hue-api").HueApi;
var async = require('async');

var displayResult = function(result) {
    console.log(JSON.stringify(result, null, 2));
};

var hostname = "192.168.1.2",
    username = "2c5ce9222d0363f3bd5766032a6821f",
    api;

api = new HueApi(hostname, username);

function blink(group){  
  return function(callback){
    api.setGroupLightState(group, {alert : "select"},callback);
  }
}

function delay(time){
  return function(callback){
    setTimeout(callback,time);  
  }
}

async.series([
    blink(0),
    delay(500),
    blink(0),
    delay(500),
],function(err){
  console.log(err);
});