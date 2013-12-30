var net = require('net');
var client = net.connect({host:'192.168.1.16',port: 3000},onConnect);

client.on('data', function(data) {
  console.log(data.toString());
  client.end();
});

client.on('error', function(err) {
  console.log(err);
});

client.on('end', function() {
  console.log('client disconnected');
});

function onConnect(){
  console.log('client connected');
  sendPacket();
}

function sendPacket(){
  var buf = new Buffer([0x05,0x03,0x00,0x05,0xEE]);
  client.write(buf);
}



