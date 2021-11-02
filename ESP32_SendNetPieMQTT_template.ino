#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "Wifi name";   //ชื่อ Wifi
const char* password = "password wifi";  //Password Wifi
const char* mqttServer = "broker.netpie.io";  
const int mqttPort = 1883;
const char* mqtt_Client = "client-ID"; //Client ID ได้จาก NetPie
const char* mqtt_username = "Token NETPIE";   //Token ได้จาก NetPie
const char* mqtt_password = "Secret NETPIE";   //Secret ได้จาก NetPie
  
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;
long temp;
long humidity;
char msg[50];

void reconnect(){
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection…");
      if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
        Serial.println("connected");
      }
      else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println("try again in 5 seconds");
        delay(5000);
      }
    }
}

/*void resetESP(void *pvParameter){
  while(1){
    vTaskDelay(180000 / portTICK_RATE_MS);
    ESP.restart();
  }
}
*/
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttServer, mqttPort);
  //xTaskCreate(&resetESP, "taskResetESP", 1024, NULL, 1, NULL);
  
}

void loop() {
if (!client.connected()) {
  reconnect();
}
client.loop();
// Message send 
long now = millis();
if (now - lastMsg > 10000) {
  lastMsg = now;
  ++value;
  float humid = random(7000,9000) / 100.0;
  float temp = random(1500,4300) / 100.0;
  String data = String(humid) + ":" + String(temp);
  Serial.println(data);
  data.toCharArray(msg, (data.length() + 1));   //นำตัวแปร Data มาแปลงเป็น CharArray คำสั่ง [ .toCharArray(ตัวแปรที่จะนำไปเก็บ, จำนวนของตัวอักษรที่นำมาใส่ทั้งหมด) ]
  client.publish("@msg/test",msg);    // @msg/test เป็น Topic ในการส่งข้อมูลไปยัง NetPie และมีตัวแปร msg เป็น message ในที่นี้ Message ก็คือ ค่าของ humidity กับ temp
}

// Shadow send for to use dashboard
  /*float humid = random(7000,9000) / 100.0;
  float temp = random(1500,4300) / 100.0;
  String data = "{\"data\": {\"humidity\":" + String(humid) + ", \"temperature\":" + String(temp) + "}}";
  Serial.println(data);
  data.toCharArray(msg, (data.length() + 1));
  client.publish("@shadow/data/update", msg);
  delay(30000);*/
}
