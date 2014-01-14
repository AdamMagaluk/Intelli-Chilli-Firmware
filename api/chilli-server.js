// udp server for capturing events from the crock pot.
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var protocol = require('intelli-chilli-client/protocol');

var events = {};
events[protocol.EventTypes.EVENT_POWERED_ON] = 'powered';
events[protocol.EventTypes.EVENT_COOK_END] = 'cookend';
events[protocol.EventTypes.EVENT_LID_OPENED] = 'lidopened';
events[protocol.EventTypes.EVENT_LID_CLOSED] = 'lidclosed';

server.on("message", function (buffer, rinfo) {
  try{
    var msg = JSON.parse(buffer);
  }catch(err){
    server.emit('error',err,buffer,rinfo);
    return;
  };

  if(msg.event === undefined)
    return;

  if(events[msg.event] !== undefined)
    server.emit(events[msg.event],msg,rinfo);

});

module.exports = server;