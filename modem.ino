
void encryptData(uint8_t *plaintext, uint8_t *ciphertext) {
  uint8_t iv[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                     0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78 };
  uint8_t tag[16] = { 0x00 };
  uint8_t key[16] = "Yepla";
  uint8_t authdata[20] = { 0x00 };

  // Encrypt
  eax.setKey(key, sizeof(key));
  eax.setIV(iv, sizeof(iv));
  eax.addAuthData(authdata, sizeof(authdata));
  eax.encrypt(ciphertext, plaintext, sizeof(plaintext));
  eax.computeTag(tag, sizeof(tag));
}

void decryptData(uint8_t *plaintext, uint8_t *ciphertext) {
  uint8_t iv[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                     0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78 };
  uint8_t tag[16] = { 0x00 };
  uint8_t key[16] = "Yepla";
  uint8_t authdata[20] = { 0x00 };

  //Decrypt
  eax.setKey(key, sizeof(key));
  eax.setIV(iv, sizeof(iv));
  eax.addAuthData(authdata, sizeof(authdata));
  eax.decrypt(plaintext, ciphertext, sizeof(ciphertext));
  /*
  if (!eax.checkTag(tag, sizeof(tag))) {
    Serial.println("!! data is invalid");
    // The data was invalid - do not use it.
  }
  */
}

uint8_t makePayload(uint8_t *payloadType, uint8_t *channel, uint8_t *dest_id, uint8_t *payload) {
  return MSG_TYPE_POSITION + "," + DEVICE_ID + "," + msg_counter + "," + channel + "," + dest_id + "," + payload;
}

void sendMessage(uint8_t *fullMessage) {
  uint8_t iv[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                     0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78 };
  uint8_t tag[16] = { 0x00 };
  uint8_t key[16] = fullMessage;
  uint8_t authdata[20] = { 0x00 };

  //Decrypt
  eax.setKey(key, sizeof(key));
  eax.setIV(iv, sizeof(iv));
  eax.addAuthData(authdata, sizeof(authdata));
  eax.decrypt(plaintext, ciphertext, sizeof(ciphertext));
}


void receiveMessage() {
    heltec_led(30);  // 50% brightness is plenty for this LED
    rxFlag = false;
    radio.readData(rxdata);
    displayRxRawData()

    uint8_t plainText[20] = "";
    uint8_t encryptedData[20] = "";
    memcpy(encryptedData, rxdata.c_str(), min(20, (int)rxdata.length()));
    Serial.println("1Plaintext");
    Serial.println((const char *)plainText);
    Serial.println("1EncryptedData");
    Serial.println((const char *)encryptedData);
    decryptData(plainText, encryptedData);
    Serial.println("2Plaintext");
    Serial.println((const char *)plainText);

    Serial.println("2EncryptedData");
    Serial.println((const char *)encryptedData);

    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("RX [%s]\n", plainText);
      both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      both.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
    heltec_led(0);
}