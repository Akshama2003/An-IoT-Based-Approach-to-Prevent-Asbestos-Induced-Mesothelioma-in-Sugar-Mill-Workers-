#define BLYNK_TEMPLATE_ID "TMPL3_eM8FiC6"
#define BLYNK_TEMPLATE_NAME "Asbestos Safety Monitor"
#define BLYNK_AUTH_TOKEN "kFi0QbmOnU9Ke4Jpue9kU4Glcv3N_naa"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// ====== WiFi Credentials ======
char ssid[] = "panda";
char pass[] = "1234aksh";

// ====== Pin Config ======
#define MQ135_PIN 34
#define MQ5_PIN   35
#define DHTPIN    4
#define BUZZER    5
#define DHTTYPE   DHT11

DHT dht(DHTPIN, DHTTYPE);

// ====== Thresholds ======
int mq135_threshold = 400;
int mq5_threshold   = 300;
float temp_threshold = 38.0;
float hum_threshold  = 80.0;

BlynkTimer timer;
bool lastDanger = false;

// ====== Connect WiFi ======
void connectWiFi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, pass);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Failed! Running offline...");
  }
}

// ====== Connect Blynk ======
void connectBlynk() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connecting to Blynk...");
    Blynk.config(BLYNK_AUTH_TOKEN);

    if (Blynk.connect(5000)) {
      Serial.println("✅ Blynk Connected!");
    } else {
      Serial.println("❌ Blynk Failed! Will retry...");
    }
  }
}

// ====== Sensor Function ======
void sendSensorData() {
  int mq135_value = analogRead(MQ135_PIN);
  int mq5_value   = analogRead(MQ5_PIN);

  float temperature = dht.readTemperature();
  float humidity    = dht.readHumidity();

  // Handle DHT failure
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ DHT Read Failed!");
    temperature = 0;
    humidity = 0;
  }

  Serial.printf("MQ135=%d | MQ5=%d | Temp=%.2f | Hum=%.2f\n",
                mq135_value, mq5_value, temperature, humidity);

  // ====== Danger Logic ======
  bool danger = false;
  String status = "Safe";

  if (mq135_value > mq135_threshold) { danger = true; status = "Bad Air"; }
  if (mq5_value > mq5_threshold)     { danger = true; status = "Gas Leak"; }
  if (temperature > temp_threshold)  { danger = true; status = "High Temp"; }
  if (humidity > hum_threshold)      { danger = true; status = "High Humidity"; }

  // ====== Buzzer ======
  digitalWrite(BUZZER, danger ? LOW : HIGH);

  // ====== Send to Blynk ONLY if connected ======
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, mq135_value);
    Blynk.virtualWrite(V1, mq5_value);
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, humidity);
    Blynk.virtualWrite(V4, status);
    Blynk.virtualWrite(V5, danger ? 1 : 0);

    if (danger && !lastDanger) {
      Blynk.logEvent("danger_alert", status);
      Serial.println("📱 Notification sent: " + status);
    }
  }

  lastDanger = danger;
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n🚀 System Starting...");

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);

  dht.begin();

  connectWiFi();
  connectBlynk();

  timer.setInterval(2000L, sendSensorData);
}

// ====== Loop ======
void loop() {
  // Auto-reconnect WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("🔄 Reconnecting WiFi...");
    connectWiFi();
  }

  // Auto-reconnect Blynk
  if (!Blynk.connected()) {
    Serial.println("🔄 Reconnecting Blynk...");
    connectBlynk();
  }

  Blynk.run();
  timer.run();
}