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
#include <ESPmDNS.h>
#include <SPIFFS.h>

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
const int freq = 5000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;
int dutyCycle = 200;

int speed1 = 0;
int speed2 = 0;
int direction = 0; // 0 stop, 1 forward, 2  backward, 3 right, 4 left
AsyncWebServer server(80);

void controlStop()
{
  // motor 1
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  // motor 2
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
    ledcWrite(pwmChannel1, 0);
  ledcWrite(pwmChannel2, 0);
  Serial.println("stop");
}
void controlForward()
{
  // motor 1
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  // motor 2
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2);
  Serial.println("forward");
}
void controlBackward()
{
  // motor 1
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  // motor 2
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2);
  Serial.println("backward");
}
void controlRight()
{
  // motor 1
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  // motor 2
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2);
  Serial.println("right");

}
void controlLeft()
{
  // motor 1
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  // motor 2
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);

  ledcWrite(pwmChannel1, speed1);
  ledcWrite(pwmChannel2, speed2);
  Serial.println("left");

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
  SPIFFS.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  MDNS.begin("tankServer");
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, PUT, POST");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");

  server.addHandler(new AsyncCallbackJsonWebHandler("/control", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                    {
    const JsonObject &jsonObj = json.as<JsonObject>();

    if (strcmp(jsonObj["direct"],"stop") == 0)
    {
      direction = 0;
    }
    else if (strcmp(jsonObj["direct"],"forward") == 0)
    {
      direction = 1;
    }
    else if (strcmp(jsonObj["direct"],"backward") == 0)
    {
      direction = 2;
    }
    else if (strcmp(jsonObj["direct"],"right") == 0)
    {
      Serial.println("right");
      direction = 3;
    }
    else if (strcmp(jsonObj["direct"],"left") == 0)
    {
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
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

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
