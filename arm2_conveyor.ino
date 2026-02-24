#include <Servo.h>

/* -------------------- Servos -------------------- */
Servo m1;
Servo m2;
Servo m3;
Servo m4;

// Posiciones actuales (estado inicial / seguridad)
int pos1 = 60;
int pos2 = 130;
int pos3 = 180;
int pos4 = 140;

// Límites del movimiento del brazo (m3)
const int downPos3 = 93;
const int upPos3 = 180;

/* -------------------- Motor / Sensor -------------------- */
// Pines del motor de la banda transportadora
const int IN1 = 12;
const int IN2 = 13;
const int ENA = 11; // PWM

// Pin del sensor infrarrojo
const int sensorPin = 7;

// Parámetros
const int conveyorSpeed = 50;            
const unsigned long motorOnMillis = 1000UL; //Mantener motor encendido 1 segundo
const int dropDelay = 500;               // Pausa antes de bajar el brazo (ms)

// Estado del sensor cuando detecta (LOW = detecta)
const int detectionActiveState = LOW;

void setup() {
  // Pines motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Sensor
  pinMode(sensorPin, INPUT);

  Serial.begin(9600);

  // Adjuntar servos
  m1.attach(2);
  m2.attach(4);
  m3.attach(6);
  m4.attach(8);

  // Posición inicial
  m1.write(pos1);
  m2.write(pos2);
  m3.write(pos3);
  m4.write(pos4);

  // Asegurar motor apagado
  stopConveyor();

  Serial.println("Sistema listo. Esperando cubo...");
}

void loop() {
  int sensorValue = digitalRead(sensorPin);

  if (sensorValue == detectionActiveState) {
    Serial.println("Cubo detectado: iniciando ciclo.");

    // 1) Encender banda 1 segundo
    runConveyorFor(motorOnMillis);

    // 2) Espera antes de mover brazo
    delay(dropDelay);

    // 3) Bajar brazo (servo m3)
    lowerArmForTimeout(2000UL);

    // 4) Subir brazo
    raiseArmToUpPosition();

    // 5) Movimiento ejemplo de servos
    Serial.println("Moviendo servos: posición de trabajo...");
    smoothMoveArm(80, 120, pos3, 160);
    delay(600);
    Serial.println("Volviendo a posición de seguridad...");
    smoothMoveArm(60, 130, upPos3, 140);

    Serial.println("Ciclo completado.");

    // Esperar a que el cubo se aleje
    unsigned long waitStart = millis();
    const unsigned long waitTimeout = 3000UL;
    while (digitalRead(sensorPin) == detectionActiveState && (millis() - waitStart) < waitTimeout) {
      delay(30);
    }
    delay(100);
  } else {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
      Serial.println("Esperando cubo...");
      lastPrint = millis();
    }
    stopConveyor();
    delay(50);
  }
}

/* -------------------- Funciones auxiliares -------------------- */

void startConveyor() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 53);
}

void stopConveyor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void runConveyorFor(unsigned long durationMillis) {
  Serial.print("Banda ON por (ms): ");
  Serial.println(durationMillis);
  startConveyor();
  delay(durationMillis);
  stopConveyor();
  Serial.println("Banda OFF");
}

void lowerArmForTimeout(unsigned long upToMillis) {
  unsigned long t0 = millis();
  while (millis() - t0 < upToMillis) {
    if (pos3 > downPos3) {
      pos3--;
      m3.write(pos3);
      delay(15);
    } else {
      break;
    }
  }
  unsigned long elapsed = millis() - t0;
  if (elapsed < upToMillis) delay(upToMillis - elapsed);
}

void raiseArmToUpPosition() {
  while (pos3 < upPos3) {
    pos3++;
    m3.write(pos3);
    delay(15);
  }
}

void smoothMoveArm(int target1, int target2, int target3, int target4) {
  int stepDelay = 10;
  int stepSize = 1;

  while (pos1 != target1 || pos2 != target2 || pos3 != target3 || pos4 != target4) {
    if (pos1 < target1) pos1 += stepSize;
    else if (pos1 > target1) pos1 -= stepSize;

    if (pos2 < target2) pos2 += stepSize;
    else if (pos2 > target2) pos2 -= stepSize;

    if (pos3 < target3) pos3 += stepSize;
    else if (pos3 > target3) pos3 -= stepSize;

    if (pos4 < target4) pos4 += stepSize;
    else if (pos4 > target4) pos4 -= stepSize;

    m1.write(pos1);
    m2.write(pos2);
    m3.write(pos3);
    m4.write(pos4);
    delay(stepDelay);
  }
}

