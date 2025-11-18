#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Certificado raíz del broker Hive MQ, para TLS/SSL (puerto 8883)
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//Variables del preprocesador
// Modo de sensor (0: físico HC-SR04, 1: simulado)
#define USE_SIMULATED                0

// ----- Wi-Fi -----
#define WIFI_SSID                    "FreeWIFI_09"
#define WIFI_PASS                    "eduard09"

// ----- MQTT -----
#define MQTT_SERVER                  "e772b5c02ba747c792bf576e640bab45.s1.eu.hivemq.cloud"
#define MQTT_PORT                    8883
#define MQTT_USER                    "eduardmeza09" 
#define MQTT_PASS                    "123456789eE" 
#define MQTT_TOPIC_CMD               "car/instructions"
#define MQTT_TOPIC_DISTANCE          "car/telemetry/distance"
#define MQTT_TOPIC_EVENT             "car/events"
#define MQTT_TOPIC_INFO              "car/info/ip"

// ----- HTTP -----
#define HTTP_PORT                    80

// ----- Motores -----
#define PIN_MOTOR1A                  19
#define PIN_MOTOR1B                  21
#define PIN_MOTOR2A                  26
#define PIN_MOTOR2B                  25

// ----- Sensor HC-SR04 -----
#define PIN_TRIG                     5
#define PIN_ECHO                     18       //Se usa divisor de voltaje 5V->3.3V

#define DIST_MIN_CM                  5.0f
#define DIST_MAX_CM                  300.0f
#define SAFE_CM                      20.0f    // Menor a esto: freno automático
#define PULSE_TIMEOUT_US             30000UL  // 30 ms

#define PUBLISH_INTERVAL_MS          500UL
#define MAX_MOVE_DURATION_MS         5000

/************* Estado global *************/
bool moving = false;
unsigned long endTime = 0;
unsigned long lastDistancePublish = 0;

WiFiClientSecure espClientSecure;
PubSubClient mqttClient(espClientSecure);
WebServer server(HTTP_PORT);

/************* Prototipos *************/
void stopMotors();
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();

void connectWiFi();
void ensureMQTT();
void publishRobotIp();
void handleDistancePublish();     // Publica distancia periódicamente por MQTT

float readDistanceCm();           // Llama a simulado o HC-SR04 físico
static float simDistance();       // Sensor simulado
static float hcDistance();        // Sensor físico

