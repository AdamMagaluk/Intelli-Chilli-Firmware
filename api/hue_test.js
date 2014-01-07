//2c5ce9222d0363f3bd5766032a6821f
//192.168.1.2
var HueApi = require("node-hue-api").HueApi;

var displayResult = function(result) {
    console.log(JSON.stringify(result, null, 2));
};

var hostname = "192.168.1.2",
    username = "2c5ce9222d0363f3bd5766032a6821f",
    api;

api = new HueApi(hostname, username);

// --------------------------
// Using a callback
api.connect(function(err, config) {
    if (err) throw err;
    displayResult(config);
});