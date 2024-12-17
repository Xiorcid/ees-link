
#include "utilities.h"

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialMon Serial
#define SerialAT Serial1

#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN "2618"

#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <Ticker.h>
#include <GyverPortal.h>
#include <xy6020l.h>
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


EspSoftwareSerial::UART SerialXY;

GyverPortal ui;

xy6020l xy(SerialXY, 0x01, 50, XY6020_OPT_SKIP_SAME_HREG_VALUE | XY6020_OPT_NO_HREG_UPDATE);

TinyGsm        modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

GyverDB db;

DB_KEYS(keys,
    key1,
    arr  // последнюю запятую не ставим
);

float VBat = 0;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len){
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, len);
    SerialMon.println();
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
    return mqtt.connected();
}

void setup(){
  Serial.begin(115200);
  SerialXY.begin(115200, SWSERIAL_8N1, 33, 34, false);

  mqtt_init();
  mqtt.setCallback(mqttCallback);

  gps_init();

  portal_init();
}

void loop(){
  static uint32_t tmr;
  static uint32_t gps_tmr;

  mqtt_update();
  if (millis() - gps_tmr > 5000){
    gps_update();
    gps_tmr = millis();
  }
  xy_update_test();

  xy.task();

  ui.tick();

  if (millis() - tmr > 2000){
    char message[100];
    sprintf(message, "{\"Id\":1, \"Gyro\":[{\"X\": %d, \"Y\": %d, \"Z\": %d}]}", analogRead(32), analogRead(33), analogRead(34));
    mqtt.publish(topicStatus, message);
    tmr = millis();
  }

}



