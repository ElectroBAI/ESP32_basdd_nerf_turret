#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Servo setup
Servo triggerServo;
const int TRIGGER_SERVO_PIN = 18;

// Motor pins
const int MOTOR1_A = 12;
const int MOTOR1_B = 13;
const int MOTOR2_A = 14;
const int MOTOR2_B = 27;

// WiFi credentials for Access Point
const char* ssid = "sim";
const char* password = "simple12";

// Web server
WebServer server(80);

// Motor control
bool motorsRunning = false;
unsigned long motorStartTime = 0;
const unsigned long MOTOR_RUN_TIME = 2000;

// Firing control
bool continuousMode = false;
bool isFiring = false;
unsigned long lastFireTime = 0;
const unsigned long FIRE_INTERVAL = 500;
const unsigned long TRIGGER_PRESS_TIME = 200;
unsigned long triggerPressStartTime = 0;
bool triggerPressed = false;

void setup() {
  Serial.begin(115200);

  // Servo setup
  triggerServo.attach(TRIGGER_SERVO_PIN);
  triggerServo.write(0);

  // Motor setup
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);
  pinMode(MOTOR2_A, OUTPUT);
  pinMode(MOTOR2_B, OUTPUT);
  stopMotors();

  // Start Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("Access Point started");
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Web routes
  server.on("/", handleRoot);
  server.on("/fire", handleFire);
  server.on("/continuous", handleContinuous);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (continuousMode && isFiring && millis() - lastFireTime > FIRE_INTERVAL) {
    fireSingleShot();
    lastFireTime = millis();
  }

  if (triggerPressed && millis() - triggerPressStartTime > TRIGGER_PRESS_TIME) {
    triggerServo.write(0);
    triggerPressed = false;
  }

  if (!continuousMode && motorsRunning && millis() - motorStartTime > MOTOR_RUN_TIME) {
    stopMotors();
    motorsRunning = false;
    Serial.println("Motors stopped automatically");
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Nerf Turret Control</title><style>";
  html += "body { font-family: Arial; text-align: center; margin: 50px; }";
  html += "button { padding: 20px; margin: 10px; font-size: 18px; }";
  html += ".fire-button { background-color: red; color: white; font-size: 24px; }";
  html += "</style></head><body><h1>Nerf Turret Control</h1>";
  html += "<button class='fire-button' onclick='singleFire()'>SINGLE FIRE</button><br><br>";
  html += "<button class='fire-button' onclick='toggleContinuous()' id='continuousBtn'>START CONTINUOUS</button><br><br>";
  html += "<button onclick='stopFiring()' style='background-color: orange; color: white;'>STOP FIRING</button>";
  html += "<p>Motors: <span id='motorStatus'>Stopped</span></p>";
  html += "<p>Fire Mode: <span id='fireMode'>Single</span></p>";
  html += "<script>";
  html += "let continuousMode = false;";
  html += "function singleFire() { fetch('/fire'); document.getElementById('motorStatus').textContent = 'Single Fire!'; document.getElementById('fireMode').textContent = 'Single'; setTimeout(() => { document.getElementById('motorStatus').textContent = 'Stopped'; }, 2000); }";
  html += "function toggleContinuous() { if (!continuousMode) { continuousMode = true; document.getElementById('continuousBtn').textContent = 'STOP CONTINUOUS'; document.getElementById('continuousBtn').style.backgroundColor = 'darkred'; document.getElementById('motorStatus').textContent = 'Continuous Fire!'; document.getElementById('fireMode').textContent = 'Continuous'; fetch('/continuous?mode=start'); } else { stopContinuous(); } }";
  html += "function stopContinuous() { continuousMode = false; document.getElementById('continuousBtn').textContent = 'START CONTINUOUS'; document.getElementById('continuousBtn').style.backgroundColor = 'red'; document.getElementById('motorStatus').textContent = 'Stopped'; document.getElementById('fireMode').textContent = 'Single'; fetch('/continuous?mode=stop'); }";
  html += "function stopFiring() { if (continuousMode) { stopContinuous(); } fetch('/stop').then(() => { document.getElementById('motorStatus').textContent = 'Stopped'; }); }";
  html += "</script></body></html>";
  server.send(200, "text/html", html);
}

void handleFire() {
  Serial.println("Single FIRE command received!");
  runMotors();
  motorsRunning = true;
  motorStartTime = millis();
  fireSingleShot();
  server.send(200, "text/plain", "SINGLE FIRE!");
}

void handleContinuous() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "start") {
      Serial.println("Starting continuous fire mode");
      continuousMode = true;
      isFiring = true;
      runMotors();
      motorsRunning = true;
      motorStartTime = millis();
      lastFireTime = millis();
      server.send(200, "text/plain", "CONTINUOUS FIRE STARTED");
    } else if (mode == "stop") {
      Serial.println("Stopping continuous fire mode");
      continuousMode = false;
      isFiring = false;
      stopMotors();
      motorsRunning = false;
      server.send(200, "text/plain", "CONTINUOUS FIRE STOPPED");
    }
  } else {
    server.send(400, "text/plain", "Missing mode parameter");
  }
}

void handleStop() {
  Serial.println("Emergency stop command received!");
  continuousMode = false;
  isFiring = false;
  stopMotors();
  motorsRunning = false;
  triggerServo.write(0);
  triggerPressed = false;
  server.send(200, "text/plain", "STOPPED");
}

void fireSingleShot() {
  triggerServo.write(90);
  triggerPressed = true;
  triggerPressStartTime = millis();
  Serial.println("Trigger actuated - dart fired!");
}

void runMotors() {
  digitalWrite(MOTOR1_A, HIGH);
  digitalWrite(MOTOR1_B, LOW);
  digitalWrite(MOTOR2_A, HIGH);
  digitalWrite(MOTOR2_B, LOW);
  Serial.println("Motors started - flywheels spinning");
}

void stopMotors() {
  digitalWrite(MOTOR1_A, LOW);
  digitalWrite(MOTOR1_B, LOW);
  digitalWrite(MOTOR2_A, LOW);
  digitalWrite(MOTOR2_B, LOW);
  Serial.println("Motors stopped");
}
