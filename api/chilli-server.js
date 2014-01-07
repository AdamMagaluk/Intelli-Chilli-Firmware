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
  var msg = protocol.parse(buffer);
  if(!msg)
    return;

  if(events[msg.data[0]] !== undefined)
    server.emit(events[msg.data[0]],msg,rinfo);

});

module.exports = server;