// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "efb4ce2e-d238-4cec-a537-8df83cfa9466";//ID: iotporcinar

const char SSID[]               = "Galaxy S20 FE4927";    // Network SSID (name)
const char PASS[]               = "heladera";    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = "9NKaNA4U6VXoj2QnCsqBNuoEi";    // Secret device password generado por Arduino Cloud


float caudal;
float caudal2;
float caudal_total;
float conductividad;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(caudal, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(caudal2, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(caudal_total, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(conductividad, READ, ON_CHANGE, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
