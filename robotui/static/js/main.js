const CONNECT = "CONNECT";
const CONTROL = "CONTROL";
const MQTT = "MQTT";
const ERROR = "ERROR";
const DRAW = "DRAW";
const EMUL = "EMULATION";

const MODE_INIT = "init"
const MODE_MANUAL = "manual";
const MODE_SUSPEND_CONNECTED = "suspend-connected";
const MODE_SUSPEND_DISCONNECTED = "suspend-disconnected";
const MODE_AUTO = "auto";

const ACTION_CURRENT= "current";
const ACTION_FORWARD = "forward";
const ACTION_LEFT = "left";
const ACTION_RIGHT = "right";
const ACTION_BACKWARD = "backward";
const ACTION_SIGN = "sign";
const ACTION_PAUSE = "pause";
const ACTION_RESUME = "resume";
const ACTION_STOP = "stop";

const ACTION_SERVOUP = "servoup";
const ACTION_SERVODOWN = "servodown";
const ACTION_SERVOLEFT = "servoleft";
const ACTION_SERVORIGHT = "servoright";
const ACTION_CAPTURE = "capture";

const ACTION_MAZE_END = "maze end";
const ACTION_MAP_END = "map end";

// Global Configuration
var emulationMode = false;
var emulationDelay = 500;
var algorithmMode = true;
var autoStartTime = 0;
var currentMode = MODE_INIT;

