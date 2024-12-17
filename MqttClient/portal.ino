void portal_init(){
  IPAddress apIP(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, subnet);
  WiFi.softAP(AP_SSID, AP_PASS);

  Serial.print("Admin IP: ");
  Serial.println(WiFi.softAPIP());

  ui.attachBuild(build);
  ui.onlineTimeout(15000);
  ui.enableAuth(ADMIN, PASS);
  ui.attach(action);

  ui.start();
}


void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.TITLE("Welcome!");
    GP.BLOCK_BEGIN(GP_DIV, "250px", "MQTT SETTINGS");
      GP.FORM_BEGIN("/settings");
        GP.TEXT("server", "Broker");
        GP.NUMBER("port", "Port");
        GP.TEXT("user", "Username");
        GP.TEXT("pass", "Password");
        GP.SUBMIT("Save & reset");
      GP.FORM_END();
    GP.BLOCK_END();
  GP.BUILD_END();
}

void action() {
  if (ui.form()) {
    Serial.print("Submit form: ");
    if (ui.form("/settings")) dumpSettings();
  }
}

void dumpSettings(){
  Serial.println("/settings");
  Serial.print("Broker: ");
  Serial.println(ui.getString("server"));
  ESP.restart();
}