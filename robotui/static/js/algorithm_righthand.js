$(document).ready(function() {
  var algorithmMap
  var count = 0;
  var algorithmMap = null
  var seq = ['init','left','right','left','right','left','left','forward','left','uturn','right','left','right','uturn','right','left','left','uturn','right','forward','forward','right','forward','forward','forward','forward','right','right','uturn','right','forward'];

  setAlgorithmMap = function(map) {
    algorithmMap = map;
  };
    
  getAlgorithmNext = function() {
    if (count < seq.length) {
      count += 1;
      return seq[count];
    }
    return ACTION_MAZE_END;
  };   
});  