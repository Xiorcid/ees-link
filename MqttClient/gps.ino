void gps_init(){
  modem.sendAT("+SIMCOMATI");
  modem.waitResponse();

  Serial.println("Enabling GPS/GNSS/GLONASS");
  while (!modem.enableGPS(MODEM_GPS_ENABLE_GPIO, MODEM_GPS_ENABLE_LEVEL)) {
      Serial.print(".");
  }
  Serial.println();
  Serial.println("GPS Enabled");

  modem.setGPSBaud(115200);
}

void gps_update(){
  float lat2      = 0;
  float lon2      = 0;
  float speed2    = 0;
  float alt2      = 0;
  int   vsat2     = 0;
  int   usat2     = 0;
  float accuracy2 = 0;
  int   year2     = 0;
  int   month2    = 0;
  int   day2      = 0;
  int   hour2     = 0;
  int   min2      = 0;
  int   sec2      = 0;
  uint8_t    fixMode   = 0;
  for (;;) {
      Serial.println("Requesting current GPS/GNSS/GLONASS location");
      if (modem.getGPS(&fixMode, &lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                        &year2, &month2, &day2, &hour2, &min2, &sec2)) {

          Serial.print("FixMode:"); Serial.println(fixMode);
          Serial.print("Latitude:"); Serial.print(lat2, 6); Serial.print("\tLongitude:"); Serial.println(lon2, 6);
          Serial.print("Speed:"); Serial.print(speed2); Serial.print("\tAltitude:"); Serial.println(alt2);
          Serial.print("Visible Satellites:"); Serial.print(vsat2);

          // GPS_BuiltIn cannot get the number of satellites in use, so it always returns 0
          Serial.print("\tUsed Satellites:"); Serial.println(usat2);
          Serial.print("Accuracy:"); Serial.println(accuracy2);

          Serial.print("Year:"); Serial.print(year2);
          Serial.print("\tMonth:"); Serial.print(month2);
          Serial.print("\tDay:"); Serial.println(day2);

          Serial.print("Hour:"); Serial.print(hour2);
          Serial.print("\tMinute:"); Serial.print(min2);
          Serial.print("\tSecond:"); Serial.println(sec2);
          break;
      } else {
          Serial.println("Couldn't get GPS/GNSS/GLONASS location.");
      }
  }
  Serial.println("Retrieving GPS/GNSS/GLONASS location again as a string");
  String gps_raw = modem.getGPSraw();
  Serial.print("GPS/GNSS Based Location String:");
  Serial.println(gps_raw);
  Serial.println("Disabling GPS");

  modem.disableGPS();
}