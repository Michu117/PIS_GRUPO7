#include <Servo.h>

// Definición de Servos
Servo servohorizontal;
int servox = 0;
int servohLimitHigh = 180; // Ajustado para 360 grados
int servohLimitLow = 20;

Servo servovertical;
int servoy = 0;
int servovLimitHigh = 180;
int servovLimitLow = 20;

// Asignar LDRs
int ldrtopl = A2;
int ldrtopr = A1;
int ldrbotl = A3;
int ldrbotr = A0;

// Pin del switch
const int switchPin = 2;
int switchState = 0;

void setup() {
  servohorizontal.attach(10);
  servohorizontal.write(0); 
  servovertical.attach(9);
  servovertical.write(0); 
  delay(500);
  delay(500);
  pinMode(switchPin, INPUT_PULLUP);  // Configura el pin del switch como entrada con resistencia pull-up interna
  Serial.begin(9600);   
}

void loop() {
  switchState = digitalRead(switchPin);

  if (switchState == HIGH) {
    modoFotorresistencias();
  } else {
    modoAzimutAngulo();
  }
}

void modoFotorresistencias() {
  Serial.println("Modo Fotorresistencias");
  servox = servohorizontal.read();
  servoy = servovertical.read();
  
  // Lectura de los valores de los LDRs
  int topl = analogRead(ldrtopl);
  int topr = analogRead(ldrtopr);
  int botl = analogRead(ldrbotl);
  int botr = analogRead(ldrbotr);
  
  // Promedios de Luz
  int avgtop = (topl + topr) / 2;
  int avgbot = (botl + botr) / 2;
  int avgleft = (topl + botl) / 2;
  int avgright = (topr + botr) / 2;

  // Control vertical
  if (avgtop > avgbot) {
    servoy = servoy + 1;
    if (servoy > servovLimitHigh) { 
      servoy = servovLimitHigh;
    }
  } else if (avgbot > avgtop) {
    servoy = servoy - 1;
    if (servoy < servovLimitLow) {
      servoy = servovLimitLow;
    }
  }
  servovertical.write(servoy);
  
  // Control horizontal
  if (avgleft > avgright) {
    servox = servox - 1;
    if (servox < servohLimitLow) {
      servox = servohLimitLow;
    }
  } else if (avgright > avgleft) {
    servox = servox + 1;
    if (servox > servohLimitHigh) {
      servox = servohLimitHigh;
    }
  }
  servohorizontal.write(servox);
  delay(50);
}

void modoAzimutAngulo() {
  Serial.println("Modo Azimut y Angulo Solar");

  // Verificar si hay datos disponibles en el puerto serial
  if (Serial.available() > 0) {
    // Leer los datos desde el puerto serial
    String data = Serial.readStringUntil('\n'); // Leer una línea completa hasta el salto de línea
    
    // Separar los valores
    int spaceIndex = data.indexOf(' ');
    if (spaceIndex != -1) {
      String val1Str = data.substring(0, spaceIndex);
      String val2Str = data.substring(spaceIndex + 1);

      // Convertir los valores a enteros
      servox = val1Str.toInt();
      servoy = val2Str.toInt();

      // Mostrar los valores recibidos para depuración
      Serial.print("Recibido: ");
      Serial.print(servoy);
      Serial.print(" ");
      Serial.println(servox);

      // Asegurarse de que los ángulos estén dentro del rango válido
      servox = constrain(servox, servohLimitLow, servohLimitHigh);
      servoy = constrain(servoy, servovLimitLow, servovLimitHigh);

      // Actualizar los ángulos de los servos
      servohorizontal.write(servoy);
      servovertical.write(servox);
    } else {
      // Mensaje de error si no se pudo separar correctamente
      Serial.println("Error: Datos recibidos no válidos");
    }
  }
}
