var Client = require('./client');
var program = require('commander');

program
  .version('0.0.1')
  .usage('[options] <command>')
  .option('-a, --address [address]', 'Host ip address','127.0.0.1')
  .option('-p, --port [port]', 'Host port address',3000)
  .parse(process.argv);


var commands = {
  start : startCook,
  stop : stopCook,
  temp : setCookTemp,
  time : setCookTime,
  state : showState,
  reset : resetDevice
};

if(typeof commands[program.args[0]] !== 'function'){
  console.error('Command not found.');
  return;
}


var c = new Client({address : program.address,
                    port : program.port
                  });


commands[program.args[0]]();

function startCook(){
  c.startCook(function(err){
    c.close();
    if(err){
      console.error("Failed to start cook",err);
      return;
    }
    console.log('successfully started cook');
  });
}

function stopCook(){
  c.stopCook(function(err){
    c.close();
    if(err){
      console.error("Failed to stop cook",err);
      return;
    }
    console.log('successfully stopped cook');
  });
}


function setCookTemp(){
  var t = program.args[1];
  if(t == undefined){
    c.close();
    console.error('Expecting temp param after command.');
    return;
  }

  c.setCookTemp(t,function(err){
    c.close()
    if(err){
      console.error("Failed to set cook temp",err);
      return;
    }
    console.log('successfully set cook temp');
  });

}
function setCookTime(){
  var t = program.args[1];
  if(t == undefined){
    c.close();
    console.error('Expecting time param after command.');
    return;
  }

  c.setCookTime(t,function(err){
    c.close()
    if(err){
      console.error("Failed to set cook time",err);
      return;
    }
    console.log('successfully set cook time');
  });

}
function showState(){
  c.returnState(function(err,state){
    c.close()
    if(err){
      console.error("Failed to get state",err);
      return;
    }
    
    console.log(state);
  });

}

function resetDevice(){
  c.resetDevice(function(err,state){
    c.close()
    if(err){
      console.error("Failed to reset device",err);
      return;
    }
    console.log('Device restting')
  });
}
