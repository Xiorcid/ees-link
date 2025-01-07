
#include "utilities.h"

#define TINY_GSM_MUX_BUFFER 1024
#define TINY_GSM_RX_BUFFER 1024
#define TINY_GSM_TX_BUFFER 1024
#define MQTT_MAX_PACKET_SIZE 512

#define SerialMon Serial
#define SerialAT Serial1

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN "2618"

#include <TinyGsmClient.h>
#include <PubSubClient.h>
//#include <SoftwareSerial.h>
// #include <HardwareSerial.h>
#include "CRC16.h"
#include "CRC.h"
#include <Ticker.h>
#include <GyverPortal.h>
// #include <xy6020l.h>
#include <GSON.h>
#include <GyverDB.h>


// Your GPRS credentials, if any
const char apn[]      = "internet.tele2.lv";
const char gprsUser[] = "";
const char gprsPass[] = "";

const char* AP_SSID     = "ESP32";
const char* AP_PASS = "123456789";

const char* ADMIN = "admin";
const char* PASS = "pass";

// MQTT details
const char *broker = "svaza.lv";
const int mqtt_port = 1883;
const char *mqtt_user = "esp32";
const char *mqtt_pass = "T-SIM7000G";
const char *mqtt_gsm_client_name = "lilygo";

const char *topicStatus       = "topic";
const char *topicInput       = "PSU";

const uint8_t CAR_ID = 1;


//EspSoftwareSerial::UART SerialXY;
// HardwareSerial SerialXY(2);

GyverPortal ui;

CRC16 crc;
//  xy6020l xy(Serial2, 0x01, 50, XY6020_OPT_SKIP_SAME_HREG_VALUE | XY6020_OPT_NO_HREG_UPDATE);

TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

GyverDB db;

DB_KEYS(keys,
    key1,
    arr  // последнюю запятую не ставим
);

struct PSU_REGs{
  uint16_t actOutVoltage;
  uint16_t actCurrent;
  uint16_t actPower;
  uint16_t actInVoltage;
  uint16_t actOutEnergy;
};
PSU_REGs psu;

float VBat = 0;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len){
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("]: ");
  SerialMon.write(payload, len);
  SerialMon.println();
  
  String pl;
  for (int i = 0; i < len; i++){
    pl += (char)(payload[i]);
  }
  parsePSUInputData(pl);
}

bool mqttConnect(){
  SerialMon.print("Connecting to ");
  SerialMon.print(broker);

  bool status = mqtt.connect(broker, mqtt_user, mqtt_pass);

  if (status == false) {
      SerialMon.println(" fail");
      return false;
  }
  SerialMon.println(" success");
  mqtt.publish(topicStatus, "Lilygo in network!");
  mqtt.subscribe(topicInput);
  return mqtt.connected();
}

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 18, 19);
  Serial2.setTimeout(10);
  setState(false);

  mqtt_init();
  mqtt.setCallback(mqttCallback);

  //gps_init();
  //getData();

  portal_init();
}

void loop(){
  static uint32_t tmr;
  static uint32_t gps_tmr;
  static bool flg;
  mqtt_update();
  if (millis() - gps_tmr > 2000){
    // gps_update();
    // setVoltage(1200);
    //setCurrent(50);
    

    // Serial.print("VBAT: ");
    // Serial.println(getAuxBatVoltage());
    // setVoltage(700);
    // setCurrent(50);
    psu = readPSURegisters();
    //setState(!flg);
    Serial.print(psu.actOutVoltage);
    Serial.print(" V ");
    Serial.print(psu.actCurrent);
    Serial.print(" A ");
    Serial.print(psu.actPower);
    Serial.print(" W ");
    Serial.print(psu.actInVoltage);
    Serial.print(" V ");
    Serial.print(psu.actOutEnergy);
    Serial.println(" Wh");
    flg = !flg;
    
    gps_tmr = millis();
  }

  if (Serial2.available()){
    Serial.print(Serial2.read(), HEX);
    Serial.print(" ");
  }

  if(Serial.available()){
    setVoltage(Serial.parseInt());
  }
  // xy_update_test();

  // xy.task();

  ui.tick();

  if (millis() - tmr > 2000){
    char message[256];
    buildPSUTelemetryPackage().toCharArray(message, 100);
    mqtt.publish(topicStatus, message);
    tmr = millis();
  }

}



