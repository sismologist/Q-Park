#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración de pines y componentes
#define TRIG1 2 // Sensor HC-SR04 entrada (TRIG)
#define ECHO1 3 // Sensor HC-SR04 entrada (ECHO)
#define TRIG2 4 // Sensor HC-SR04 salida (TRIG)
#define ECHO2 5 // Sensor HC-SR04 salida (ECHO)
#define BUZZER_ENTRADA 8 // Pin del buzzer para entrada
#define BUZZER_SALIDA 9  // Pin del buzzer para salida
#define DISTANCIA_UMBRAL 5 // Umbral para detectar coches en cm

Servo servoEntrada;
Servo servoSalida;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int espaciosTotales = 10;
int espaciosDisponibles = 10;

void setup() {
  // Configuración de pantalla y sensores
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Parqueadero");
  lcd.setCursor(0, 1);
  lcd.print("Espacios: ");
  lcd.print(espaciosDisponibles);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Configuración de servos
  servoEntrada.attach(6);
  servoSalida.attach(7);
  servoEntrada.write(0); // Barrera abajo
  servoSalida.write(0);  // Barrera abajo

  // Configuración de buzzers
  pinMode(BUZZER_ENTRADA, OUTPUT);
  pinMode(BUZZER_SALIDA, OUTPUT);
}

void loop() {
  // Detección entrada
  if (detectarCoche(TRIG1, ECHO1) && espaciosDisponibles > 0) {
    activarBuzzer(BUZZER_ENTRADA); // Activar buzzer de entrada
    servoEntrada.write(90); // Levanta la barrera
    delay(3000); // Espera
    servoEntrada.write(0); // Baja la barrera
    espaciosDisponibles--;
    actualizarPantalla();
  }

  // Detección salida (solo si no hay más de 10 espacios vacíos)
  if (detectarCoche(TRIG2, ECHO2)) {
    if (espaciosDisponibles < espaciosTotales) {
      activarBuzzer(BUZZER_SALIDA); // Activar buzzer de salida
      servoSalida.write(90); // Levanta la barrera
      delay(3000); // Espera
      servoSalida.write(0); // Baja la barrera
      espaciosDisponibles++;
      actualizarPantalla();
    } else {
      mostrarErrorSalida();
    }
  }
}

bool detectarCoche(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracion = pulseIn(echoPin, HIGH);
  int distancia = duracion * 0.034 / 2;
  return (distancia > 0 && distancia < DISTANCIA_UMBRAL);
}

void actualizarPantalla() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parqueadero");
  lcd.setCursor(0, 1);
  if (espaciosDisponibles > 0) {
    lcd.print("Espacios: ");
    lcd.print(espaciosDisponibles);
  } else {
    lcd.print("Lo siento:");
    lcd.setCursor(0, 1);
    lcd.print("No hay lugares");
  }
}

void activarBuzzer(int pinBuzzer) {
  digitalWrite(pinBuzzer, HIGH); // Encender buzzer
  delay(500);                   // Esperar 500 ms
  digitalWrite(pinBuzzer, LOW); // Apagar buzzer
}

void mostrarErrorSalida() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error salida:");
  lcd.setCursor(0, 1);
  lcd.print("Espacios maximos");
  delay(2000); // Mostrar el mensaje durante 2 segundos
  actualizarPantalla();
}
