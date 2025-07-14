#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 13;  // پایه اتصال سنسور مادون قرمز

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // شروع دریافت IR
  Serial.println("منتظر دریافت سیگنال مادون قرمز...");
  pinMode(2, OUTPUT);
}

void loop() {
  if (IrReceiver.decode()) {
    // چاپ مقدار دریافتی به صورت هگز
    Serial.print("کد دریافتی: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    uint8_t command = IrReceiver.decodedIRData.command;
    switch (command) {
      case 0xC:  // دکمه A
        digitalWrite(2, HIGH);
        Serial.println("LED روشن شد");
        break;
      case 0x16:  // دکمه B
        digitalWrite(2, LOW);
        Serial.println("LED خاموش شد");
        break;
      case 0x47:  // دکمه C
        Serial.println("دکمه C فشرده شد!");
        break;
      default:
        Serial.println("کلید تعریف‌نشده");
        break;
    }

    IrReceiver.resume();  // آماده‌سازی برای دریافت بعدی
  }
}
