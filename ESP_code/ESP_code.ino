#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <DHT.h>
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
unsigned long alarm_duration = 10000;
unsigned long last_alarm = alarm_duration;



WiFiClient espClient;
PubSubClient client(espClient);


void IRAM_ATTR detectsMovement() {
  int state = digitalRead(pirPin);
  if (state == HIGH) {
    digitalWrite(2,HIGH);
   
    last_alarm=millis();
  } else {
    digitalWrite(2,LOW);
    
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
  
  attachInterrupt(digitalPinToInterrupt(23), detectsMovement, CHANGE);



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



   unsigned long currentMillis = millis();

   if (currentMillis-last_alarm<=alarm_duration){
      
   }
   else{

   }

  if (currentMillis - lastDHTTime >= DHTInterval) {
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