/************* SETUP *************/
void setup() {
  Serial.begin(115200);
  delay(200);

  // Inicializa pines de motores como salida
  pinMode(PIN_MOTOR1A, OUTPUT);
  pinMode(PIN_MOTOR1B, OUTPUT);
  pinMode(PIN_MOTOR2A, OUTPUT);
  pinMode(PIN_MOTOR2B, OUTPUT);
  stopMotors();

  // Inicializa pines del sensor ultrasónico
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // Conecta a WiFi, configura el certificado TLS.
  connectWiFi();

  // Configura MQTT seguro
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback([](char* topic, byte* payload, unsigned int len){
    Serial.print("MQTT ["); Serial.print(topic); Serial.print("] ");
    for (unsigned int i=0; i<len; ++i) Serial.print((char)payload[i]);
    Serial.println();
  });

  // Endpoints REST con CORS
  // Healthcheck REST
  server.on("/api/v1/healthcheck", HTTP_GET, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // Preflight CORS para /api/v1/move
  server.on("/api/v1/move", HTTP_OPTIONS, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);
  });

  // Control via REST GET (compatibilidad: usa query params direction/duration)
  server.on("/api/v1/move", HTTP_GET, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    if (!server.hasArg("direction") || !server.hasArg("duration")) {
      server.send(400, "application/json", "{\"error\":\"missing params\"}");
      return;
    }

    String direction = server.arg("direction");
    long duration = server.arg("duration").toInt();
    
    if (duration < 0) duration = 0;
    if (duration > (long)MAX_MOVE_DURATION_MS) duration = MAX_MOVE_DURATION_MS;

    // detiene el motor al inicio
    stopMotors(); moving = false;
    
    if (duration > 0) {
      if      (direction == "adelante")  {moveForward(); moving = true;}
      else if (direction == "atras") {moveBackward(); moving = true;}
      else if (direction == "izquierda")     {turnLeft(); moving = true;}
      else if (direction == "derecha")    {turnRight(); moving = true;}
      else if (direction == "parar")    {stopMotors(); moving = false;}
      else {
        server.send(400, "application/json", "{\"error\":\"invalid direction\"}");
        return;
      }
      endTime = millis() + duration;
    }

    // Publica en MQTT
    String clientIP = server.client().remoteIP().toString();
    String msg = "{\"direction\":\"" + direction +
                 "\",\"duration\":" + String(duration) +
                 ",\"clientIP\":\"" + clientIP + "\"}";
    if (mqttClient.connected()) {
      mqttClient.publish(MQTT_TOPIC_CMD, msg.c_str());
    }
    server.send(200, "application/json", "{\"ok\":true}");
  });

  // Control via POST: body JSON {"direction":"adelante","duration":1200}
  server.on("/api/v1/move", HTTP_POST, [](){
    server.sendHeader("Access-Control-Allow-Origin", "*");
    if (!server.hasArg("plain")) {
      server.send(400, "application/json", "{\"error\":\"missing body\"}");
      return;
    }
    // Parse básico sin ArduinoJson
    String body = server.arg("plain");
    String direction;
    long duration = 0;
    int dirIdx = body.indexOf("\"direction\"");
    int durIdx = body.indexOf("\"duration\"");
    if (dirIdx >= 0) {
      int firstQuote = body.indexOf("\"", dirIdx+11);
      int secondQuote = body.indexOf("\"", firstQuote+1);
      direction = body.substring(firstQuote+1, secondQuote);
    }
    if (durIdx >= 0) {
      int colon = body.indexOf(":", durIdx);
      int comma = body.indexOf(",", colon);
      int endBr = body.indexOf("}", colon);
      String durStr = body.substring(colon+1, comma>0?comma:endBr);
      duration = durStr.toInt();
    }

    if (duration < 0) duration = 0;
    if (duration > (long)MAX_MOVE_DURATION_MS) duration = MAX_MOVE_DURATION_MS;
    // Seguridad anti-choque por REST
    float cm = readDistanceCm();
    if (direction == "adelante" && cm < SAFE_CM) {
      stopMotors();
      server.send(409, "application/json", "{\"error\":\"OBSTACLE\"}");
      return;
    }

    //Se detiene al iniciar
    stopMotors(); 
    moving = false;

    if (duration > 0) {
      if      (direction == "adelante")  moveForward();
      else if (direction == "atras") moveBackward();
      else if (direction == "izquierda")     turnLeft();
      else if (direction == "derecha")    turnRight();
      else {
        server.send(400, "application/json", "{\"error\":\"invalid direction\"}");
        return;
      }
      moving = true;
      endTime = millis() + duration;
    }
    String clientIP = server.client().remoteIP().toString();
    String msg = "{\"direction\":\"" + direction +
                 "\",\"duration\":" + String(duration) +
                 ",\"clientIP\":\"" + clientIP + "\"}";
    if (mqttClient.connected()) {
      mqttClient.publish(MQTT_TOPIC_CMD, msg.c_str());
    }
    server.send(200, "application/json", "{\"ok\":true}");
  });

  server.begin();
  Serial.println("Setup listo.");
}

/************* LOOP *************/
void loop() {
  ensureMQTT();          // Mantiene conexión MQTT
  mqttClient.loop();     // Procesa mensajes MQTT

  server.handleClient(); // Procesa peticiones HTTP REST

  // Auto-stop por tiempo detrás de movimiento
  if (moving && (millis() >= endTime)) {
    stopMotors();
    moving = false;
  }

  // Publica periódicamente distancia en MQTT
  handleDistancePublish();
}

/************* Implementación *************/

