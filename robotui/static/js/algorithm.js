$(document).ready(function(){
  
  const MODE_NONE = 0;
  const MODE_MAP = 1;
  const MODE_MAZE = 2;
  const maxWidth = 6;
  const maxHeight = 6;
  
  var algorithmMode = MODE_NONE;
  var discoveredMap = null;
  var algorithmCurX = 0;
  var algorithmCurY = 0;
  var algorithmCurDir = 'w';

  initAlgorithm = function() {
    discoveredMap = [];
    discoveredMap = new Array();

    for (var i = 0; i < (maxHeight * 2 - 1) * 2 + 1; i++) {
      //discoveredMap.push(Array.from('?'.repeat((maxWidth * 2 - 1) * 2 + 1)));
      discoveredMap[i] = [];
      for (var j = 0; j < (maxWidth * 2 - 1) * 2 + 1; j++) {
        discoveredMap[i][j] = '?';
      }
    }
    algorithmCurX = 0;
    algorithmCurY = 0;    
    algorithmCurDir = 'w';
  }  
  
  doAlgorithm = function(topic, message) {
    var array = message.split(/(\s+)/).filter( function(e) { return e.trim().length > 0; } );  

    if (topic == "algorithm-request") {
      if (array[0] == "map" && array[1] == "start") {
        algorithmMode = MODE_MAP;
        initAlgorithm();
        mqttPublish("algorithm-response", "ack map start");
        
      } else if (array[0] == "maze" && array[1] == "start") {
        algorithmMode = MODE_MAZE;
        initAlgorithm();
        mqttPublish("algorithm-response", "ack maze start");
        mqttPublish("robot-request", ACTION_CURRENT);
      } else if (array[1] == "stop") {
        algorithmMode = MODE_NONE;
      }  
    } else if (topic == "robot-response") {
      if (array[0] == "ack" && array[2] != "cannot"
          && (array[1] == ACTION_CURRENT || array[1] == ACTION_FORWARD || array[1] == ACTION_LEFT || array[1] == ACTION_RIGHT || array[1] == ACTION_BACKWARD)) {
        var mm = array.slice(2).join(" ");
        var target = getTargetPosition(algorithmCurX, algorithmCurY);
        
        console.log(discoveredMap);
        console.log(target);
        console.log(mm);

        discoveredMap[target.y][target.x] = eraseRobotStr(discoveredMap[target.y][target.x]);
        var result = getNextPosition(algorithmCurX, algorithmCurY, algorithmCurDir, array[1]);
        algorithmCurX = result.x;
        algorithmCurY = result.y;
        algorithmCurDir = result.dir;
        var map = makeMap(algorithmCurX, algorithmCurY, algorithmCurDir, mm);
        mqttPublish("algorithm-response", map);
        
        // Auto Mode
        if (algorithmMode == MODE_MAZE) {
          setAlgorithmMap(discoveredMap);
          var action = getAlgorithmNext();
          if (action != ACTION_MAZE_END && action != ACTION_MAP_END) {
            mqttPublish("robot-request", action);
          } else {
            mqttPublish("algorithm-response", action);
          }  
        }  
      }
      if (array[0] == "ack" && array[1] == ACTION_SIGN && array[2] != "cannot") {
        var sign = array[2];
        var target = getTargetPosition(algorithmCurX, algorithmCurY);
        discoveredMap[target.y][target.x] += mapSignStr(sign, algorithmCurDir);

        var map = makeMap(algorithmCurX, algorithmCurY, algorithmCurDir, "");
        mqttPublish("algorithm-response", map);

        // Auto Mode
        if (algorithmMode == MODE_MAZE) {
          setAlgorithmMap(discoveredMap);
          var action = getAlgorithmNext();
          if (action != ACTION_MAZE_END && action != ACTION_MAP_END) {
            mqttPublish("robot-request", action);
          } else {
            mqttPublish("algorithm-response", action);
          }  
        }  
      } 
  
    }
  };

  getTargetPosition = function(x, y) {
    var centerX = maxWidth - 1;
    var centerY = maxHeight - 1;
    var targetX = (centerX + x) * 2 + 1;
    var targetY = (centerY + y) * 2 + 1;
    return { x : targetX, y : targetY };
  };  
    
  makeMap = function(currentX, currentY, currentDir, mm) {
    
    if (mm.length > 0) {
      var target = getTargetPosition(currentX, currentY);
	    var map = strToMap(1, 1, mm);
	    var result = getMap(map, 0, 0);
	    discoveredMap[target.y][target.x] = "r" + currentDir + result.current;
	    setMap(discoveredMap, target.x, target.y, currentDir, result.forward, result.left, result.right, result.backward, '?');
    }
    
    // calculate map size
    var minX = 1000;
    var maxX = -1000;
    var minY = 1000;
    var maxY = -1000;
    for (var i = 0; i < (maxWidth * 2 - 1) * 2 + 1; i++) {
      for (var j = 0; j < (maxHeight * 2 - 1) * 2 + 1; j++) {
        if (discoveredMap[j][i] != '?') {
          if (j > maxY)   maxY = j;
          if (j < minY)   minY = j;
          if (i > maxX)   maxX = i;
          if (i < minX)   minX = i;
        }   
      }   
    }  
    
    var calWidth = maxX - minX + 1;
    var calHeight = maxY - minY + 1;
    var resultArray = [];
    var resultString = "map " + ((calWidth - 1) / 2) + " " + ((calHeight - 1) / 2) + " ";
    
    for (var i = 0; i < calHeight; i++) {
      var row = discoveredMap[minY + i].slice(minX, maxX + 1);

      resultArray.push(row);
    }  
    mapReplace(resultArray, "?", "-");
    resultString += mapToStr(resultArray);
    
    //console.log(discoveredMap);
    //console.log(resultArray);
    
    return resultString; 
  };
});  