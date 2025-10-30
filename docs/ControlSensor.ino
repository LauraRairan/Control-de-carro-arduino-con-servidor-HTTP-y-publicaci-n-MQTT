#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

/************* CONFIGURACIÓN (EDITA AQUÍ) *************/
// Modo: 1 = simulado, 0 = sensor físico HC-SR04
#define USE_SIMULATED                0   

// Wi-Fi
#define WIFI_SSID                    "motoedge50fusion_5477"
#define WIFI_PASS                    "Laura123"

// MQTT
#define MQTT_SERVER                  "test.mosquitto.org"
#define MQTT_PORT                    1883
#define MQTT_TOPIC_CMD               "car/instructions"
#define MQTT_TOPIC_DISTANCE          "car/telemetry/distance"

// HTTP
#define HTTP_PORT                    80

// Motores (tu cableado original)
#define PIN_MOTOR1A                  19
#define PIN_MOTOR1B                  21
#define PIN_MOTOR2A                  26
#define PIN_MOTOR2B                  25

// Sensor HC-SR04
#define PIN_TRIG                     5
#define PIN_ECHO                     18   // ⚠ usar divisor a 3.3V

// Distancia válida (cm) y timeout del pulso
#define DIST_MIN_CM                  5.0f
#define DIST_MAX_CM                  300.0f
#define PULSE_TIMEOUT_US             30000UL  // 30ms

// Publicación periódica de distancia
#define PUBLISH_INTERVAL_MS          500UL

// /control: duración máxima permitida (ms)
#define MAX_MOVE_DURATION_MS         5000

/************* GLOBALES *************/
bool moving = false;
unsigned long endTime = 0;

unsigned long lastDistancePublish = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WebServer server(HTTP_PORT);

/************* PROTOTIPOS *************/
void stopMotors();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();

void connectWiFi();
void ensureMQTT();
void handleDistancePublish();

float readDistanceCm();         // sin argumentos, retorna float
static float simDistance();     // helper simulado
static float hcDistance();      // helper físico

/************* SETUP *************/
void setup() {
  Serial.begin(115200);
  delay(200);

  // Motores
  pinMode(PIN_MOTOR1A, OUTPUT);
  pinMode(PIN_MOTOR1B, OUTPUT);
  pinMode(PIN_MOTOR2A, OUTPUT);
  pinMode(PIN_MOTOR2B, OUTPUT);
  stopMotors();

  // Sensor
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // Wi-Fi
  connectWiFi();

  // MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback([](char* topic, byte* payload, unsigned int len){
    Serial.print("MQTT ["); Serial.print(topic); Serial.print("] ");
    for (unsigned int i=0; i<len; ++i) Serial.print((char)payload[i]);
    Serial.println();
  });

  // HTTP endpoints
  server.on("/status", [](){
    server.send(200, "text/plain", "OK");
  });

  // /control?direction=forward|backward|left|right&duration=0..5000
  server.on("/control", [](){
    if (!server.hasArg("direction") || !server.hasArg("duration")) {
      server.send(400, "text/plain", "Faltan parámetros");
      return;
    }

    String direction = server.arg("direction");
    long duration = server.arg("duration").toInt();

    if (duration < 0) duration = 0;
    if (duration > (long)MAX_MOVE_DURATION_MS) duration = MAX_MOVE_DURATION_MS;

    stopMotors();
    moving = false;

    if (duration > 0) {
      if      (direction == "forward")  moveForward();
      else if (direction == "backward") moveBackward();
      else if (direction == "left")     turnLeft();
      else if (direction == "right")    turnRight();
      else {
        server.send(400, "text/plain", "Dirección inválida");
        return;
      }
      moving = true;
      endTime = millis() + duration;
    }

    // Publica en topic de instrucciones
    String clientIP = server.client().remoteIP().toString();
    String msg = "Direccion: " + direction +
                 ", Duracion(ms): " + String(duration) +
                 ", ClienteIP: " + clientIP;
    if (mqttClient.connected()) {
      mqttClient.publish(MQTT_TOPIC_CMD, msg.c_str());
    }

    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Setup listo ✅");
}

/************* LOOP *************/
void loop() {
  ensureMQTT();
  mqttClient.loop();

  server.handleClient();

  // Auto-stop por tiempo
  if (moving && millis() >= endTime) {
    stopMotors();
    moving = false;
  }

  // Telemetría distancia
  handleDistancePublish();
}

/************* IMPLEMENTACIÓN *************/
// Motores
void stopMotors() {
  digitalWrite(PIN_MOTOR1A, LOW);
  digitalWrite(PIN_MOTOR1B, LOW);
  digitalWrite(PIN_MOTOR2A, LOW);
  digitalWrite(PIN_MOTOR2B, LOW);
}
void moveForward() {
  digitalWrite(PIN_MOTOR1A, HIGH);
  digitalWrite(PIN_MOTOR1B, LOW);
  digitalWrite(PIN_MOTOR2A, HIGH);
  digitalWrite(PIN_MOTOR2B, LOW);
}
void moveBackward() {
  digitalWrite(PIN_MOTOR1A, LOW);
  digitalWrite(PIN_MOTOR1B, HIGH);
  digitalWrite(PIN_MOTOR2A, LOW);
  digitalWrite(PIN_MOTOR2B, HIGH);
}
void turnLeft() {
  digitalWrite(PIN_MOTOR1A, HIGH);
  digitalWrite(PIN_MOTOR1B, LOW);
  digitalWrite(PIN_MOTOR2A, LOW);
  digitalWrite(PIN_MOTOR2B, HIGH);
}
void turnRight() {
  digitalWrite(PIN_MOTOR1A, LOW);
  digitalWrite(PIN_MOTOR1B, HIGH);
  digitalWrite(PIN_MOTOR2A, HIGH);
  digitalWrite(PIN_MOTOR2B, LOW);
}

// Wi-Fi
void connectWiFi() {
  Serial.printf("Conectando a WiFi: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(800);
    Serial.print(".");
  }
  Serial.printf("\nWiFi OK. IP: %s\n", WiFi.localIP().toString().c_str());
}

// MQTT
void ensureMQTT() {
  if (mqttClient.connected()) return;

  Serial.print("Conectando a MQTT...");
  while (!mqttClient.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("conectado ✅");
    } else {
      Serial.print(" fallo rc=");
      Serial.print(mqttClient.state());
      Serial.println(" reintento en 3s");
      delay(3000);
    }
  }
}

