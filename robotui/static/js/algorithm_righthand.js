$(document).ready(function() {
  var algorithmMap
  var count = 0;
  var algorithmMap = null
  
  setAlgorithmMap = function(map) {
    algorithmMap = map;
  };
    
  getAlgorithmNext = function() {
    if (!algorithmMap) {
      return ACTION_INIT;
    } 
    if (count < 10) {
      count += 1;
      return ACTION_RIGHT;
    }
    return ACTION_MAZE_END;
  };   
});  