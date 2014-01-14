var Client = require('./client');

var c = new Client({address : 'arduino1.local'});

c.ping(function(err,json){
  console.log(err)
  console.log(json)
})