// Motores: cada función ejecuta el movimiento indicado
void stopMotors() {
  digitalWrite(PIN_MOTOR1A, LOW);
  digitalWrite(PIN_MOTOR1B, LOW);
  digitalWrite(PIN_MOTOR2A, LOW);
  digitalWrite(PIN_MOTOR2B, LOW);
}
void moveBackward() {
  digitalWrite(PIN_MOTOR1A, HIGH);
  digitalWrite(PIN_MOTOR1B, LOW);
  digitalWrite(PIN_MOTOR2A, HIGH);
  digitalWrite(PIN_MOTOR2B, LOW);
}
void moveForward() {
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

//Publica la ip del robot por mqtt
void publishRobotIp() {
  String ip = WiFi.localIP().toString();
  String msg = "{\"ip\":\"" + ip + "\"}";
  if (mqttClient.connected()) {
    mqttClient.publish(MQTT_TOPIC_INFO, msg.c_str(), true);
    Serial.printf("IP publicada por MQTT: %s\n", msg.c_str());
  }
}

// Wi-Fi: conecta, configura modo STA y CA para TLS
void connectWiFi() {
  Serial.printf("Conectando a WiFi: %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(800);
    Serial.print(".");
  }
  Serial.printf("\nWiFi OK. IP: %s\n", WiFi.localIP().toString().c_str());
  // Verifica el certificado
  espClientSecure.setCACert(root_ca);
}

// Asegura conexión TLS a MQTT y suscribe al tópico de comandos
void ensureMQTT() {
  if (mqttClient.connected()) return;
  Serial.print("Conectando a MQTT...");
  while (!mqttClient.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    // Sin usuario/clave para el broker público
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      mqttClient.subscribe(MQTT_TOPIC_CMD);
      Serial.println("conectado al servidor MQTT de forma segura.");
      // ¡AQUÍ SÍ ESTÁ CONECTADO! → PUBLICAR IP AHORA
      publishRobotIp();

    } else {
      Serial.print(" fallo rc=");
      Serial.print(mqttClient.state());
      Serial.println(" reintento en 3s");
      delay(3000);
    }
  }
}

// Publica la lectura de distancia cada cierto tiempo en MQTT y asegura el sistema antichoques
void handleDistancePublish() {
  unsigned long now = millis();
  if (now - lastDistancePublish < PUBLISH_INTERVAL_MS) return;
  lastDistancePublish = now;

  float cm = readDistanceCm();

  //AntiChoques
  if (cm <= SAFE_CM){
    stopMotors();
    moving = false;
  }

  // JSON simple con distancia y timestamp
  String payload = "{";
  payload += "\"distance_cm\":"; payload += String(cm, 2);
  payload += ",\"ts\":";         payload += String(now);
  payload += "}";

  bool ok = false;
  if (mqttClient.connected()) ok = mqttClient.publish(MQTT_TOPIC_DISTANCE, payload.c_str());
  Serial.printf("[DIST] -> %s : %s (%s)\n", MQTT_TOPIC_DISTANCE, payload.c_str(), ok ? "OK" : "FAIL");
}

// Lectura del sensor (simulado o HC-SR04 real)
float readDistanceCm() {
#if USE_SIMULATED
  return simDistance();
#else
  return hcDistance();
#endif
}

// Sensor simulado: onda + ruido dentro de rango
static float simDistance() {
  static float base = 80.0f;
  float t = millis() / 1000.0f;
  float wave  = 40.0f * sinf(t * 0.7f);
  float noise = random(-300, 301) / 100.0f;
  float v = base + wave + noise;
  if (v < DIST_MIN_CM) v = DIST_MIN_CM;
  if (v > DIST_MAX_CM) v = DIST_MAX_CM;
  return v;
}

// Sensor físico HC-SR04
static float hcDistance() {
  // Pulso TRIG 10us
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Lee pulso ECHO
  unsigned long dur = pulseIn(PIN_ECHO, HIGH, PULSE_TIMEOUT_US);
  if (dur == 0) return DIST_MAX_CM; // timeout: muy lejos o error

  // Convertir a cm
  float cm = (dur * 0.0343f) / 2.0f;
  if (cm < DIST_MIN_CM) cm = DIST_MIN_CM;
  if (cm > DIST_MAX_CM) cm = DIST_MAX_CM;
  return cm;
}
