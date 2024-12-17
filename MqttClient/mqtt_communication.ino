void mqtt_init(){
    // Turn on DC boost to power on the modem
  pinMode(BOARD_POWERON_PIN, OUTPUT);
  digitalWrite(BOARD_POWERON_PIN, HIGH);

  // Set modem reset pin ,reset modem
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
  digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
  digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);

  // Turn on modem
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(1000);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);

  // Set modem baud
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

  Serial.println("Start modem...");
  delay(3000);
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.init()) {
      DBG("Failed to restart modem, delaying 10s and retrying");
      return;
  }

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);
  // Unlock your SIM card with a PIN if needed
  if (GSM_PIN && modem.getSimStatus() != 3) {
      modem.simUnlock(GSM_PIN);
  }

  SerialMon.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
      SerialMon.println(" fail");
      delay(10000);
      return;
  }
  SerialMon.println(" success");

  if (modem.isNetworkConnected()) {
      SerialMon.println("Network connected");
  }

  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      SerialMon.println(" fail");
      delay(10000);
      return;
  }
  SerialMon.println(" success");

  if (modem.isGprsConnected()) {
      SerialMon.println("GPRS connected");
  }

  // MQTT Broker setup
  mqtt.setServer(broker, 1883);
}

void mqtt_update(){
  // Make sure we're still registered on the network
  if (!modem.isNetworkConnected()) {
      SerialMon.println("Network disconnected");
      if (!modem.waitForNetwork(180000L, true)) {
          SerialMon.println(" fail");
          delay(10000);
          return;
      }
      if (modem.isNetworkConnected()) {
          SerialMon.println("Network re-connected");
      }

      // and make sure GPRS/EPS is still connected
      if (!modem.isGprsConnected()) {
          SerialMon.println("GPRS disconnected!");
          SerialMon.print(F("Connecting to "));
          SerialMon.print(apn);
          if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
              SerialMon.println(" fail");
              delay(10000);
              return;
          }
          if (modem.isGprsConnected()) {
              SerialMon.println("GPRS reconnected");
          }
      }
  }

  if (!mqtt.connected()) {
      SerialMon.println("=== MQTT NOT CONNECTED ===");
      // Reconnect every 10 seconds
      uint32_t t = millis();
      if (t - lastReconnectAttempt > 10000L) {
          lastReconnectAttempt = t;
          if (mqttConnect()) {
              lastReconnectAttempt = 0;
          }
      }
      delay(100);
      return;
  }

  mqtt.loop();
}