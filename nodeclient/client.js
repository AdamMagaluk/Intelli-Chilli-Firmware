var dgram = require('dgram');
var message = new Buffer("Some bytes");
var client = dgram.createSocket("udp4");


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


