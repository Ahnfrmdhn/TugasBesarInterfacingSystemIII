#define BLYNK_PRINT Serial
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <BlynkSimpleEsp8266.h>

//DHT
#define DHTTYPE DHT11
#define DHTPIN D4
DHT dht(DHTPIN, DHTTYPE);

//Firebase
#define FIREBASE_HOST "suhu-dan-kelembaban-dht11-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "caIuo8fmZvILTzcFsBjkaHyNy5A7pqvq6qIRABwt"

#define WIFI_SSID "Hotspot"
#define WIFI_PASSWORD "ahnaf12345"

FirebaseData firebaseData;

//Blynk auth
char auth[] = "xtr-uHD_OXIubdp7uI9ChHPgV_5j13uj"; //Enter your Blynk application auth token

//WiFi
char ssid[] = "Hotspot"; //Enter your WIFI name
char pass[] = "ahnaf12345"; //Enter your WIFI passowrd

BlynkTimer timer;
int pinValue = 0;

void sendSensor(){

  //variabel sensor DHT
  float h = dht.readHumidity();
  float s = dht.readTemperature();
  Blynk.virtualWrite(V1, s);
  Blynk.virtualWrite(V2, h);

  Serial.print("Suhu : ");
  Serial.print(s);
  Serial.println();
  Serial.print("Kelembaban : ");
  Serial.print(h);
  Serial.println();

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/suhu", s)){
    Serial.println("Suhu terkirim");
  }else{
    Serial.println("Suhu tidak terkirim");
    Serial.println("Karena : "+firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban", h)){
    Serial.println("Kelembaban terkirim");
  }else{
    Serial.println("Kelembaban tidak terkirim");
    Serial.println("Karena : "+firebaseData.errorReason());
  }

  //variabel sensor PIR
  bool sensor = digitalRead(D3);
  Serial.println(sensor);
  
  if (isnan(h) || isnan(s)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if(pinValue == 1){
    Serial.println("System is ON");

    //Lampu sensor PIR
    if (sensor == 1) {
      Blynk.notify("WARNING! Please check your security system");
      digitalWrite(D0, HIGH);
    } else if (sensor == 0) {
      digitalWrite(D0, LOW);
    }

   //Lampu sensor suhu
    if (s >= 35) {
      digitalWrite(D1, HIGH);
      digitalWrite(D2, LOW);
    }else if(s < 39){
      digitalWrite(D1, LOW);
      digitalWrite(D2, HIGH);
    }  
  } else if(pinValue == 0){
    Serial.println("System is OFF");
  } 
}

void setup() {
  Serial.begin(9600);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, INPUT); //input data sensor PIR
  dht.begin();

  //Firebase
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Blynk
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, sendSensor);
}

BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}

void loop() {
  Blynk.run();
  timer.run();
}
