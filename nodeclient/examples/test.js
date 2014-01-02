var Client = require('../client');


var c = new Client({address : '10.0.1.24'});

c.on('error',function(err){
  console.log('Error:',err);
});

c.stopCook(function(err,rmsg){
  console.log(rmsg);
});

/*
setInterval(function(){
  c.returnState(function(err,rmsg){
    console.log(rmsg);
  });
},5000);
*/