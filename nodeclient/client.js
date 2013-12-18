var net = require('net');
var client = net.connect({host:'192.168.1.16',port: 3000},onConnect);

client.on('data', function(data) {
  console.log(data);
  client.end();
});

client.on('end', function() {
  console.log('client disconnected');
});

function onConnect(){
  console.log('client connected');
  sendPacket();
}

function sendPacket(){
  var buf = new Buffer([0x01,0x02,0x03,0x04,0x05]);
  client.write(buf);
}



