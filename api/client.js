var Fog = require('thefog'),
    Client = require('intelli-chilli-client'),
    Packet = Fog.Packet;

var client = new Fog.Client({'endpoint':'ws://thefog.herokuapp.com/'});
var chili = new Client({address:'192.168.1.23'});

client.open(function() {
  console.log('Client opened!');
});

client.on('ACK', function(data) {
  var clientId = data.clientId;
  console.log('subscription acknowledged clientId:'+clientId);
//  var p = new Packet({'action':'PONG', 'data':{'clientId':clientId}});
//  client.send(p);
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
