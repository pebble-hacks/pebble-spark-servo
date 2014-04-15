

var url = "https://api.spark.io/v1/devices/53ff6c065067544809260487/servo";

function setPosition(position) {
  var response;
  var req = new XMLHttpRequest();

  var params = "access_token=56be42817fe56cbb8afcfbaa9d7cecd2ff875bb5&params=" + position
  console.log("sending: " + url + params);

  req.open('POST', url, true);

  // Send the proper header information along with the request
  req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  req.setRequestHeader("Content-length", params.length);
  req.setRequestHeader("Connection", "close");

  //req.onreadystatechange = function() {//Call a function when the state changes.
  //    if(req.readyState == 4 && req.status == 200) {
  //        console.log(req.responseText);
  //    }
  //}
  req.send(params);

  console.log(req.responseText);
}


Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          setPosition(e.payload.servo);
                          console.log(e.type);
                          console.log(e.payload.servo);
                          console.log("received message!");
                        });

Pebble.addEventListener("webviewclosed",
                         function(e) {
                         console.log("webview closed");
                         console.log(e.type);
                         console.log(e.response);
                         });
