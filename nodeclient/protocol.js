var PacketTypes = {
  ACK : 0,
};

var PacketTypes = Object.freeze({
  CMD_ACK : 0,
  CMD_PING : 1,
  CMD_SET_COOK_TIME : 2,
  CMD_SET_COOK_TEMP : 3,
  CMD_START_COOK : 4,
  CMD_STOP_COOK : 5,
  CMD_RESET : 6,
  CMD_RETURN_STATE : 7,
  CMD_EVENT : 8
});

var EventTypes = Object.freeze({
  EVENT_POWERED_ON : 0,
  EVENT_COOK_STARTED : 1,
  EVENT_COOK_END : 2,
  EVENT_LID_OPENED : 3,
  EVENT_LID_CLOSED : 4
});

module.exports.PacketTypes = PacketTypes;
module.exports.EventTypes = EventTypes;

function isValidType(t){
  for(var k in PacketTypes){
    if(t === PacketTypes[k])
      return true;
  }
  return false;
}

module.exports.parse = function(buffer){
  if(buffer.length < 3)
    return null;

  var dl = buffer[0];
  if(!isValidType(buffer[1]))
    return null;

  return {
    length : buffer[0],
    type : buffer[1],
    data : buffer.slice(2,buffer.length-1)
  };
};

module.exports.serialize = function(msg){
  if(!msg)
    return null;
  if(msg.type === undefined)
    return null;
  if(!isValidType(msg.type))
    return null;

  var output = null;
  if(Buffer.isBuffer(msg.data)){
    output = new Buffer(msg.data.length+3);
    msg.data.copy(output,2,0);
  }else{
    output = new Buffer(3);
  }
  output[0] = output.length;
  output[1] = msg.type;

  output[output.length-1] = 0xEE;

  return output;
};

