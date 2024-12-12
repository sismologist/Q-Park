// Importar librerias, LiquidCrystal_I2C.h se uploadeo desde un zip, se puede encontrar en GitHub
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración de pines y componentes

#define TRIG1 2 // Sensor HC-SR04 entrada (TRIG)
#define ECHO1 3 // Sensor HC-SR04 entrada (ECHO)
#define TRIG2 4 // Sensor HC-SR04 salida (TRIG)
#define ECHO2 5 // Sensor HC-SR04 salida (ECHO)
#define BUZZER1 8 // Pin del buzzer pasivo
#define BUZZER2 9
#define DISTANCIA_UMBRAL 5 // Umbral para detectar coches en cm

Servo servoEntrada;
Servo servoSalida;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definición de valiables y listas

int espaciosTotales = 10;
int espaciosDisponibles = 10;

int melody[] = {
    392, 392, 392, 349, 311,
};

int noteDurations[] = {
    220, 200, 200, 300, 300, 
};

int melody2[] = {
    294, 294, 311, 349,
};

int noteDurations2[] = {
    250, 250, 250, 300,
};

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
  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

  // Configuración de servos
  servoEntrada.attach(6); // Pin del servo de entrada
  servoSalida.attach(7); // Pin del servo de salida
  servoEntrada.write(0); // Barrera abajo
  servoSalida.write(0);  // Barrera abajo
}

void loop() {
  // Detección entrada

  if (detectarCoche(TRIG1, ECHO1) && espaciosDisponibles > 0) {
    reproducirMelodiaEntrada(); //Llama a mi función
    servoEntrada.write(90); // Levanta la barrera 90°
    delay(3000); // Espera en milisegundos
    servoEntrada.write(0); // Baja la barrera
    espaciosDisponibles--;
    actualizarPantalla(); //Llama a mi función
  }

  // Detección salida
  if (detectarCoche(TRIG2, ECHO2)) {
    if (espaciosDisponibles < espaciosTotales) {
      reproducirMelodiaSalida(); //Llama a mi función
      servoSalida.write(90); // Levanta la barrera
      delay(3000); // Espera en milisegundos
      servoSalida.write(0); // Baja la barrera
      espaciosDisponibles++;
      actualizarPantalla(); //Llama a mi función
    } else {
      mostrarErrorSalida(); //Llama a mi función
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
  return (distancia > 0 && distancia < DISTANCIA_UMBRAL); // Compara la distancia que detecta en sensor con la que establecí en mi definición de variables
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

void reproducirMelodiaEntrada() {
  // Melodía ascendente: pin pín pÍn
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    int noteDuration = noteDurations[i];
    
    if (melody[i] != 0) { // Si no es pausa
      tone(BUZZER1, melody[i], noteDuration);
    }
    delay(noteDuration * 1.3); // Pausa entre notas
    noTone(BUZZER1); // Detener el buzzer después de la nota
  }

  delay(1000); // Pausa entre repeticiones de la melodía
}

void reproducirMelodiaSalida() {
  for (int i = 0; i < sizeof(melody2) / sizeof(melody2[0]); i++) {
    int noteDuration2 = noteDurations2[i];
    
    if (melody2[i] != 0) { // Si no es pausa
      tone(BUZZER2, melody2[i], noteDuration2);
    }
    delay(noteDuration2 * 1.3); // Pausa entre notas
    noTone(BUZZER2); // Detener el buzzer después de la nota
  }
  delay(1000);
}

void mostrarErrorSalida() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("No puede salir");
  delay(2000);
  actualizarPantalla();
}
