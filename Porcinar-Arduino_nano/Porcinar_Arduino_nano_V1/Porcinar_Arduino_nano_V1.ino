// Programa cargado el 20/03/2024 en PLC con Wifi para Porcinar (BsAS)
//Modificaciones en PLC: Se anuló la entrada 2 (Pin D3) de 4-20mA para ser usada como pin digital para el caudalimetro 2
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//link caudalimetro 1": https://isentrol.en.made-in-china.com/product/yBgEWiSOrDRV/China-BSPP-Usc-HS10ta-Brass-Flow-Sensor-Water-G1-3-3-26-4V.html
//link caudalimetro 3/4": https://www.seeedstudio.com/Water-Flow-Sensor-YF-B5-p-2882.html 

LiquidCrystal_I2C lcd(0x27, 20, 4);
//Designaciones conductimetro
#define ANALOG_PIN A2  // Pin analógico al que está conectada la señal de corriente (conductimetro)
#define RESISTOR_VALUE 250  // Valor del resistor de carga en ohmios del bucle de corriente
#define RELAY_PIN 5  // Pin digital al que está conectado el relé

// Variables para el control de tiempo
unsigned long startTime;  // Tiempo de inicio de la medición
const unsigned long interval = 7200000;  // Intervalo de 2 horas en milisegundos (2 horas * 60 minutos * 60 segundos * 1000 milisegundos)
// Coeficientes de la relación lineal
const float m = 500.0;  // Pendiente de la relación (ppm por cada cambio en voltaje)
const float b = -500.0; // Término de intersección (concentración en ppm cuando el voltaje es cero)

//Designaciones caudalimetro 1
volatile int flow_frequency;   // Measures flow sensor pulsesunsigned
float l_hour;                    // Calculated litres/hour
unsigned char flowsensor = 2;  // Sensor Input - 4--> D2 en wemos
unsigned long currentTime;
unsigned long cloopTime;
//Designaciones caudalimetro 2 
volatile int flow_frequency2;   // Measures flow sensor pulsesunsigned
float l_hour2;                    // Calculated litres/hour
unsigned char flowsensor2 = 3;

bool send_mail = false; // Variable para decirle a wemos que envie el mail por alta conductividad

void flow()  // Interrupt function
{
  flow_frequency++;
}
void flow2()  // Interrupt function
{
  flow_frequency2++;
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  //Codigo de caudalimetro
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);  // Optional Internal Pull-Up
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING);
  //Codigo de caudalimetro 2
  pinMode(flowsensor2, INPUT);
  digitalWrite(flowsensor2, HIGH);  // Optional Internal Pull-Up
  attachInterrupt(digitalPinToInterrupt(flowsensor2), flow2, RISING);
  sei();  // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  //Codigo de Rele
  pinMode(RELAY_PIN, OUTPUT);  // Configura el pin del relé como salida
  digitalWrite(RELAY_PIN, HIGH);  // Apaga el relé al inicio
  startTime = millis();  // Guarda el tiempo de inicio
}

void loop() {
 
  // Every second, calculate and print litres/hour
  currentTime = millis();
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 4.8);  //  Calculation Formula: F(Hz)= 4.8*flow rate(Q)(l/min) ±10%
    flow_frequency = 0;                    // Reset Counter
    l_hour2 = (flow_frequency2 * 60 / 6.6);  //  Calculation Formula: F(Hz)= 6.6*Q = L/MIN ±3%
    flow_frequency2 = 0;                    // Reset Counter

    float caudal_total = l_hour + l_hour2;

    // Lee el valor del pin analógico (CONDUCTIVIDAD)
    int sensorValue = analogRead(ANALOG_PIN);
    // Convierte el valor leído a voltaje (suponiendo 5V de referencia y 10 bits de resolución)
    float voltage = sensorValue * (5.0 / 1023.0);
    // Calcula la concentración en ppm
    float ppm = m * voltage + b;

    // Verifica si la concentración es mayor a 950 durante el intervalo de 2 horas
    if (ppm > 950 && millis() - startTime >= interval) {
      // Activa la salida a relé
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Concentracion alta durante 2 horas, activando rele");
      lcd.clear();
      //Renglon 1
      lcd.setCursor(0, 0);
      lcd.print("QEquipo: ");
      lcd.setCursor(10, 0);
      lcd.print(l_hour,1);
      lcd.setCursor(16, 0);
      lcd.print(" L/h");
      //Renglon 2
      lcd.setCursor(0, 1);
      lcd.print("QCrudo: ");
      lcd.setCursor(10, 1);
      lcd.print(l_hour2,1);
      lcd.setCursor(16, 1);
      lcd.print(" L/h");
      //Renglon 3
      lcd.setCursor(0, 2);
      lcd.print("QTotal: ");
      lcd.setCursor(10, 2);
      lcd.print(caudal_total,1);
      lcd.setCursor(16, 2);
      lcd.print(" L/h");
      //Renglon 4
      lcd.setCursor(0, 3);
      lcd.print("Conductividad ALTA!");
      send_mail = true; // Avisa a wemos que debe enviar el mail por alta conductividad
    }
      // Verifica si la concentración es menor a 950
    if (ppm < 950) {
      // Reinicia el tiempo de inicio
      startTime = millis();      
      // Apaga la salida a relé
      digitalWrite(RELAY_PIN, HIGH);
      //Serial.println("Concentracion inferior a 1150 ppm, desactivando rele");
      lcd.clear();
      //Renglon 1
      lcd.setCursor(0, 0);
      lcd.print("QEquipo: ");
      lcd.setCursor(10, 0);
      lcd.print(l_hour,1);
      lcd.setCursor(16, 0);
      lcd.print(" L/h");
      //Renglon 2
      lcd.setCursor(0, 1);
      lcd.print("QCrudo: ");
      lcd.setCursor(10, 1);
      lcd.print(l_hour2,1);
      lcd.setCursor(16, 1);
      lcd.print(" L/h");
      //Renglon 3
      lcd.setCursor(0, 2);
      lcd.print("QTotal: ");
      lcd.setCursor(10, 2);
      lcd.print(caudal_total,1);
      lcd.setCursor(16, 2);
      lcd.print(" L/h");
      //Renglon 4
      lcd.setCursor(0, 3);
      lcd.print("Conductividad OK");
      send_mail = false;
    }
    // Imprime la concentración en ppm
    /*Serial.print("Concentracion: ");
    Serial.print(ppm);
    Serial.println(" ppm");
    Serial.print("Voltaje: ");
    Serial.print(voltage);
    Serial.println(" V");
    */
    Serial.print(l_hour);             // Print litres/hour
    Serial.print(",");
    Serial.print(l_hour2);
    Serial.print(",");
    Serial.print(caudal_total);
    Serial.print(",");
    Serial.print(ppm);
    Serial.print(",");
    Serial.print(send_mail);
    Serial.println();
    
  }
   

}

