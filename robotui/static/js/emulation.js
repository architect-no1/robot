$(document).ready(function(){

  var emulationMap = null;
  var emulationMapWidth = 0;
  var emulationMapHeight = 0;
  var emulationCurX = 0;
  var emulationCurY = 0;
  var emulationCurDir = 'w';
    
  startEmulation = function(width, height, map) {
    emulationMode = true;
    appendLog(EMUL, "Start emulation " + width + "x" + height, false);

    emulationMap = strToMap(width, height, map);
    console.log(emulationMap);
    emulationMapWidth = width;
    emulationMapHeight = height;

    var robot = findRobot(emulationMap);
    console.log(robot);
    emulationCurX = robot.x;
    emulationCurY = robot.y;
    emulationCurDir = robot.dir;
    emulationMap[robot.y * 2 + 1][robot.x * 2 + 1] = robot.current;
    appendLog(EMUL, "robot=(" + emulationCurX + "," + emulationCurY + "," + emulationCurDir +")");

    $("#cameraImage").attr('width', '0');
    $("#cameraImage").attr('height', '0');
    $("#smallMapCanvas").show();
    drawInitMap();
    drawSmallMap(width, height, map);

    mqttPublish("algorithm-request", "map clear");  
    mqttPublish("algorithm-request", "map start");  
  }; 

  stopEmulation = function() {
    $("#emulationButton").attr("disabled", false);
    emulationMode = false;
    emulationMap = null;
    emulationMapWidth = 0;
    emulationMapHeight = 0;
    appendLog(EMUL, "Stop emulation");

    $("#cameraImage").attr('width', '320');
    $("#cameraImage").attr('height', '240');
    $("#smallMapCanvas").hide();
  };
  
  doEmulation = function(message) {
    message = message.trim();
    
    if (message != ACTION_CURRENT && message != ACTION_FORWARD && message != ACTION_LEFT && message != ACTION_RIGHT && message != ACTION_BACKWARD && message != ACTION_SIGN
        && message != ACTION_PAUSE && message != ACTION_RESUME && message != ACTION_STOP) {
      appendLog(EMUL, "Invalid command");
      return;
    }   

    if (message == ACTION_PAUSE || message == ACTION_RESUME || message == ACTION_STOP) {
      mqttPublish("robot-response", "ack " + message);
      return;
    }

    // check move possible
    var r = getMap(emulationMap, emulationCurX, emulationCurY, emulationCurDir);
    if ((message == ACTION_RIGHT && isWallStr(r.right)) 
        || (message == ACTION_LEFT && isWallStr(r.left))
        || (message == ACTION_FORWARD && isWallStr(r.forward))
        || (message == ACTION_BACKWARD && isWallStr(r.backward))) {
      mqttPublish("robot-response", "ack " + message + " cannot");
      return;
    }   

    if (message == ACTION_SIGN) {
      mqttPublish("robot-response", "ack sign " + rotatedSignStr(getSignStr(r.current), emulationCurDir));
      return;
    }

    // check out of map
    var result = getNextPosition(emulationCurX, emulationCurY, emulationCurDir, message);
    if (!checkRange(result.x, result.y, emulationMapWidth, emulationMapHeight)) {
      mqttPublish("robot-response", "ack " + message + " cannot");
      return;
    }  
    
    emulationCurX = result.x;
    emulationCurY = result.y;
    emulationCurDir = result.dir;

    var mm = getEmulation(emulationCurX, emulationCurY, emulationCurDir);
    mqttPublish("robot-response", "ack " + message + " " + mm);
    appendLog(EMUL, "robot=(" + emulationCurX + "," + emulationCurY + "," + emulationCurDir +")");
    
    // draw mini map
    var clonedMap = JSON.parse(JSON.stringify(emulationMap));
    setRobot(clonedMap, emulationCurX, emulationCurY, emulationCurDir);
    drawSmallMap(emulationMapWidth, emulationMapHeight, mapToStr(clonedMap));
  };
      
  getEmulation = function(curX, curY, curDir) {
    var result = getMap(emulationMap, curX, curY, curDir);
    return ". " + result.forward + " . " + result.left + " " + eraseSignStr(result.current) + " " + result.right + " . - .";
  }  
});