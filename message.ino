
void sendPosition() {
  uint8_t position = "3.15646:1.1843123"; // get position
  uint8_t message = makePayload(MSG_TYPE_POSITION, position); // make message
  sendMessage(message); // send message
}

void sendText(uint8_t *text) {
  uint8_t message = makePayload(MSG_TYPE_TEST, text);
  sendMessage(message);
}