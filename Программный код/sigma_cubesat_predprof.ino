#include <SPI.h>
#include <Servo.h>
#include <RF24.h>

#define SERVO_AZIMUTH_PIN 9//поворот
#define SERVO_ELEVATION_PIN 10//наклон
#define LASER_PIN 6
#define EMERGENCY_PIN A0

RF24 radio(7, 8);
const byte address[6] = "00001";

Servo servoAzimuth;
Servo servoElevation;

#define CENTER 90
#define MIN_ANGLE 50
#define MAX_ANGLE 130
#define STEP 10

bool emergency_active = false;

void setup() {
  Serial.begin(9600);
  Serial.println("КУБСАТ");
  
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);
  pinMode(EMERGENCY_PIN, INPUT_PULLUP);
  
  servoAzimuth.attach(SERVO_AZIMUTH_PIN);
  servoElevation.attach(SERVO_ELEVATION_PIN);
  servoAzimuth.write(CENTER);
  servoElevation.write(CENTER);
  
  if (!radio.begin()) {
    Serial.println("NRF24 не обнаружен!");
    while (1);
  }
  
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  Serial.println("Готов.");
}

void loop() {
  if (digitalRead(EMERGENCY_PIN) == LOW) {
    if (!emergency_active) {
      emergency_stop();
      emergency_active = true;
    }
    return;
  } else {
    emergency_active = false;
  }

  if (radio.available()) {
    char command;
    radio.read(&command, sizeof(command));
    
    switch (command) {
      case '1': horizontal_scan(); break;
      case '2': vertical_scan(); break;
      case '3': diagonal_scan_1(); break;
      case '4': diagonal_scan_2(); break;
      case '0': stop_and_home(); break;
      case '5': digitalWrite(LASER_PIN, HIGH); break;
      case '6': digitalWrite(LASER_PIN, LOW); break;
    }
  }
  delay(10);
}

void horizontal_scan() {
  digitalWrite(LASER_PIN, HIGH);
  for (int angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += STEP) {
    if (check_emergency()) return;
    servoAzimuth.write(angle);
    servoElevation.write(CENTER);
    delay(500);
  }
  stop_and_home();
}

void vertical_scan() {
  digitalWrite(LASER_PIN, HIGH);
  for (int angle = MIN_ANGLE; angle <= MAX_ANGLE; angle += STEP) {
    if (check_emergency()) return;
    servoAzimuth.write(CENTER);
    servoElevation.write(angle);
    delay(500);
  }
  stop_and_home();
}

void diagonal_scan_1() {
  digitalWrite(LASER_PIN, HIGH);
  for (int offset = -40; offset <= 40; offset += STEP) {
    if (check_emergency()) return;
    int az = CENTER + offset;
    int el = CENTER + offset;
    servoAzimuth.write(az);
    servoElevation.write(el);
    delay(500);
  }
  stop_and_home();
}

void diagonal_scan_2() {
  digitalWrite(LASER_PIN, HIGH);
  for (int offset = -40; offset <= 40; offset += STEP) {
    if (check_emergency()) return;
    int az = CENTER + offset;
    int el = CENTER - offset;
    servoAzimuth.write(az);
    servoElevation.write(el);
    delay(500);
  }
  stop_and_home();
}

void stop_and_home() {
  digitalWrite(LASER_PIN, LOW);
  servoAzimuth.write(CENTER);
  servoElevation.write(CENTER);
}

void emergency_stop() {
  Serial.println("АВАРИЙНАЯ ОСТАНОВКА");
  digitalWrite(LASER_PIN, LOW);
  servoAzimuth.write(CENTER);
  servoElevation.write(CENTER);
}

bool check_emergency() {
  if (digitalRead(EMERGENCY_PIN) == LOW) {
    emergency_stop();
    return true;
  }
  return false;
}