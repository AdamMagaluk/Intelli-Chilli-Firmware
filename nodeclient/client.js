var util = require('util')
  , path = require('path')
  , url = require('url')
  , dgram = require("dgram")
  , request = require('request')
  , events = require('events')
  , extend = require('extend')
  , protocol = require('./protocol');


var ChilliEvents = {};
ChilliEvents[protocol.EventTypes.EVENT_POWERED_ON] = 'powered';
ChilliEvents[protocol.EventTypes.EVENT_COOK_END] = 'cookend';
ChilliEvents[protocol.EventTypes.EVENT_LID_OPENED] = 'lidopened';
ChilliEvents[protocol.EventTypes.EVENT_LID_CLOSED] = 'lidclosed';

function _word(n,i,s){
  return ((n >> (8*(s-i))) & 0xff); // cook time msb
}

var CookTemps = {
  warm : 15,
  low : 30,
  medium : 45,
  high : 65
};

module.exports = Client;
function Client(opts){
  
  this.options = {
    address : 'arduino.local'
  };

  this.server = dgram.createSocket("udp4");
  this.server.on("message",this._onMessage.bind(this));
  this.server.bind(3000);
  this.server.on('error',function(err){
    console.error(err);
  });

  extend(this.options,opts);
  events.EventEmitter.call(this);
}

util.inherits(Client, events.EventEmitter);

//legacy support
Client.prototype.close = function() {}

Client.prototype._request = function(resource,val,callback) {
  var pathname = path.join('/arduino',resource);
  
  if(typeof val === 'function')
    callback = val;
  else(val !== undefined)
    pathname = path.join(pathname,''+val);

  var u = url.format({
    protocol : 'http',
    host : this.options.address,
    pathname : pathname
  });
  
  request(u,function(err,res,body){
    if(err)
      return callback(err);
    
    try{
      var json = JSON.parse(body);
      if(res.statusCode !== 200)
        return callback(json);
      return callback(err,json);
    }catch(err){
      return callback(err);
    }
  });
};

Client.prototype.ping = function(callback) {
  this._request('/ping',callback);
};

Client.prototype.setCookTime = function(time,callback) {
  time = parseInt(time);
  if(time < 0 || time > 1440)
    return callback(new Error('Inavlid cook time'));

  this._request('/time',time,callback);
};

Client.prototype.setCookTemp = function(temp,callback) {
  var tempVal = CookTemps[temp];
  if(tempVal === undefined)
    return callback(new Error('Inavlid cook temp.'));

  this._request('/temp',tempVal,callback);
};

Client.prototype.startCook = function(callback) {
  this._request('/start',callback);
};

Client.prototype.stopCook = function(callback) {
  this._request('/stop',callback);
};

Client.prototype.resetDevice = function(callback) {
  this._request('/reset',callback);
};
Client.prototype.returnState = function(callback) {
  this._request('/state',function(err,state){
    if(err)
      return callback(err);

    state.cookTimeRange  = [0,(24*60)];
    state.cookTempRange  = ['warm','low','medium','high'];

    if(state.cooking === 0)
      state.cooking = false;
    else
      state.cooking = true;

    if(state.heaterOn === 0)
      state.heaterOn = false;
    else
      state.heaterOn = true;

    if(state.lidState === 1)
      state.lidState = 'closed';
    else
      state.lidState = 'opened';

    callback(err,state);
  });
};

Client.prototype._onMessage = function(buffer, rinfo) {
  try{
    var msg = JSON.parse(buffer);
  }catch(err){
    server.emit('error',err,buffer,rinfo);
    return;
  };

  if(msg.event === undefined)
    return;

  if(ChilliEvents[msg.event] !== undefined)
    this.emit(ChilliEvents[msg.event],msg,rinfo);  
};
