$(document).ready(function() {

  strToMap = function(width, height, mapStr) {
    var mapRawArray = mapStr.split(/(\s+)/).filter( function(e) { return e.trim().length > 0; } );  
    var arrayWidth = width * 2 + 1;
    var arrayHeight = height * 2 + 1;
    
    if (mapRawArray.length != arrayWidth * arrayHeight) {
      appendLog(ERROR, "map data size is invalid. " + (width * 2 + 1) + " x " + (height * 2 + 1) + " != " + mapRawArray.length, true);
    }

    var map = [];
    for (var i = 0; i < arrayHeight; i++) {
      map[i] = mapRawArray.slice(i * arrayWidth, (i + 1) * arrayWidth); 
    }
    return map;
  }
  
  mapToStr = function(map) {
    var str = "";
    for (var i = 0; i < map.length; i++) {
      str += map[i].join(" ") + " ";
    }  
    return str.trim();
  }  

  mapReplace = function(map, from, to) {
    for (var i = 0; i < map.length; i++) {
      for (var j = 0; j < map[i].length; j++) {
        if (map[i][j] == from) {
          map[i][j] = to;
        }  
      }  
    }  
  };

  checkRange = function(x, y, width, height) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      return false;
    }
    return true;   
  }
  
  // x, y : map position  
  getNextPosition = function(x, y, dir, action) {
    var tx = 0, ty = 0, td = dir;
    
    if (action == ACTION_FORWARD) {
      switch (dir) {
        case 'w':  ty = -1;  td = 'w';  break;
        case 's':  ty = 1;   td = 's';  break;
        case 'a':  tx = -1;  td = 'a';  break;
        case 'd':  tx = 1;   td = 'd';  break;
      }  
    } else if (action == ACTION_RIGHT) {   
      switch (dir) {
        case 'w':  tx = 1;   td = 'd';  break;
        case 's':  tx = -1;  td = 'a';  break;
        case 'a':  ty = -1;  td = 'w';  break;
        case 'd':  ty = 1;   td = 's';  break;
      }  
    } else if (action == ACTION_LEFT) {   
      switch (dir) {
        case 'w':  tx = -1;  td = 'a';  break;
        case 's':  tx = 1;   td = 'd';  break;
        case 'a':  ty = 1;   td = 's';  break;
        case 'd':  ty = -1;  td = 'w';  break;
      }  
    } else if (action == ACTION_BACKWARD) {   
      switch (dir) {
        case 'w':  ty = 1;   td = 's';  break;
        case 's':  ty = -1;  td = 'w';  break;
        case 'a':  tx = 1;   td = 'd';  break;
        case 'd':  tx = -1;  td = 'a';  break;
      }  
    }
    return { x: x + tx, y: y + ty, dir:td };  
  };    

  // x, y : map position
  getMap = function(map, x, y, dir = 'w') {
    x = x * 2 + 1;
    y = y * 2 + 1; 
    var current, forward, left, right, backward;
    
    current = map[y][x];
    switch (dir) {
      case 'w':
        forward = map[y - 1][x];
        left = map[y][x - 1];
        right = map[y][x + 1];
        backward = map[y + 1][x];
        break;
      case 's':
        forward = map[y + 1][x];
        left = map[y][x + 1];
        right = map[y][x - 1];
        backward = map[y - 1][x];
        break;
      case 'a':
        forward = map[y][x - 1];
        left = map[y + 1][x];
        right = map[y - 1][x];
        backward = map[y][x + 1];
        break;
      case 'd':
        forward = map[y][x + 1];
        left = map[y - 1][x];
        right = map[y + 1][x];
        backward = map[y][x - 1];
        break;
    }  
    return { current : current, forward : forward, left : left, right : right, backward : backward};
  };  
  
  // x, y : array position
  setMap = function(map, x, y, dir, forward, left, right, backward = null, checkBackward = null) {
    switch(dir) {
      case 'w':
        map[y - 1][x] = forward;
        map[y][x - 1] = left;
        map[y][x + 1] = right;
        if (backward && (!checkBackward || (checkBackward && map[y + 1][x] == checkBackward))) {
          map[y + 1][x] = backward;
        }  
        break;
      case 's':
        map[y + 1][x] = forward;
        map[y][x + 1] = left;
        map[y][x - 1] = right;
        if (backward && (!checkBackward || (checkBackward && map[y - 1][x] == checkBackward))) {
          map[y - 1][x] = backward;
        }  
        break;
      case 'a':
        map[y][x - 1] = forward;
        map[y + 1][x] = left;
        map[y - 1][x] = right;
        if (backward && (!checkBackward || (checkBackward && map[y][x + 1] == checkBackward))) {
          map[y][x + 1] = backward;
        }  
        break;
      case 'd':
        map[y][x + 1] = forward;
        map[y - 1][x] = left;
        map[y + 1][x] = right;
        if (backward && (!checkBackward || (checkBackward && map[y][x - 1] == checkBackward))) {
          map[y][x - 1] = backward;
        }  
        break;
    }  
  };  

  setRobot = function(map, x, y, dir) {
    map[y * 2 + 1][x * 2 + 1] = 'r' + dir + map[y * 2 + 1][x * 2 + 1];
  };
    
  findRobot = function(map, erase = false) {
    var height = (map.length - 1) / 2;
    var width = (map[0].length - 1) / 2;
    
    for (var i = 0; i < height; i++) {
      for (var j = 0; j < width; j++) {
        var robot = getRobotStr(map[i * 2 + 1][j * 2 + 1]);
        var current = eraseRobotStr(map[i * 2 + 1][j * 2 + 1]);
        if (erase) {
          map[i * 2 + 1][j * 2 + 1] = current;
        }  
        if (robot) {
          return { x : j, y : i, dir : robot.charAt(1), current : current };
        }  
      }
    }
    return { x : -1, y : -1, dir : 'w', current : 'o'};
  }  
    
  eraseRobotStr = function(str) {
    var index = str.indexOf("r");
    if (index >= 0) {
      var result = str.replace(str.substring(index, index+2), "");
      if (result.length == 0) {
        return "o";
      } 
      return result;
    } 
    return str;  
  };  
  
  getRobotStr = function(str) {
    var index = str.indexOf("r");
    if (index >= 0) {
      return str.substring(index, index+2)
    }  
    return null;
  }; 
  
  isWallStr = function(str) {
    var index = str.indexOf("x");
    if (index >= 0) {
      return true;
    }  
    return false;
  };  

  verifyMap = function(width, height, mm) {
    // check robot location
  };
});  