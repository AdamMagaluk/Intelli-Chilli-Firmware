var util = require('util')
  , path = require('path')
  , url = require('url')
  , request = require('request')
  , events = require('events')
  , extend = require('extend')
  , protocol = require('./protocol');

function _word(n,i,s){
  return ((n >> (8*(s-i))) & 0xff); // cook time msb
}


module.exports = Client;
function Client(opts){
  
  this.options = {
    address : 'arduino.local'
  };

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

Client.prototype.setCookTemp = function(tempC,callback) {
  tempC = parseInt(tempC);
  if(tempC < 0 || tempC > 120)
    return callback(new Error('Inavlid cook temp.'));

  this._request('/temp',tempC,callback);
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
  this._request('/state',callback);
};


/*
function sendPacket(done){

  client.once("error", function (err) {
    console.log("server error:\n" + err.stack);
    client.close();
  });

  var buf = new Buffer([0x03,0x07,0xEE]);
  client.send(buf, 0, buf.length, 3000, "10.0.1.24", function(err, bytes) {
    client.once("message", function (msg, rinfo) {
      console.log(msg);
      client.close();
    });
  });
}

sendPacket();
*/

