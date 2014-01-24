var HueApi = require("node-hue-api").HueApi;
var async = require('async');

var hostname = process.env.HUE_HOST || "192.168.1.2",
    username = process.env.HUE_USER || "2c5ce9222d0363f3bd5766032a6821f";

var api = new HueApi(hostname, username);
module.exports.api = api;

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

module.exports.notification = function notification(callback){
  async.series([
      blink(0),
      delay(500),
      blink(0),
      delay(500),
      blink(0),
  ],callback);
}

module.exports.notificationOne = function notificationOne(callback){
  blink(0)(callback);
}