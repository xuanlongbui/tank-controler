/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-slider-pwm/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

// Replace with your network credentials
const char *ssid = "Hn";
const char *password = "12345688";

String sliderValue = "0";

// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin1 = 14; 


// Motor 2
int motor2Pin1 = 19; 
int motor2Pin2 = 18; 
int enable1Pin2 = 16; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 0;
const int resolution = 8;
int dutyCycle = 200;

int speed1 = 0;
int speed2 = 0;
int direction = 0; //0 stop, 1 forward, 2  backward, 3 right, 4 left
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<html>

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <style>
    * {
      touch-action: manipulation;
    }

    .noselect {
      -webkit-touch-callout: none;
      /* iOS Safari */
      -webkit-user-select: none;
      /* Safari */
      -khtml-user-select: none;
      /* Konqueror HTML */
      -moz-user-select: none;
      /* Old versions of Firefox */
      -ms-user-select: none;
      /* Internet Explorer/Edge */
      user-select: none;
      /* Non-prefixed version, currently
                                  supported by Chrome, Edge, Opera and Firefox */
    }

    body {
      font-family: Arial;
      text-align: center;
      margin: 0px auto;
      padding-top: 30px;
    }

    table {
      margin-left: auto;
      margin-right: auto;
    }

    td {
      padding: 8 px;
    }

    .button {
      background-color: #2f4468;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 18px;
      margin: 6px 3px;
      cursor: pointer;
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
    }

    .slider {
      -webkit-appearance: none;
      margin: 14px;
      width: 360px;
      height: 25px;
      background: #FFD65C;
      outline: none;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 35px;
      height: 35px;
      background: #003249;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 35px;
      height: 35px;
      background: #003249;
      cursor: pointer;
    }
  </style>
</head>

<body>
  <script>
    let obj = {
      'direct': 'stop',
      'speed1': 50,
      'speed2': 50
    }
    function driectControl(param) {
      obj.direct = param;
      let xhr = new XMLHttpRequest();
      let url = "control";
      xhr.open("POST", url, true);
      xhr.setRequestHeader("Content-Type", "application/json");
      console.log(obj);
      var data = JSON.stringify(obj);
      xhr.send(data);

    }
    function speedControl1() {
      var sliderValue = document.getElementById("pwmSlider1").value;
      obj.speed1 = sliderValue;
      document.getElementById("textSliderValue1").innerHTML = "Motor1: "+ sliderValue;
    }
    function speedControl2() {
      var sliderValue = document.getElementById("pwmSlider2").value;
      obj.speed2 = sliderValue;
      document.getElementById("textSliderValue2").innerHTML = "Motor2: "+ sliderValue;
    }
  </script>
  <p><span id="textSliderValue1" class="noselect"> Motor1: 50</span></p>
  <p><input type="range" onchange="speedControl1()" id="pwmSlider1" min="0" max="255" value="50" step="1"
      class="slider"></p>
  <p><span id="textSliderValue2" class="noselect"> Motor2: 50</span></p>
  <p><input type="range" onchange="speedControl2()" id="pwmSlider2" min="0" max="255" value="50" step="1"
      class="slider"></p>
  <table>
    <tr>
      <td colspan="3" align="center"><button class="button" onmousedown="driectControl('forward');"
          ontouchstart="driectControl('forward');" onmouseup="driectControl('stop');"
          ontouchend="driectControl('stop');">Forward</button></td>
    </tr>
    <tr>
      <td align="center"><button class="button" ontouchstart="driectControl('left');"
          ontouchend="driectControl('stop');">Left</button></td>
      <td align="center"><button class="button" onmousedown="driectControl('stop');"
          ontouchstart="driectControl('stop');">Stop</button></td>
      <td align="center"><button class="button" ontouchstart="driectControl('right');"
          ontouchend="driectControl('stop');">Right</button></td>
    </tr>
    <tr>
      <td colspan="3" align="center"><button class="button" ontouchstart="driectControl('backward');"
          ontouchend="driectControl('stop');">Backward</button></td>
    </tr>
  </table>
</body>

</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "SLIDERVALUE")
  {
    return sliderValue;
  }
  return String();
}

void controlStop(){
  //motor 1 
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  //motor 2 
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}
void controlForward(){
  //motor 1
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 

  //motor 2
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2); 
}
void controlBackward(){
  //motor 1
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  //motor 2
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW); 

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2); 
}
void controlRight(){
    //motor 1
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  //motor 2
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2); 
}
void controlLeft(){
    //motor 1
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
    //motor 2
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH); 

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2); 
}

void setup()
{

  // Serial port for debugging purposes
  Serial.begin(115200);
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin1, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable1Pin2, OUTPUT);
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel1, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin1, pwmChannel1);
  ledcAttachPin(enable1Pin2, pwmChannel2);

  // ledcWrite(ledChannel, sliderValue.toInt());
  // ledcWrite(ledChannel, sliderValue.toInt());

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  // // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  // server.on("/control", HTTP_POST, [] (AsyncWebServerRequest *request) {
  //   String inputMessage;

  //   // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
  //   if (request->hasParam(PARAM_INPUT)) {
  //     inputMessage = request->getParam(PARAM_INPUT)->value();
  //     sliderValue = inputMessage;
  //     // ledcWrite(ledChannel, sliderValue.toInt());
  //   }
  //   else {
  //     inputMessage = "No message sent";
  //   }
  //   Serial.println(inputMessage);
  //   request->send(200, "text/plain", "OK");
  // });
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.addHandler(new AsyncCallbackJsonWebHandler("/control", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                    {
    const JsonObject &jsonObj = json.as<JsonObject>();
    // String op = jsonObj["direct"];
    // Serial.println(op);
    if (strcmp(jsonObj["direct"],"stop") == 0)
    {
      Serial.println("stop");
      direction = 0;
    }
    else if (strcmp(jsonObj["direct"],"forward") == 0)
    {
      Serial.println("forward");
      direction = 1;
    }
    else if (strcmp(jsonObj["direct"],"backward") == 0)
    {
      Serial.println("backward");
      direction = 2;
    }
    else if (strcmp(jsonObj["direct"],"right") == 0)
    {
      Serial.println("right");
      direction = 3;
    }
    else if (strcmp(jsonObj["direct"],"left") == 0)
    {
      Serial.println("left");
      direction = 4;
    }
    else
    {
      Serial.println(" the direction is invalid");
    }
    speed1 = jsonObj["speed1"];
    speed2 = jsonObj["speed2"];

    Serial.print("SPEED 1: ");
    Serial.println(speed1);
    Serial.print("SPEED 2: ");
    Serial.println(speed2);
    request->send(200, "OK"); }));
  // Start server
  server.begin();
}

void loop()
{
  switch (direction)
  {
  case 0:
    controlStop();
    break;
  case 1:
    controlForward();
    break;
  case 2:
    controlBackward();
    break;
  case 3:
    controlRight();
    break;
  case 4:
    controlLeft();
    break;
  default:
    break;
  }
}
