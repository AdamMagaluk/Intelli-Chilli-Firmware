var Fog = require('thefog'),
    Client = require('intelli-chilli-client'),
    Packet = Fog.Packet;

//var client = new Fog.Client({'endpoint':'ws://thefog.herokuapp.com/'});
var client = new Fog.Client({'endpoint':'ws://localhost:3000/'});
var chili = new Client({address:'192.168.1.23'});




var udpserver = require('./chilli-server');
udpserver.bind(3000);

udpserver.on('listening',function(err){
  console.error('now listening');
});

udpserver.on('error',function(err){
  console.error(err);
});

udpserver.on('powered',function(msg,rinfo){
  client.send(new Packet({'action':'powered',data : { host : rinfo.address }  }));

  console.log("Event: (powered on) from " + rinfo.address + ":" + rinfo.port);
});

udpserver.on('cookend',function(msg,rinfo){
  client.send(new Packet({'action':'cookend',data : { host : rinfo.address }}));
  
  console.log("Event: (cook end) from " + rinfo.address + ":" + rinfo.port);
});

udpserver.on('lidopened',function(msg,rinfo){
  client.send(new Packet({'action':'lidopened',data : { host : rinfo.address }}));
  
  console.log("Event: (lid closed) from " + rinfo.address + ":" + rinfo.port);
});

udpserver.on('lidclosed',function(msg,rinfo){
  client.send(new Packet({'action':'lidclosed',data : { host : rinfo.address }}));
  
  console.log("Event: (lid opened) from " + rinfo.address + ":" + rinfo.port);
});




client.open(function() {
  console.log('Client opened!');
});

client.on('ACK', function(data) {
  var clientId = data.clientId;
  console.log('subscription acknowledged clientId:'+clientId);
  var p = new Packet({'action':'PONG', 'data':{'clientId':clientId}});
  client.send(p);
});

client.on('PING', function(data) {
  console.log('Just pinged by server.');
  chili.ping(function(err) {
    console.log(arguments);    
  });
});

client.on('error', function(data) {
  console.log('error');
  console.log(data);
});
