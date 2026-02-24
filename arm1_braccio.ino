#include <Braccio.h>
#include <Servo.h>

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_ver;
Servo wrist_rot;
Servo gripper;

// Infrarrojo
const int sensorPin = 7; // Pin digital del sensor
int lastState = HIGH;    // Estado anterior del sensor (suponiendo que HIGH = ausencia inicial)

void setup() {
  Braccio.begin();
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  // Posición inicial del brazo (quieto)
  Braccio.ServoMovement(20, 90, 45, 180, 180, 90, 10);
}

void loop() {
  // Leer el valor del sensor
  int sensorValue = digitalRead(sensorPin); 
  Serial.print("Sensor: ");
  Serial.println(sensorValue);

  // Si hay ausencia (0) y antes estaba en 1 → ejecutar el movimiento
  if (sensorValue == LOW && lastState == HIGH) {
    ejecutarMovimiento();
  }

  // Actualizar estado anterior
  lastState = sensorValue;

  delay(50); // Pequeño delay para evitar lecturas erráticas
}

// Secuencia predeterminada del brazo
void ejecutarMovimiento() {
  Braccio.ServoMovement(15,137,50,180,180,56,10);
  Braccio.ServoMovement(15,137,80,180,180,56,73);
  Braccio.ServoMovement(15,137,50,180,180,56,73);
  Braccio.ServoMovement(15,65,50,180,180,56,73);
  Braccio.ServoMovement(15,65,82,170,170,56,73);
  Braccio.ServoMovement(15,65,82,170,170,56,50);

  // Volver a posición inicial
  Braccio.ServoMovement(20, 90, 45, 180, 180, 90, 10);
}
