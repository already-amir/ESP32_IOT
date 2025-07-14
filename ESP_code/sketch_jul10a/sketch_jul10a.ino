#include <IRremote.h>

#define IR_RECEIVE_PIN 14

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("IR Receiver ready");
}

void loop() {
  if (IrReceiver.decode()) {
    //Serial.print("IR Code: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    // مثال: کنترل LED
    if (IrReceiver.decodedIRData.decodedRawData == 0xFFA25D) {
      digitalWrite(33, HIGH);
    }

    IrReceiver.resume();
  }
}
