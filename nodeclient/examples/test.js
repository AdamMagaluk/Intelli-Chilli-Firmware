var Client = require('../client');

//var Client = require('intelli-chilli-client');

var c = new Client({address : '10.0.1.24'});
c.returnState(function(err,state){
  //handle err

  console.log('The current temp is: ',state.currentTemp);
  c.close();
});


/*
setInterval(function(){
  c.returnState(function(err,rmsg){
    console.log(rmsg);
  });
},5000);
*/