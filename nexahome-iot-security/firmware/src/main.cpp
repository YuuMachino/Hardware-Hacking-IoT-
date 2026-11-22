#include <Arduino.h>
/*
 * NexaSense - firmware EDUCACIONAL (protótipo v0.3-lab)
 * Projeto: "Do Protótipo ao Produto Seguro" - Hardware Hacking IoT
 *
 * ATENÇÃO: este firmware contém fragilidades INTENCIONAIS para fins
 * didáticos (credenciais fictícias fixas, MQTT sem TLS, OTA sem assinatura,
 * console de depuração aberto). Todos os valores sensíveis são FICTÍCIOS.
 * Uso exclusivo em laboratório / simulador (Wokwi).
 */
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ---------- Configuração (protótipo: valores fixos no código) ----------
#define FW_VERSION      "0.3.1-lab"
#define DEVICE_ID       "nexasense-0001"          // mesmo ID em todas as unidades do lote
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASS       ""
#define MQTT_HOST       "broker.hivemq.com"       // broker público de teste (sem TLS)
#define MQTT_PORT       1883
#define MQTT_USER       "nexasense"
#define MQTT_PASS       "Nexa@2024-lab"           // FICTÍCIA
#define API_TOKEN       "nx_lab_token_FAKE_123456"// FICTÍCIO
#define OTA_URL         "http://updates.nexahome.example/nexasense/fw.bin" // HTTP, sem assinatura
#define TOPIC_DATA      "nexahome/lab/" DEVICE_ID "/telemetry"
#define TOPIC_CMD       "nexahome/lab/" DEVICE_ID "/cmd"

#define DHT_PIN   15
#define DHT_TYPE  DHT22
#define LED_PIN   2

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient netClient;                 // WiFiClient puro = sem TLS
PubSubClient mqtt(netClient);
unsigned long lastPublish = 0;

void debugBanner() {
  Serial.println();
  Serial.println("=== NexaSense boot ===");
  Serial.printf("[DBG] fw=%s device=%s\n", FW_VERSION, DEVICE_ID);
  Serial.printf("[DBG] chip=%s rev=%d cores=%d flash=%uKB\n",
                ESP.getChipModel(), ESP.getChipRevision(), ESP.getChipCores(),
                ESP.getFlashChipSize() / 1024);
  Serial.printf("[DBG] wifi ssid=%s pass='%s'\n", WIFI_SSID, WIFI_PASS);
  Serial.printf("[DBG] mqtt %s:%d user=%s pass=%s\n", MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS);
  Serial.printf("[DBG] api_token=%s\n", API_TOKEN);
  Serial.printf("[DBG] ota_url=%s\n", OTA_URL);
  Serial.println("[DBG] console de depuracao ativo: digite 'help'");
}

void handleConsole() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd == "help")        Serial.println("cmds: help | info | config | mac | ota | reboot");
  else if (cmd == "info")   Serial.printf("fw=%s heap=%u uptime=%lus\n", FW_VERSION, ESP.getFreeHeap(), millis()/1000);
  else if (cmd == "config") Serial.printf("wifi=%s/%s mqtt=%s:%d %s/%s token=%s\n",
                                          WIFI_SSID, WIFI_PASS, MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS, API_TOKEN);
  else if (cmd == "mac")    Serial.println(WiFi.macAddress());
  else if (cmd == "ota")    Serial.printf("[OTA] buscaria %s (sem verificacao de assinatura)\n", OTA_URL);
  else if (cmd == "reboot") ESP.restart();   // sem autenticacao
  else if (cmd.length())    Serial.println("comando desconhecido");
}

void onMqtt(char* topic, byte* payload, unsigned int len) {
  String msg;
  for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
  Serial.printf("[MQTT] cmd em %s: %s\n", topic, msg.c_str());
  // Comandos aceitos sem nenhuma autenticacao de origem / assinatura
  if (msg == "led_on")  digitalWrite(LED_PIN, HIGH);
  if (msg == "led_off") digitalWrite(LED_PIN, LOW);
  if (msg == "reboot")  ESP.restart();
  if (msg == "ota")     Serial.printf("[OTA] baixando %s e gravando (sem validar)\n", OTA_URL);
}

void connectWifi() {
  Serial.printf("[WiFi] conectando a %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print("."); }
  Serial.printf("\n[WiFi] ok ip=%s mac=%s\n", WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());
}

void connectMqtt() {
  while (!mqtt.connected()) {
    Serial.printf("[MQTT] conectando %s:%d ...\n", MQTT_HOST, MQTT_PORT);
    String clientId = String(DEVICE_ID) + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    if (mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("[MQTT] conectado (TCP/1883, sem TLS)");
      mqtt.subscribe(TOPIC_CMD);
    } else {
      Serial.printf("[MQTT] falhou rc=%d, tentando de novo\n", mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  dht.begin();
  debugBanner();
  connectWifi();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMqtt);
}

void loop() {
  handleConsole();
  if (!mqtt.connected()) connectMqtt();
  mqtt.loop();

  if (millis() - lastPublish > 5000) {
    lastPublish = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(t) || isnan(h)) { Serial.println("[SENSOR] falha na leitura"); return; }
    char buf[200];
    snprintf(buf, sizeof(buf),
             "{\"id\":\"%s\",\"fw\":\"%s\",\"mac\":\"%s\",\"temp\":%.1f,\"hum\":%.1f,\"token\":\"%s\"}",
             DEVICE_ID, FW_VERSION, WiFi.macAddress().c_str(), t, h, API_TOKEN);
    mqtt.publish(TOPIC_DATA, buf);
    Serial.printf("[PUB] %s -> %s\n", TOPIC_DATA, buf);
  }
}
