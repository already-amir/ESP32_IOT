
void setup() {
 

  attachInterrupt(digitalPinToInterrupt(pirPin), detectMotion, RISING);

  // اینجا MQTT و WiFi خودت را ستاپ کن
}

void loop() {
  // اینجا MQTT.loop() و سایر دستوراتی که نیاز داری را بنویس

  if (motionDetected) {
    motionDetected = false;
    alarmActive = true;
    alarmStartTime = millis();
    currentFreq = 1000;
    freqStep = 50;
  }

  
}
