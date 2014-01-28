var HueApi = require("node-hue-api").HueApi;
    Fog = require('thefog'),
    Client = require('intelli-chilli-client'),
    udpserver = require('./chilli-server'),
    hue = require('./hue'),
    Packet = Fog.Packet;

//var client = new Fog.Client({'endpoint':'ws://thefog.herokuapp.com/'});
var client = new Fog.Client({'endpoint':'ws://localhost:3000/'});
var chili = new Client({address:'arduino1.local'});

client.on('error', function(data) {
  console.log('error');
  console.log(data);
});

client.open(function() {
  console.log('Client opened!');
});

client.on('ACK', function(data) {
  var clientId = data.clientId;
  console.log('subscription acknowledged');
  var p = new Packet({'action':'PONG', 'data':{'clientId':clientId}});
  client.send(p);
});

client.on('PING', function(data) {
  console.log('Just pinged by server.');
  chili.ping(function(err) {
    console.log(arguments);    
  });
});

client.on('state', function(p) {

  var state = { 
    cookTime: 0,
    cookTimeRange : [0,1440],
    cookTimeLeft: 0,
    cookTemp: 'warm',
    cookTempRange : ['warm','low','medium','high'],
    currentTemp: 25,
    lidState: 'closed',
    cooking: false,
    heaterOn: false,
    address: '28554D220500009C',
  };

  var p2 = new Packet({'action':'state',data : state});
  client.respondTo(p, p2);
});

client.on('time', function(p) {
  var p2 = new Packet({'action':'time',data : { time : p.message.data.time} });
  client.respondTo(p, p2);
});

client.on('temp', function(p) {
  var p2 = new Packet({'action':'temp',data : { temp : p.message.data.temp} });
  client.respondTo(p, p2);
});

client.on('start', function(p) {
  var p2 = new Packet({'action':'start',data : {} });
  client.respondTo(p, p2);
});

client.on('stop', function(p) {
  var p2 = new Packet({'action':'stop',data : {} });
  client.respondTo(p, p2);
});

client.on('reset', function(p) {
  var p2 = new Packet({'action':'reset',data : {} });
  client.respondTo(p, p2);
});

client.on('error', function(p) {
  var p2 = new Packet({'action':'state',data : {
    error : "Timeout reached when trying to communicate with end device."
  }});
  client.respondTo(p, p2);
});

udpserver.bind(3000);

udpserver.on('error',function(err){
  console.error(err);
});

udpserver.on('powered',function(msg,rinfo){
  client.send(new Packet({'action':'powered',data : { host : rinfo.address }  }));
  console.log("Event: (powered on) from " + rinfo.address + ":" + rinfo.port);
});

udpserver.on('cookend',function(msg,rinfo){

  hue.notification(function(err){
    if(err)
      console.error(err);
  });

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


