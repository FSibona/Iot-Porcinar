/*Envio de mail correcto
  Reconexion a wifi automatico
  Recepcion de datos I2c correcta

 * EMailSender library for Arduino, esp8266 and esp32
 * Simple esp8266 Gmail send example
 *
 * https://www.mischianti.org/category/my-libraries/emailsender-send-email-with-attachments/
 *
 */

#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

//Designaciones WIFI
const char* ssid = "Galaxy S20 FE4927";  //<-----COLOCAR NOMBRE DE RED WIFI
const char* password = "heladera";       //<-----CONTRASEÑA DE RED WIFI
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

//EMailSender emailSend("fsiotservice@gmail.com", "busherujqbsxcnfl");
EMailSender emailSend("iotporcinar@gmail.com", "eduualkoihpwpsbd");
float caudal, presion, conductividad;
bool send_mail = 0;
String data;
unsigned long lastEmailSent = 0;

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr) {
  static uint16_t attempt = 0;
  Serial.print("Connecting to ");
  if (nSSID) {
    WiFi.begin(nSSID, nPassword);
    Serial.println(nSSID);
  }

  uint8_t i = 0;
  uint32_t startTime = millis();  // Guarda el tiempo de inicio

  while (WiFi.status() != WL_CONNECTED && i++ < 50) {
    if (millis() - startTime >= 200) {  // Comprueba si han pasado 200 ms
      startTime = millis();             // Actualiza el tiempo de inicio
      Serial.print(".");
    }
  }
  ++attempt;
  Serial.println("");
  if (i == 51) {
    Serial.print("Connection: TIMEOUT on attempt: ");
    Serial.println(attempt);
    if (attempt % 2 == 0)
      Serial.println("Check if access point available or SSID and Password\r\n");
    return false;
  }
  Serial.println("Connection: ESTABLISHED");
  Serial.print("Got IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void Awaits() {
  uint32_t ts = millis();
  while (!connection_state) {
    delay(50);
    if (millis() > (ts + reconnect_interval) && !connection_state) {
      connection_state = WiFiConnect();
      ts = millis();
    }
  }
}

void setup() {
  Serial.begin(9600);

  //Codigo de wifi
  connection_state = WiFiConnect(ssid, password);
  if (!connection_state)  // if not connected to WIFI
    Awaits();             // constantly trying to connect


}

void loop() {

// Si send_mail es igual a 1 y ha pasado un día desde el último correo, envía el correo electrónico
if (send_mail == 1 && millis() - lastEmailSent >= 24*60*60*1000) {
  EMailSender::EMailMessage message;
  message.subject = "Alerta: conductividad alta";
  message.message = "Equipo Porcinar. La conductividad del equipo de osmosis ha superado los 950 ppm";

  const char* arrayOfEmail[] = {"iotporcinar@gmail.com", "facusibona97@gmail.com", "fsiotservice@gmail.com"};//<----AGREGAR MAILS DESEADOS
  EMailSender::Response resp = emailSend.send(arrayOfEmail, 3, message); // <-----CAMBIAR EL NUMERO POR LA CANTIDAD DE MAILS EN EL ARRAY
  //EMailSender::Response resp = emailSend.send("iotporcinar@gmail.com", message); //<----- DESCOMENTAR ESTA LINEA Y COMENTAR LAS DOS DE ARRIBA PARA ENVIAR MAIL A UN SOLO DESTINATARIO
  if (resp.status == 1) {
    Serial.println("Correo enviado correctamente.");
    // Actualiza la última vez que se envió el correo
    lastEmailSent = millis();
  } else {
    Serial.println("Error al enviar el correo.");
  }
}

if (Serial.available()) {
    String data = Serial.readStringUntil('\n');

    // Extraer el primer valor (l_hour)
    int idx1 = data.indexOf(',');
    caudal = data.substring(0, idx1).toFloat();

    // Extraer el segundo valor (l_hour2)
    data = data.substring(idx1 + 1);
    int idx2 = data.indexOf(',');
    float caudal2 = data.substring(0, idx2).toFloat();

    // Extraer el tercer valor (caudal_total)
    data = data.substring(idx2 + 1);
    int idx3 = data.indexOf(',');
    float caudal_total = data.substring(0, idx3).toFloat();

    // Extraer el cuarto valor (ppm)
    data = data.substring(idx3 + 1);
    int idx4 = data.indexOf(',');
    float conductividad = data.substring(0, idx4).toFloat();

    // Extraer el quinto valor (send_mail)
    data = data.substring(idx4 + 1);
    send_mail = (data.toInt() == 1);

    // Imprimir los valores leídos
    Serial.print("Litres/hour 1: ");
    Serial.println(caudal);
    Serial.print("Litres/hour 2: ");
    Serial.println(caudal2);
    Serial.print("Caudal total: ");
    Serial.println(caudal_total);
    Serial.print("PPM: ");
    Serial.println(conductividad);
    Serial.print("Send Mail: ");
    Serial.println(send_mail);
  }

}
