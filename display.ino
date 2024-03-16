

void displayInfo() {
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  float vbat = heltec_vbat();
  float temperature = temperatureRead();
  float vbatPerc = heltec_battery_percent();
  display.printf("BAT: %.0f%% %.2fV CPU:%.0f°C\n", vbatPerc, vbat, temperature);
}

void displayHome() {
  display.clear();
  display.drawString(0, 0, "-> Send message");
  display.drawString(0, 15, "Received messages");
  display.drawString(0, 30, "Config");
  display.drawString(0, 45, "Tools");
  display.display();
}

void displaySendMsg() {
  display.clear();
  display.drawString(0, 0, " Send message:");
  display.drawString(0, 8, "----------------------------");
  display.drawString(0, 20, "Hello!");
  display.drawString(0, 50, "-> SEND");
  display.display();
}

void displayRxRawData() {
  Serial.println("rxdata");
  Serial.println(rxdata);
  Serial.println("rxdata length");
  Serial.println(rxdata.length());
}