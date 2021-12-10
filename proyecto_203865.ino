/*
  Proyecto final -Termo inteligente
  Sistemas Empotrados

  Diana Estefanía Castro Aguilar - 203865

*/
#include <NoDelay.h>
#include "DHT.h"

const unsigned int PIN_DHT = 2;
const unsigned int BAUD_RATE = 9600;
char response[11];
String comando = "off";
bool mensajeEnviado = false;

// Periodo en ms entre lecturas del DHT
const long PERIODO = 2000;
//Pines
const int PIN_LED_AZUL = 8;
const int PIN_LED_VERDE = 9;
const int PIN_LED_ROJO = 10;
const unsigned int PIN_RELEVADOR = 6;

// Crea una instancia de la clase noDelay
// que determina si han transcurrido PERIODO ms
noDelay pausa(PERIODO);
// Inicialice el sensor DHT.
DHT dht(PIN_DHT, DHT11);


void setup() {
  pinMode(PIN_RELEVADOR, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);

  Serial.begin(BAUD_RATE);
  digitalWrite(PIN_RELEVADOR, LOW);
  //Serial.println("Prueba sensor de temperatura DHT11: ");
  dht.begin();
}

int nivel;
int auxNivel = -1;
void loop() {

  // Verifica si es tiempo de leer el DHT. La lectura de la
  // temperatura y humedadtoma unos 250 ms. Depende del sensor
  if (pausa.update()) {
    leeHumedadTemperatura();
    nivel = analogRead(A0);
    Serial.println("niv:" + String(nivel));
  }

 if (Serial.available() > 0) {
    int n = Serial.readBytesUntil('\n', response, 10);
    response[n] = '\0';
    
    if (!strcmp(response, "on")) {
      comando = "on";
      Serial.println(response);
      mensajeEnviado = false;
    }
    
    if(!strcmp(response, "off")) {
      comando = "off";
      Serial.println(response);
      mensajeEnviado = false;
    }    
  }
  
}




/*
  Esta funcion lee la humedad, temperadura e indice de calor
  y los despliega en el monitor serial
*/
void leeHumedadTemperatura() {
  // Lee la humedad
  float h = dht.readHumidity();
  // Lee la temperatura en grados Celsius (por omision)
  float tc = dht.readTemperature();
  // Lee la temperatura en grados Fahrenheit
  //(isFahrenheit = true)
  float tf = dht.readTemperature(true);
  // Verifica si alguna lectura fallo y aborta
  // (para intentar de nuevo).
  if (isnan(h) || isnan(tc) || isnan(tf)) {
    //Serial.println("Fallo la lectura del sensor DHT!");
    return;

  }

  //Valida si la temperatura está en el rango de frío
  if (tc <= 18.99) {
    //Enciende solamente el color azul del LED RGB
    digitalWrite(PIN_LED_AZUL, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_VERDE, LOW);

    //Valida si la temperatura está en el rango de ambiente
  } else if (tc >= 19 && tc < 30.99) {
    //Enciende solamente el color verde del LED RGB
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);

    //Valida si la temperatura está en el rango de caliente
  } else if (tc >= 30) {
    //Enciende solamente el color verde del LED RGB
    digitalWrite(PIN_LED_ROJO, HIGH);
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);

    //En cualquier otro caso todos los colores del LED RGB se apagan
  } else {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_AZUL, LOW);
    digitalWrite(PIN_LED_ROJO, LOW);
  }


  // Valida si la temperatura es igual a la deseada
  if (comando == "on") {
    // Envia señal a relevador conectado al pin para que energice el LED
    digitalWrite(PIN_RELEVADOR, HIGH);
    // Escribe al puerto serie
    Serial.println("CALENTANDO...LED encendido");
  
  
  // Valida si la temperatura es diferente a la deseada para calentar
  }else if (comando == "off") {
    digitalWrite(PIN_RELEVADOR, LOW);
    // Escribe al puerto serie
    Serial.println("TEMPERATURA IDEAL...LED apagado");
  }  
 

//  if (comando == "apagar") {
//    if (!mensajeEnviado) {
//      Serial.println("apagar");
//      mensajeEnviado = true;
//    }
//  }


// Calcula el indice de calor en grados Fahrenheit
// (por omision)
float hif = dht.computeHeatIndex(tf, h);
// Calcula el indice de calor en grados Celsius
float hic = dht.computeHeatIndex(tc, h, false);
//Serial.print("Humedad: ");
//Serial.println(h);
//Serial.print("Temperatura: ");
Serial.println("tem:" + String(tc,2));
//Serial.print(" *C, ");
//Serial.print(tf);
//Serial.println(" *F.");
//Serial.print("Indice de calor: ");
//Serial.print(hic);
//Serial.print(" *C, ");
//Serial.print(hif);
//Serial.println(" *F\n");
}
