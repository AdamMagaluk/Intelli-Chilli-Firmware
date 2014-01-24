var fs = require('fs');
var Client = require('./client');

var c = new Client({address : 'arduino1.local'});

var file = process.argv[2];

var fd = fs.openSync(file,'a+');

function getTemp(){
  c.returnState(function(err,json){
    if(err)
      return console.error(err);
    var text = new Date().getTime() + " " + json.currentTemp + " " + json.heaterOn+"\n";
    fs.writeSync(fd,text);
  });
}

setInterval(getTemp,15*1000);