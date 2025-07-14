#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <DHT.h>
#include <IRremote.hpp>
#define n 10





int inp[n]={23};
int out[n]={2,13,12,5,33,32,13};

//modem
const char* ssid = "dlink";
const char* password = "43453036";

//mqtt blocker
const char* mqtt_server = "192.168.0.10";
uint16_t mqtt_port= 1883;

// DHT
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastDHTTime = 0;
const long DHTInterval = 5000;

//RGB
const int freq = 5000;
const int resolution = 8;

//PIR
volatile bool motionDetected = false;
bool alarmActive = false;
unsigned long alarmStartTime = 0;
const unsigned long alarmDuration = 5000; 
unsigned long lastToneChange = 0;
const unsigned long toneChangeInterval = 20;
int currentFreq = 1000;
int freqStep = 50;
bool alarm_activation=false;
bool last_mot=false;

WiFiClient espClient;
PubSubClient client(espClient);



void IRAM_ATTR detectMotion() {
  
  int state=digitalRead(23);
  if (state==HIGH){
    digitalWrite(2,HIGH);
    alarmStartTime = millis();
    motionDetected=true;
    
  }
  else{
    digitalWrite(2,LOW);
    motionDetected=false;
  }
  
}



void led_controle(String msg){

    int pin;

    if (msg[0]=='1'){

      pin=33;
    }
    else if(msg[0]=='2'){

      pin=32;
    }
    else if (msg[0]=='3'){

      pin=13;
    }
    else if (msg[0]=='4'){

      pin=12;
    }
    // you can add even more leds (dont forget to initialize the pins in setup ;)
   
    if (msg[2] == '1') {

      digitalWrite(pin, HIGH);
      
    } 
    else if (msg[2] == '0') {

      digitalWrite(pin, LOW);
    }
    
}

void slidebar(int pin,String msg){

  int value = msg.toInt();
 
  ledcWrite(pin, value);
    
}

void rgb(String msg){

}





void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  String top=String(topic);

  for (int i=0;i<length;i++){
    msg+=(char)payload[i];
  }

  if (top == "onnoff") {

    led_controle(msg);
  }
  else if (top == "alarm_on"){
    if (msg=="1"){
      alarm_activation=true;
    }
    else{
      alarm_activation=false;
    }
  }
 
  else if (top == "red"){
    slidebar(25,msg);
  }
   else if (top == "green"){
    slidebar(26,msg);
  }
   else if (top == "blue"){
    slidebar(27,msg);
  }
  // you can add more


}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("esp32client")) {
      Serial.println("MQTT connected!");
      client.subscribe("onnoff");
      client.subscribe("alarm_on");

      

      client.subscribe("red");
      client.subscribe("green");
      client.subscribe("blue");
      ///oooooooooooooooooooooooo
    } else {
      delay(5000);
    }
  }
}


void setup(){
  Serial.begin(115200);
  dht.begin();

  for (int i=0;i<n;i++){
    pinMode(inp[i],INPUT);
    pinMode(out[i],OUTPUT);
  }
  ledcAttach(25, freq, resolution);
  ledcAttach(26, freq, resolution);
  ledcAttach(27, freq, resolution);
  
  attachInterrupt(digitalPinToInterrupt(23), detectMotion, CHANGE);

  IrReceiver.begin(14, ENABLE_LED_FEEDBACK); 


  WiFi.begin(ssid,password);
  while (WiFi.status()!=WL_CONNECTED){
    delay(500);
  }
  Serial.println("wifi connected");
  client.setServer(mqtt_server,mqtt_port);
  client.setCallback(callback);

}


void loop(){
  
  if (!client.connected()){
    Serial.println("reconnecting ...");
    reconnect();
  }
  client.loop();

  if (IrReceiver.decode()) {
    
   
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    uint8_t command = IrReceiver.decodedIRData.command;
    switch (command) {
      case 0x16: 
        digitalWrite(12, HIGH);
        break;
      case 0xC:  
        digitalWrite(12, LOW);
        break;


      case 0x19: 
        digitalWrite(33, HIGH);
        break;
      case 0x18: 
        digitalWrite(33, LOW);
        break;


      case 0xD: 
        digitalWrite(32, HIGH);
        break;
      case 0x5E: 
        digitalWrite(32, LOW);
        break;

      
      case 0x8: 
        digitalWrite(2, HIGH);
        break;
      case 0x42: 
        digitalWrite(2, LOW);
        break;

      case 0x1C: 
        digitalWrite(2, HIGH);
        break;
      case 0x52: 
        digitalWrite(2, LOW);
        break;

      case 0x7: 
        alarm_activation=false;
        break;
      case 0x15: 
        alarm_activation=true;
        break;

      

      
      default:
        break;
    }

    IrReceiver.resume(); 
  }



  if (motionDetected ) {
    
    if (!last_mot==motionDetected){
      client.publish("alarm", "high");
      alarmActive = true;
      currentFreq = 1000;
      freqStep = 50;

    }
    
  }
  else{
    if (!last_mot==motionDetected){
       client.publish("alarm", "low");
    

    }
   

  }
  last_mot=motionDetected;

  if (!alarm_activation){
    alarmActive=false;
    noTone(13);
  }

  if (alarmActive) {
    
    if (millis() - lastToneChange > toneChangeInterval) {
      lastToneChange = millis();

      tone(13, currentFreq);
      currentFreq += freqStep;
      if (currentFreq >= 2000 || currentFreq <= 1000) {
        freqStep = -freqStep; 
      }
      
    }

    if (millis() - alarmStartTime > alarmDuration){
      alarmActive = false;
      noTone(13);
    }
  }
  

  unsigned long currentMillis = millis();

  
  if ( currentMillis - lastDHTTime >= DHTInterval) {
    lastDHTTime = currentMillis;

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    if (!isnan(hum)) {
      char hum1[8];
      dtostrf(hum, 1, 2, hum1);
      client.publish("hum", hum1);
    }
    if (!isnan(temp)) {
      char temp1[8];
      dtostrf(temp, 1, 2, temp1);
      client.publish("temp", temp1);
    }
  }


}