// Publicación periódica de distancia (a tópico distinto)
void handleDistancePublish() {
  unsigned long now = millis();
  if (now - lastDistancePublish < PUBLISH_INTERVAL_MS) return;
  lastDistancePublish = now;

  float cm = readDistanceCm();

  // JSON simple
  String payload = "{";
  payload += "\"distance_cm\":"; payload += String(cm, 2);
  payload += ",\"ts\":";         payload += String(now);
  payload += "}";

  bool ok = false;
  if (mqttClient.connected()) ok = mqttClient.publish(MQTT_TOPIC_DISTANCE, payload.c_str());

  Serial.printf("[DIST] -> %s : %s (%s)\n",
                MQTT_TOPIC_DISTANCE, payload.c_str(), ok ? "OK" : "FAIL");
}

// LECTURA DISTANCIA (API pedida)
float readDistanceCm() {
#if USE_SIMULATED
  return simDistance();
#else
  return hcDistance();
#endif
}

// Simulado
static float simDistance() {
  static float base = 80.0f; // cm
  float t = millis() / 1000.0f;
  float wave  = 40.0f * sinf(t * 0.7f);
  float noise = random(-300, 301) / 100.0f; // ±3.00 cm
  float v = base + wave + noise;
  if (v < DIST_MIN_CM) v = DIST_MIN_CM;
  if (v > DIST_MAX_CM) v = DIST_MAX_CM;
  return v;
}

// Físico HC-SR04
static float hcDistance() {
  // Pulso TRIG 10us
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Alto en ECHO con timeout
  unsigned long dur = pulseIn(PIN_ECHO, HIGH, PULSE_TIMEOUT_US);
  if (dur == 0) return DIST_MAX_CM; // timeout → muy lejos o error

  // cm = (dur_us/2) * 0.0343
  float cm = (dur * 0.0343f) / 2.0f;
  if (cm < DIST_MIN_CM) cm = DIST_MIN_CM;
  if (cm > DIST_MAX_CM) cm = DIST_MAX_CM;
  return cm;
}
