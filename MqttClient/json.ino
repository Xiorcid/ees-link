String buildPSUTelemetryPackage(){
  gson::string gs;
  gs.beginObj();
  gs["Type"] = (uint8_t)0;
  gs["Id"] = (uint8_t)CAR_ID;
    gs.beginObj("PSU");
      gs["Uop"] = (uint16_t)psu.actOutVoltage;
      gs["Iop"] = (uint16_t)psu.actCurrent;
      gs["Pop"] = (uint16_t)psu.actPower;
      gs["Uip"] = (uint16_t)psu.actInVoltage;
      gs["Wh"] = (uint16_t)psu.actOutEnergy;
    gs.endObj();
  gs.endObj();
  gs.end();
  Serial.println(gs);
  return String(gs);
}

String buildGPSTelemetryPackage(){
  gson::string gs;
  gs.beginObj();
  gs["Type"] = (uint8_t)1;
  gs["Id"] = (uint8_t)CAR_ID;
    gs.beginObj("GPS");
      gs["Lat"] = (double)56.9496;
      gs["Lon"] = (double)24.1052;
    gs.endObj();
  gs.endObj();
  gs.end();
  Serial.println(gs);
  return String(gs);
}

void parsePSUInputData(String data){
  gson::Parser p;
  p.parse(data);
  if (p["Id"] == CAR_ID){
    setVoltage((uint16_t)p["PSU"]["U"]);
    setCurrent((uint16_t)p["PSU"]["I"]);
    setState((bool)p["PSU"]["St"]);
  }
}
