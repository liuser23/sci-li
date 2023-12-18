
let webSocket = new WebSocket("wss://kazar4.com:9001");
webSocket.addEventListener("open", (event) => {
    webSocket.send("player1");

    if (window.location.pathname.includes("controller")) {
        webSocket.send("S")
    } else if (window.location.pathname.includes("graphics")) {
        webSocket.send("G")
    } else {
        webSocket.send("F")
    }
});

webSocket.addEventListener("open", (event) => {
    
    let msg = event.data;
    if (length(Array.from(msg)) != 0 && Array.from(msg)[0] == "S") {
        scoreSpan = document.getElementById("scoreVal")
        scoreSpan.innerHTML = msg.substring(1, length(Array.from(msg)) - 1)
    }

});

//https://stackoverflow.com/questions/38956121/how-to-add-delay-to-promise-inside-then
function sleeper(ms) {
    return new Promise(resolve => setTimeout(() => resolve(), ms));
  }

function buttonClicked(buttonName) {

    if (buttonName === "U") {
        webSocket.send("U");
    } else if (buttonName === "R") {
        webSocket.send("R");
    } else if (buttonName === "D") {
        webSocket.send("D");
    } else if (buttonName === "L") {
        webSocket.send("L");
    } else if (buttonName === "DISCONNECT") {
        webSocket.send("DISCONNECT");
    } else if (buttonName === "PONG") {
        webSocket.send("PONG");
    } else if (buttonName === "RAINBOW") {
        webSocket.send("RN");
    } else if (buttonName === "SCORE") {
        webSocket.sen1d("SCORE");
    } else if (buttonName === "OFF") {
        webSocket.send("F");
    } else if (buttonName === "RESTART") {
        webSocket.send("F");
        webSocket.send("S");
        // sleeper(2000).then(() => {
        //     //webSocket.send("SNAKE")
        // })
    } else if (buttonName === "CB") {
        webSocket.send("CB")
    } else if (buttonName === "CONFETTI") {
        webSocket.send("CF")
    } else if (buttonName === "ALL") {
        webSocket.send("G")
    } else if (buttonName === "WDT") {
        webSocket.send("WDT")
    }
}



document.addEventListener("keypress", function(event) {
    if (event.code == "KeyW") {
        console.log('U');
        webSocket.send("U");
    }
    if (event.code == "KeyD") {
        console.log('R');
        webSocket.send("R");
    }
    if (event.code == "KeyS") {
        console.log('D');
        webSocket.send("D");
    }
    if (event.code == "KeyA") {
        console.log('L');
        webSocket.send("L");
    }
});

let upDown = false;
let rightDown = false;
let downDown = false;
let leftDown = false;

document.addEventListener("keydown", function(event) {
    if (event.code == "ArrowUp") {
        console.log('U');
        webSocket.send("U");
    }
    if (event.code == "ArrowRight") {
        console.log('R');
        webSocket.send("R");
    }
    if (event.code == "ArrowDown") {
        console.log('D');
        webSocket.send("D");
    }
    if (event.code == "ArrowLeft") {
        console.log('L');
        webSocket.send("L");
    }
});

// document.addEventListener("keyup", function(event) {
//     if (upDown && event.code == "ArrowUp") {
//         console.log('U');
//         upDown = false;
//     }
//     if (rightDown && event.code == "ArrowRight") {
//         console.log('R');
//         upDown = false;
//     }
//     if (downDown && event.code == "ArrowDown") {
//         console.log('D');
//         downDown = false;
//     }
//     if (leftDown && event.code == "ArrowLeft") {
//         console.log('L');
//         leftDown = false;
//     }
// });