$(document).ready(function(){
  
  $("#ipInput").val("172.20.10.14");
  //$("#ipInput").val("128.237.128.214");
  
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
      reconnectPeriod : 1000,
      keepalive : 5, 
      will : {
        topic : "robot-request",
        payload : "pause" 
      }
    };
    
    if (ip == "") {
      appendLog(ERROR, "Address is not valid" + ip);
      return;
    } 
    mqttConnect(ip, options);
    
  });

  // Mode Button Handler
  $("#autoButton").click(function() {
    setMode(MODE_AUTO, true);
  });

  $("#resumeAutoButton").click(function() {
    setMode(MODE_AUTO, false);
  });

  $("#manualButton").click(function() {
    setMode(MODE_MANUAL);
  });

  $("#suspendManualButton").click(function() {
    setMode(MODE_MANUAL);
  });

  $("#suspendResumeButton").click(function() {
    setMode(MODE_AUTO, false);
  });

  $("#emul_2x2").click(function() {
    //var map = "x x x x x x o o o x x o x o x x o x e x x x x x x"
	var map = "x x x x x x x x o o o x o x x o x o x o x x o x o x o x x o x o x o x x js2 x rs x o x x x x x x x x"
    startEmulation(3, 3, map);
  });

  $("#emul_2x2_empty").click(function() {
    var map = "x x x x x x o o o x x o o o x x rwo o o x x x x x x"
    startEmulation(2, 2, map);
  });

  $("#emul_2x2_reddot").click(function() {
    var map = "x x x x x x j o o x x o o o x x rwo o o x x x x x x"
    startEmulation(2, 2, map);
  });

  $("#emul_4x4").click(function() {
    startEmulation(4, 4, "x x x x x x x x x x o o o x o o o x x o x o x o x o x x o x da7 x o x o x x o x o x o x o x x rw x o x o x o x x o x o x o x o x x s x o o dd1w4 x e x x x x x x x x x x");
  });

  $("#emul_6x4").click(function() {
    startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o rw x x o x x x o x x x o o o x x o x o x o o o x o o o x x o x o x o x o x x x o x x o o js3 o o x o o o x o x x o x x x o x x x o x o x x o o f x o o b x o o o x x x x x x x x x x x x x x");
  });

  $("#emul_6x4_empty").click(function() {
    //startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o rwo o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x x x x x x x x x x x x x");
    //startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o o x x o x x x o x x x o o o x x o x o x o o o x o o o x x o x o x o x o x x x o x x o o js1 o o x o o o x o x x o x x x o x x x o x o x x o o e x o o s x o o rw x x x x x x x x x x x x x x");
    //startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o o x x o o o x x x x x o o o x x o o o x o o o x o o o x x o o o x x x x x o o o x x o o o o rs o o o o o o x x o o o o o o o o o o o x x o o o o o o o o o o o x x x x x x x x x x x x x x");
    startEmulation(6, 4, "x x x x x x x x x x x x x x o o o o o o o o o o jw2 x x o x x x o x x x o o o x x o x o x o o o x o o o x x o x o x o x o x x x o x x o o js1 o o x o o o x o x x o x x x o x x x o x o x x o o e x o o s x o o rw x x x x x x x x x x x x x x");
  });

  $("#emul_stop").click(function() {
    stopEmulation();
  });
  
  // Move Button Handler
  $("#currentButton").click(function() {
    mqttPublish("robot-request", ACTION_CURRENT);
  });
  
  $("#forwardButton").click(function() {
    mqttPublish("robot-request", ACTION_FORWARD);
  });

  $("#leftButton").click(function() {
    mqttPublish("robot-request", ACTION_LEFT);
  });

  $("#rightButton").click(function() {
    mqttPublish("robot-request", ACTION_RIGHT);
  });

  $("#backwardButton").click(function() {
    mqttPublish("robot-request", ACTION_BACKWARD);
  });

  $("#signButton").click(function() {
    mqttPublish("robot-request", ACTION_SIGN);
  });

  $("#pauseButton").click(function() {
    mqttPublish("robot-request", ACTION_PAUSE);
  });

  $("#resumeButton").click(function() {
    mqttPublish("robot-request", ACTION_RESUME);
  });

  $("#stopButton").click(function() {
    mqttPublish("robot-request", ACTION_STOP);
  });

  $("#servoupButton").click(function() {
    mqttPublish("robot-request", ACTION_SERVOUP);
  });
    
  $("#servodownButton").click(function() {
    mqttPublish("robot-request", ACTION_SERVODOWN);
  });
    
  $("#servoleftButton").click(function() {
    mqttPublish("robot-request", ACTION_SERVOLEFT);
  });
    
  $("#servorightButton").click(function() {
    mqttPublish("robot-request", ACTION_SERVORIGHT);
  });

  $("#captureButton").click(function() {
    mqttPublish("robot-request", ACTION_CAPTURE);
  });

  $("#algorithm_ui").click(function() {
    $("#algorithmButton").text('UI\'s Algorithm');
    algorithmMode = true;
    //initAlgorithm();
  });

  $("#algorithm_robot").click(function() {
    $("#algorithmButton").text('Robot\'s Algorithm');
    algorithmMode = false;
    //initAlgorithm();
  });
    
  // Log Clear
  $("#logClearButton").click(function() {
    $("#logPanel").text("").append('<font class="text-warning">CMU Robot Project</font> by YOLO\n');
  });

  // ---------------------------------------------------------------------------
  // Common functions
  // ---------------------------------------------------------------------------

  setMode = function(mode, clearmap = false) {
    switch (mode) {
      case MODE_INIT:
        $("#modeLabel").text("INITIAL");
        $("#modeBadge").removeClass("badge-primary").addClass("badge-warning");
        $("#manualButton").attr("disabled", true);
        $("#autoButton").attr("disabled", true);
        $("#resumeAutoButton").attr("disabled", true);
        disableMoveButtons(true);
        break;

      case MODE_AUTO:
        $("#modeLabel").text("AUTONOMOUSE");
        $("#modeBadge").removeClass("badge-primary").addClass("badge-danger");
        $("#manualButton").attr("disabled", false);
        $("#autoButton").attr("disabled", true);
        $("#resumeAutoButton").attr("disabled", true);
        disableMoveButtons(true);

        autoStartTime = Date.now();

        switch(currentMode) {
          case MODE_MANUAL:
            if (clearmap) {
              mqttPublish("algorithm-request", "map clear");
            }
            mqttPublish("algorithm-request", "maze start");
            break;
          case MODE_SUSPEND_CONNECTED:
            mqttPublish("robot-request", "resume");
            mqttPublish("algorithm-request", "maze start");
            break;
        } 

        break;
      case MODE_MANUAL:
        $("#modeLabel").text("MANUAL");
        $("#modeBadge").removeClass("badge-warning").removeClass("badge-danger").addClass("badge-primary");
        $("#manualButton").attr("disabled", false);
        $("#autoButton").attr("disabled", false);
        $("#resumeAutoButton").attr("disabled", false);
        disableMoveButtons(false);

        switch(currentMode) {
          case MODE_INIT:
            mqttPublish("robot-request", "resume");
            mqttPublish("algorithm-request", "map clear");
            mqttPublish("algorithm-request", "map start");
            break;
          case MODE_SUSPEND_CONNECTED:
            mqttPublish("robot-request", "resume");
            mqttPublish("algorithm-request", "map start");
            break;
          case MODE_MANUAL:
            mqttPublish("algorithm-request", "map clear");
            mqttPublish("algorithm-request", "map start");
            break;
          case MODE_AUTO:
            mqttPublish("algorithm-request", "map start");
            break;
        }
        break;
      case MODE_SUSPEND_CONNECTED:
        $("#modeLabel").text("SUSPEND");
        $("#modeBadge").removeClass("badge-danger").removeClass("badge-primary").addClass("badge-warning");
        $("#manualButton").attr("disabled", false);
        $("#autoButton").attr("disabled", true);
        $("#resumeAutoButton").attr("disabled", false);
        disableMoveButtons(false);

        switch(currentMode) {
          case MODE_SUSPEND_DISCONNECTED:
            $('#disconnectModal').modal('hide');
            drawLastMap("failMapCanvas");
            $("#suspendLabel").text("Connection Established");
            $("#suspendDescLabel").text("");
            $("#suspendModal").modal('show');
            break;
        }
        break;
      case MODE_SUSPEND_DISCONNECTED:
        $("#modeLabel").text("SUSPEND");
        $("#modeBadge").removeClass("badge-danger").removeClass("badge-primary").addClass("badge-warning");
        $("#manualButton").attr("disabled", true);
        $("#autoButton").attr("disabled", true);
        $("#resumeAutoButton").attr("disabled", true);
        disableMoveButtons(false);
        break;
    }
    currentMode = mode;
  };

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
          verifyMap(array[1], array[2], array.slice(3).join(" "));
          drawMap(array[1], array[2], array.slice(3).join(" "));
        } else if (array[0] == "maze" && array[1] =="end") {
          drawLastMap("finishMapCanvas");
          var elapsedTime = (Date.now() - autoStartTime) / 1000;
          elapsedTime = elapsedTime.toFixed(1);
          $("#finishLabel").text("Maze Mapping Finish");
          $("#countLabel").text(elapsedTime + " secs / " + array[2] + " moves");
          $("#finishModal").modal('show');

          setMode(MODE_MANUAL);

        } else if (array[0] == "maze" && array[1] =="fail") {
          drawLastMap("failMapCanvas");
          $("#suspendLabel").text("Maze Mapping Fail");
          $("#suspendDescLabel").text(array.slice(2).join(" "));
          $("#suspendModal").modal('show');

          setMode(MODE_SUSPEND_CONNECTED);
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
      $("#connectionLabel").text("Connected");

      mqttClient.subscribe("algorithm-request");
      mqttClient.subscribe("algorithm-response");
      mqttClient.subscribe("robot-request");
      mqttClient.subscribe("robot-response");

      if (currentMode == MODE_INIT) {
        setMode(MODE_MANUAL);
      } else if (currentMode == MODE_SUSPEND_DISCONNECTED) {
        setMode(MODE_SUSPEND_CONNECTED);
      }
    });

    mqttClient.on('close', function () {
      $("#connectButton").attr("disabled", false);
      $("#ipInput").attr("disabled", false);
      appendLog(ERROR, "Connection closed");
      $("#connectionLabel").text("Disconnected");

      setMode(MODE_SUSPEND_DISCONNECTED);
      $('#disconnectModal').modal('show');
      $('#disconnectModal').focus();
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
    $("#currentButton").attr("disabled", disabled);
    $("#leftButton").attr("disabled", disabled);
    $("#forwardButton").attr("disabled", disabled);
    $("#rightButton").attr("disabled", disabled);
    $("#leftButton").attr("disabled", disabled);
    $("#backwardButton").attr("disabled", disabled);
    $("#signButton").attr("disabled", disabled);

    $("#pauseButton").attr("disabled", disabled);
    $("#resumeButton").attr("disabled", disabled);
    $("#stopButton").attr("disabled", disabled);

    $("#servoupButton").attr("disabled", disabled);
    $("#servodownButton").attr("disabled", disabled);
    $("#servoleftButton").attr("disabled", disabled);
    $("#servorightButton").attr("disabled", disabled);
    $("#captureButton").attr("disabled", disabled);
  };
  
  $("#algorithm_robot").trigger("click");
  setMode(MODE_INIT);
});
