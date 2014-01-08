var util = require('util')
  , dgram = require('dgram')
  , events = require('events')
  , extend = require('extend')
  , protocol = require('./protocol');

function _word(n,i,s){
  return ((n >> (8*(s-i))) & 0xff); // cook time msb
}


module.exports = Client;
function Client(opts){
  
  this.options = {
    address : '127.0.0.1',
    port : 3000,
    timeout : 1000
  };
  extend(this.options,opts);

  events.EventEmitter.call(this);

  this._client = dgram.createSocket("udp4");
  this._client.on('error',this.emit.bind(this));
}
util.inherits(Client, events.EventEmitter);

Client.prototype.close = function(buffer,callback) {
  this._client.close();
};

Client.prototype._send = function(msg,callback) {

  var b = protocol.serialize(msg); 
  if(b === null)
    return callback(new Error('Bad message'));

  var self = this;
  
  this._sendBuffer(b,function onSendFinish(){
    self._recvAck(callback,b);
  });
};

Client.prototype._sendBuffer = function(buffer,callback) {
  this._client.send(buffer, 0, buffer.length, this.options.port, this.options.address,callback);
};

Client.prototype._recvAck = function(callback,b) {

  var self = this;
  var to = setTimeout(onTimeout,this.options.timeout);
  var toRetry = setTimeout(onRetry,this.options.timeout/2);

  function onMessage(data,rinfo){
    clearTimeout(to);
    clearTimeout(toRetry);

    var m = protocol.parse(data);
    if(m === null)
      return callback(new Error('Failed to parse the return packet.'));

    return callback(null,m);
  }

  function onTimeout(){
    self._client.removeListener('message', onMessage);
    callback(new Error('Timeout reached when trying to communicate with end device.'));
  }

  function onRetry(){
    self._sendBuffer(b,function onSendFinish(){});
  }
  
  this._client.once("message",onMessage);
};






Client.prototype.ping = function(callback) {
  this._send({type : protocol.PacketTypes.CMD_PING},callback);
};

Client.prototype.setCookTime = function(time,callback) {
  time = parseInt(time);
  if(time < 0 || time > 1440)
    return callback(new Error('Inavlid cook time'));

  var b = new Buffer(2);
  b[0] = _word(time,1,2);
  b[1] = _word(time,2,2);
  this._send({type : protocol.PacketTypes.CMD_SET_COOK_TIME,data : b},callback);
};

Client.prototype.setCookTemp = function(tempC,callback) {
  tempC = parseInt(tempC);
  if(tempC < 0 || tempC > 120)
    return callback(new Error('Inavlid cook temp.'));

  var b = new Buffer([tempC]);
  this._send({type : protocol.PacketTypes.CMD_SET_COOK_TEMP,data : b},callback);
};

Client.prototype.startCook = function(callback) {
  this._send({type : protocol.PacketTypes.CMD_START_COOK},callback);
};

Client.prototype.stopCook = function(callback) {
  this._send({type : protocol.PacketTypes.CMD_STOP_COOK},callback);
};

Client.prototype.resetDevice = function(callback) {
  this._send({type : protocol.PacketTypes.CMD_RESET},function(){});
  callback();
};
Client.prototype.returnState = function(callback) {
  this._send({type : protocol.PacketTypes.CMD_RETURN_STATE},function(err,m){
    if(err)
      return callback(err);

    if(m.data.length < 7)
      return callback(new Error('Return packet data malformed'));

    var obj = {
      cookTime : (m.data[0] << 8) + m.data[1],
      cookTimeLeft : (m.data[2] << 8) + m.data[3],
      cookTemp : m.data[4],
      currentTemp : m.data[5],
      lidState : (m.data[6] == 0) ? 'closed' : 'opened',
      cooking : Boolean(m.data[7]),
      heaterOn : Boolean(m.data[8])
    };

    callback(err,obj);
  });
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

