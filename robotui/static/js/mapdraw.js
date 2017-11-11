$(document).ready(function(){

  drawMap = function(mapWidth, mapHeight, mapString, id="mapCanvas", smallMode = false) {
    
    // initialize variables
    var canvas = document.getElementById(id);
    var context = canvas.getContext('2d');
    var marginX = 15;
    var marginY = 15;
    var canvasWidth = canvas.width - marginX * 2;
    var canvasHeight = canvas.height - marginY * 2;
    var robotImage = document.getElementById("robotImage");
    var wallLineWidth = 9;
    if (smallMode) {
      wallLineWidth = 5;
    }  
    
    // sign images 
    var arrowBImage = document.getElementById("arrowBImage");
    var arrowGoImage = document.getElementById("arrowGoImage");
    var arrowStopImage = document.getElementById("arrowStopImage");
    var arrowTImage = document.getElementById("arrowTImage");
    var arrowLImage = document.getElementById("arrowLImage");
    var arrowL45Image = document.getElementById("arrowL45Image");
    var arrowRImage = document.getElementById("arrowRImage");
    var arrowR45Image = document.getElementById("arrowR45Image");
    var arrowImage = document.getElementById("arrowImage");

    var maxMapWidth = 6;
    var maxMapHeight = 4;
    var blockSize = Math.floor(Math.min(canvasWidth / maxMapWidth, canvasHeight / maxMapHeight));
    
    context.fillStyle = "white";
    context.fillRect(0, 0, canvas.width, canvas.height);

    // draw one block function
    drawBlock = function(x, y, width, height, data) {
      var startX = marginX + (canvasWidth - blockSize * width) / 2 + x * blockSize; 
      var startY = marginY + (canvasHeight - blockSize * height) / 2 + y * blockSize;
      
      // draw background
      if (data[1][1].charAt(data[1][1].length - 1) == 's' || data[1][1].charAt(data[1][1].length - 1) == 'e' || data[1][1].charAt(data[1][1].length - 1) == '-') {
        context.beginPath();
        if (data[1][1].charAt(data[1][1].length - 1) == 's') {
          context.fillStyle = "#00B050";
        } else if (data[1][1].charAt(data[1][1].length - 1) == 'e') {
          context.fillStyle = "#00B0F0";
        } else if (data[1][1].charAt(data[1][1].length - 1) == '-') {
          context.fillStyle = "#DDDDDD";
        }
        context.fillRect(startX, startY, blockSize, blockSize);
        context.stroke();
      }
              
      // draw outer box
      context.beginPath();
      context.lineWidth = 1;
      context.strokeStyle = '#DDDDDD';
      context.rect(startX, startY, blockSize, blockSize);
      context.stroke();
      
      // draw trace line
      if (data[1][1].charAt(data[1][1].length - 1) != '-') {
        context.beginPath();
        context.lineWidth = 2;
        context.strokeStyle = '#333';
        context.moveTo(startX, startY + blockSize / 2);
        context.lineTo(startX + blockSize, startY + blockSize / 2);
        context.moveTo(startX + blockSize / 2, startY);
        context.lineTo(startX + blockSize / 2, startY + blockSize);
        context.stroke();
      }
            
      // draw wall
      context.beginPath();
      context.lineWidth = wallLineWidth;
      //context.strokeStyle = '#178298';
      context.strokeStyle = '#000';
      context.lineCap="round";
      
      if (data[0][1] == 'x') {
        context.moveTo(startX, startY);
        context.lineTo(startX + blockSize, startY);
      }
      if (data[1][0] == 'x') {
        context.moveTo(startX, startY);
        context.lineTo(startX, startY + blockSize);
      }
      if (data[1][2] == 'x') {
        context.moveTo(startX + blockSize, startY);
        context.lineTo(startX + blockSize, startY + blockSize);
      }
      if (data[2][1] == 'x') {
        context.moveTo(startX, startY + blockSize);
        context.lineTo(startX + blockSize, startY + blockSize);
      }
      context.stroke();

      // draw unkown wall
      context.beginPath();
      context.lineWidth = wallLineWidth + 1;
      //context.strokeStyle = '#178298';
      context.strokeStyle = '#CCCCCC';
      context.lineCap="round";
      
      if (data[0][1] == '-' || data[0][1] == '?') {
        context.moveTo(startX, startY);
        context.lineTo(startX + blockSize, startY);
      }
      if (data[1][0] == '-' || data[1][0] == '?') {
        context.moveTo(startX, startY);
        context.lineTo(startX, startY + blockSize);
      }
      if (data[1][2] == '-' || data[1][2] == '?') {
        context.moveTo(startX + blockSize, startY);
        context.lineTo(startX + blockSize, startY + blockSize);
      }
      if (data[2][1] == '-' || data[2][1] == '?') {
        context.moveTo(startX, startY + blockSize);
        context.lineTo(startX + blockSize, startY + blockSize);
      }
      context.stroke();
      
      // draw red dot
      if (data[1][1].charAt(0) == 'j') {
        var radius = blockSize / 9;
        context.beginPath();
        context.arc(startX + blockSize / 2, startY + blockSize / 2, radius, 0, Math.PI * 2, false);
        context.fillStyle = "red";
        context.fill();
        context.lineWidth = 2;
        context.strokeStyle = '#222';
        context.stroke();

        // draw sign
        if (data[1][1].length >= 2 && !smallMode) {
          var signWidth = 30;
          var signHeight = 23;
          var signXMargin = (blockSize - signWidth) / 2;           
          var signYMargin = (blockSize - signHeight) / 2;
          context.beginPath();
          context.lineWidth = 3;
          context.strokeStyle = '#73DC80';
          
          var signs = data[1][1].substring(1)
          for (var i = 0; i < signs.length / 2; i++) {
            var sign = signs.substring(i * 2, (i + 1) * 2);
            var signImage = arrowBImage;
            
            switch(sign.charAt(1)) {
              case '1':   signImage = arrowBImage;    break;
              case '2':   signImage = arrowGoImage;    break;
              case '3':   signImage = arrowStopImage;    break;
              case '4':   signImage = arrowTImage;    break;
              case '5':   signImage = arrowLImage;    break;
              case '6':   signImage = arrowL45Image;    break;
              case '7':   signImage = arrowRImage;    break;
              case '8':   signImage = arrowR45Image;    break;
              case '9':   signImage = arrowImage;    break;
            }   
        
            switch (sign.charAt(0)) {
              case 'w':   
                context.rect(startX + signXMargin, startY, signWidth, signHeight);
                context.drawImage(signImage, startX + signXMargin, startY, signWidth, signHeight);
                break;
              case 's':   
                context.rect(startX + signXMargin, startY + blockSize - signHeight, signWidth, signHeight);
                context.drawImage(signImage, startX + signXMargin, startY + blockSize - signHeight, signWidth, signHeight);
                break;
              case 'a':  
                context.rect(startX, startY + signYMargin, signWidth, signHeight);
                context.drawImage(signImage, startX, startY + signYMargin, signWidth, signHeight);
                break;
              case 'd': 
                context.rect(startX + blockSize - signWidth, startY + signYMargin, signWidth, signHeight);
                context.drawImage(signImage, startX + blockSize - signWidth, startY + signYMargin, signWidth, signHeight);
            }
            context.stroke();
          }
        }              
      }
      
      // draw robot
      var robotStr = getRobotStr(data[1][1]);
      
      if (robotStr) {
        var robotMargin = blockSize / 6;
        
        context.save();
        context.translate(startX + robotMargin, startY + robotMargin);
        context.translate((blockSize - robotMargin * 2) / 2, (blockSize - robotMargin * 2) / 2);
        
        var rad = 0;
        switch (robotStr.charAt(1)) {
          case 'w':   rad = 0;  break;
          case 's':   rad = Math.PI;  break;
          case 'a':   rad = -Math.PI / 2;  break;
          case 'd':   rad = Math.PI / 2;  break;
        }            
        context.rotate(rad);
        context.drawImage(robotImage, - (blockSize - robotMargin * 2) / 2, - (blockSize - robotMargin * 2) / 2, 
                                      blockSize - robotMargin * 2, blockSize - robotMargin * 2);
        context.restore();
      }
    };  
    
    // draw map
    var MA = [];
    var mapRawArray = mapString.split(/(\s+)/).filter( function(e) { return e.trim().length > 0; } );  
    var arrayWidth = mapWidth * 2 + 1;
    var arrayHeight = mapHeight * 2 + 1;
    var signBlockArray = [];
    for (var i = 0; i < arrayHeight; i++) {
      MA[i] = mapRawArray.slice(i * arrayWidth, (i + 1) * arrayWidth); 
    }
    for (var i = 0; i < mapHeight; i++) {
      for (var j = 0; j < mapWidth; j++) {
        var x = j * 2 + 1;
        var y = i * 2 + 1;
        var data = [
          [MA[y-1][x-1], MA[y-1][x], MA[y-1][x+1]],
          [MA[y][x-1], MA[y][x], MA[y][x+1]],
          [MA[y+1][x-1], MA[y+1][x], MA[y+1][x+1]],
        ]; 
        if (MA[y][x].charAt(0) == 'd') {
          signBlockArray.push([j, i, data]);
        } else {  
          drawBlock(j, i, mapWidth, mapHeight, data);
        }
      }
    }
    for (var i = 0; i < signBlockArray.length; i++) {
      var item = signBlockArray[i];
      drawBlock(item[0], item[1], mapWidth, mapHeight, item[2]);
    }
  };

  //mapString = "x x x x x x o o o x x o x o x x rss x e x x x x x x"
  //drawMap(2, 2, mapString);
  
  //mapString = "x x x x x x x x x x o o o x o o o x x o x o x o x o x x o x da7 x o x o x x o x o x o x o x x 0 x o x o x o x x o x o x o x o x x ras x o o dd1s4 x e x x - x x x x x x x";
  //drawMap(4, 4, mapString);
  //drawMap(4, 4, mapString, "smallMapCanvas", true);
  
  //mapString = "x x x x x x x x x x x x x x o o o o o o o o o o o x x o x x x o x x x o o o x x o x o x o o o x ds1 o o x x o x o x o x o x x x o x x o o o o o x o o o x o x x o x x x o x x x o x o x x o o e x o o s x o o rw x x x x x x x x x x x x x x"
  //drawMap(6, 4, mapString);
  
  drawInitMap = function() {
    drawMap(1, 1, "o o o o r o o o o");  
  }

  drawSmallMap = function(width, height, map) {
    drawMap(width, height, map, "smallMapCanvas", true);
  }
  
  drawInitMap();
});
