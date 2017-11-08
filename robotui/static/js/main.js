const CONNECT = "CONNECT";
const CONTROL = "CONTROL";
const MQTT = "MQTT";
const ERROR = "ERROR";
const DRAW = "DRAW";
const EMUL = "EMULATION";

const ACTION_INIT = "init";
const ACTION_FORWARD = "forward";
const ACTION_LEFT = "left";
const ACTION_RIGHT = "right";
const ACTION_UTURN = "uturn";
const ACTION_MAZE_END = "maze end";
const ACTION_MAP_END = "map end";

// Global Configuration
var emulationMode = false;
var emulationDelay = 10;
var algorithmMode = true;

$(document).ready(function(){
  
  $("#ipInput").val("192.168.56.101");
  
  var mqttClient = null;
  
  // ---------------------------------------------------------------------------
  // Assign UI Handler
  // ---------------------------------------------------------------------------

  // Connect
  $("#connectButton").click(function() {
    $("#connectButton").attr("disabled", true);
    $("#ipInput").attr("disabled", true);
    
    var ip = $("#ipInput").val();
    var options = {
      reconnectPeriod : 0,
      keepalive : 60
    };
    
    if (ip == "") {
      appendLog(ERROR, "Address is not valid" + ip);
      return;
    } 
    mqttConnect(ip, options);
    
  });

  // Mode Button Handler
  $("#autoButton").click(function() {
    mqttPublish("algorithm-request", "maze start");  
  });

  $("#manualButton").click(function() {

  });

  $("#emul_2x2").click(function() {
    var map = "x x x x x x o o o x x o x o x x o x e x x x x x x"
    startEmulation(2, 2, map);
  });

  $("#emul_2x2_empty").click(function() {
    var map = "x x x x x x o o o x x o o o x x rwo o o x x x x x x"
    startEmulation(2, 2, map);
  });

  $("#emul_4x4").click(function() {
    startEmulation(4, 4, "x x x x x x x x x x o o o x o o o x x o x o x o x o x x o x da7 x o x o x x o x o x o x o x x rw x o x o x o x x o x o x o x o x x s x o o dd1w4 x e x x x x x x x x x x");
  });

  $("#emul_6x4").click(function() {
    startEmulation(6, 4, "map");
  });

  $("#emul_6x4_empty").click(function() {
    startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o rwo o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x x x x x x x x x x x x x");
  });

  $("#emul_stop").click(function() {
    stopEmulation();
  });
  
  // Move Button Handler
  $("#initButton").click(function() {
    mqttPublish("robot-request", "init");
  });
  
  $("#forwardButton").click(function() {
    mqttPublish("robot-request", "forward");
  });

  $("#leftButton").click(function() {
    mqttPublish("robot-request", "left");
  });

  $("#rightButton").click(function() {
    mqttPublish("robot-request", "right");
  });

  $("#uturnButton").click(function() {
    mqttPublish("robot-request", "uturn");
  });

  $("#stopButton").click(function() {
    mqttPublish("robot-request", "stop");
  });
    
  // Log Clear
  $("#logClearButton").click(function() {
    $("#logPanel").text("").append('<font class="text-warning">CMU Robot Project</font> by YOLO\n');
  });

  // ---------------------------------------------------------------------------
  // Common functions
  // ---------------------------------------------------------------------------

  mqttConnect = function(ip, options) {
    appendLog(CONNECT, "Connect to " + ip);
    
    mqttClient = mows.createClient(8083, ip, options);

    mqttClient.on("error", function (e) {
      appendLog(ERROR, e);
    });

    mqttClient.on("message", function (topic, message) {
      appendLog(MQTT, '<font class="text-success">' + topic + "</font> " + message);

      if (topic == "robot-response") {
        if (algorithmMode) {
          doAlgorithm(topic, message);
        }    
      }  
    
      if (topic == "algorithm-response") {
        var array = message.split(/(\s+)/).filter( function(e) { return e.trim().length > 0; } ); 
        if (array[0] == "map") {
          drawMap(array[1], array[2], array.slice(3).join(" "));
        }  
      }  
      
      if (emulationMode && topic == "robot-request") {
        setTimeout(function() { 
           doEmulation(message);
        }, emulationDelay);   
      }
      
      if (algorithmMode && topic == "algorithm-request") {
        doAlgorithm(topic, message);  
      }  
    });

    mqttClient.on('connect', function () {
      $("#connectButton").attr("disabled", true);
      $("#ipInput").attr("disabled", true);
      appendLog(CONNECT, "Connection is established");

      mqttClient.subscribe("algorithm-request");
      mqttClient.subscribe("algorithm-response");
      mqttClient.subscribe("robot-request");
      mqttClient.subscribe("robot-response");
    });

    mqttClient.on('close', function () {
      $("#connectButton").attr("disabled", false);
      $("#ipInput").attr("disabled", false);
      console.log(mqttClient);
      appendLog(ERROR, "Connection closed");
    });
  };
  
  mqttPublish = function(topic, message) {
    if (!mqttClient) {
      appendLog(ERROR, "Connection is not established. Do connect first.");
      return;
    }
    mqttClient.publish(topic, message);
  };
  
  appendLog = function(action, message, useConsole = false) {
    var dt = new Date();
    var timestr = dt.toTimeString().split(' ')[0];
    var logPanel = $("#logPanel");
    
    if (action == ERROR) {
      logPanel.append('<font class="text-warning">' + timestr 
        + '</font> <b><font class="text-danger">' + action + '</font></b> ' + message + "\n");
    } else {
      logPanel.append('<font class="text-warning">' + timestr 
        + '</font> <b><font class="text-primary">' + action + '</font></b> ' + message + "\n");
    }
    logPanel.scrollTop(logPanel.get(0).scrollHeight);
    
    if (useConsole) {
      console.log(action + " " + message);
    }
    // TODO : Prevent Log Overflow
    // console.log(logPanel.text().split('\n').length);
  };  
  
  disableMoveButtons = function(disabled) {
    $("#initButton").attr("disabled", disabled);
    $("#leftButton").attr("disabled", disabled);
    $("#forwardButton").attr("disabled", disabled);
    $("#rightButton").attr("disabled", disabled);
    $("#leftButton").attr("disabled", disabled);
    $("#uturnButton").attr("disabled", disabled);
    $("#stopButton").attr("disabled", disabled);
  };
  
});
