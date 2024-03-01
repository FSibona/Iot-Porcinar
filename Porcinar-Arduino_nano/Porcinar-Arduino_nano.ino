#include <Wire.h>

//link caudalimetro 1": https://isentrol.en.made-in-china.com/product/yBgEWiSOrDRV/China-BSPP-Usc-HS10ta-Brass-Flow-Sensor-Water-G1-3-3-26-4V.html

//float caudal = 0.7;
float presion = 0.3;
float conductividad = 2.0;
//Designaciones caudalimetro
volatile int flow_frequency;   // Measures flow sensor pulsesunsigned
float l_hour;                    // Calculated litres/hour
unsigned char flowsensor = 2;  // Sensor Input - 4--> D2 en wemos
unsigned long currentTime;
unsigned long cloopTime;

void flow()  // Interrupt function
{
  flow_frequency++;
}

void setup() {
  Serial.begin(9600);
  //Codigo de caudalimetro
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);  // Optional Internal Pull-Up
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING);
  sei();  // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
}

void loop() {
 
  // Every second, calculate and print litres/hour
  currentTime = millis();
  if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 / 4.8);  //  Calculation Formula: F(Hz)= 4.8*flow rate(Q)(l/min) ±10%
    flow_frequency = 0;                    // Reset Counter
    Serial.print(l_hour, 2);             // Print litres/hour
    Serial.print(",");
    Serial.print(presion);
    Serial.print(",");
    Serial.print(conductividad);
    Serial.println();

  }
}

