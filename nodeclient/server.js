var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var protocol = require('./protocol');

server.on("error", function (err) {
  console.log("server error:\n" + err.stack);
  server.close();
});

/*
  EVENT_POWERED_ON : 0,
  EVENT_COOK_STARTED : 1,
  EVENT_COOK_END : 2,
  EVENT_LID_OPENED : 3,
  EVENT_LID_CLOSED : 4
*/

server.on("message", function (buffer, rinfo) {

  try {
    var msg = JSON.parse(buffer);
  }catch(err){
    console.error(err)
    return;
  }

  console.log(msg);
  
  switch(msg.event){
    case protocol.EventTypes.EVENT_POWERED_ON:
      console.log(rinfo.address + ":" + rinfo.port+": Powered On");
    break;
    case protocol.EventTypes.EVENT_COOK_END:
      console.log(rinfo.address + ":" + rinfo.port+": Cook Ended");
    break;
    case protocol.EventTypes.EVENT_LID_OPENED:
      console.log(rinfo.address + ":" + rinfo.port+": Lid Opened");
    break;
    case protocol.EventTypes.EVENT_LID_CLOSED:
      console.log(rinfo.address + ":" + rinfo.port+": Lid Closed");
    break;
  }


  
});

server.on("listening", function () {
  var address = server.address();
  console.log("server listening " +
      address.address + ":" + address.port);
});

server.bind(3000);
