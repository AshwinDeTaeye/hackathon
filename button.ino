
void handleButtons() {
  if (menu == DSP_SEND_MESSAGE && button.isDoubleClick()) {
    menu = DSP_HOME;
  }
  
  // Transmit a packet every PAUSE seconds or when the button is pressed
  if ((PAUSE && millis() - last_tx > (PAUSE * 200)) || button.isDoubleClick()) {
    if (menu == DSP_HOME) {
      menu = DSP_SEND_MESSAGE;
      displayHome();
    }
  }
  if (menu == DSP_SEND_MESSAGE && button.isSingleClick()) {
    displaySendMsg();
  }
  if (menu == DSP_SEND_MESSAGE && submenu == 1 && button.isSingleClick()) {
    display.clear();
    heltec_led(30);  // 50% brightness is plenty for this LED
    tx_time = millis();

    uint8_t plainText[20] = "Hello!";

    uint8_t encryptedData[20] = "";
    encryptData(plainText, encryptedData);
    char encryptedDataStr[21];  // one extra byte for the null terminator
    memcpy(encryptedDataStr, encryptedData, 20);
    encryptedDataStr[20] = '\0';  // null terminator
    RADIOLIB(radio.transmit(String(counter++).c_str()));
    tx_time = millis() - tx_time;
    display.drawString(0, 20, "Meassage send!");
    display.display();
    delay(2000);
    button.update();
    display.clear();
    display.drawString(0, 20, "        ");
    display.display();
    menu = 0;
    submenu = 0;
  }
  if (submenu == 0 && button.isSingleClick()) {
    submenu = 1;
  }

  if ((PAUSE && millis() - last_tx > (PAUSE * 200)) || button.isSingleClick() && menu != 1) {
    // In case of button click, tell user to wait
    both.printf("\nTX [%s] ", String(counter).c_str());
    radio.clearDio1Action();
    heltec_led(30);  // 50% brightness is plenty for this LED
    tx_time = millis();
    uint8_t plainText[20] = "Test";
    uint8_t encryptedData[20] = "";
    encryptData(plainText, encryptedData);
    Serial.println("encryptedData SEND:");
    Serial.println((const char *)encryptedData);
    RADIOLIB(radio.transmit((const char *)encryptedData));
    tx_time = millis() - tx_time;
    heltec_led(0);
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      display.normalDisplay();
      both.printf("OK (%i ms)\n", tx_time);
      float vbat = heltec_vbat();
      both.printf("Battery: %.2f V\n\n", vbat);

    } else {
      both.printf("fail (%i)\n", _radiolib_status);
    }
    // Maximum 1% duty cycle
    minimum_pause = tx_time * 10;
    last_tx = millis();
    radio.setDio1Action(rx);
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }

}