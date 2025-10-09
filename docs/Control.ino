#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
 

const char* ssid = "motoedge50fusion_5477";
const char* password = "Laura123";
 
// Broker MQTT (broker público de prueba; reemplaza si es necesario)
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "car/instructions";
 
// Pines de los motores
const int Motor1A = 19;  // IN1 motor 1
const int Motor1B = 21;  // IN2 motor 1
const int Motor2A = 26;  // IN1 motor 2
const int Motor2B = 25;  // IN2 motor 2
 
// Variables para control de movimiento
bool moving = false;
unsigned long endTime = 0;
 
// Clientes WiFi y MQTT
WiFiClient espClient;
PubSubClient mqttClient(espClient);
 
// Servidor HTTP en puerto 80
WebServer server(80);
 
void stopMotors() {
  digitalWrite(Motor1A, LOW);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2A, LOW);
  digitalWrite(Motor2B, LOW);
}
 
void moveForward() {
  digitalWrite(Motor1A, HIGH); digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2A, HIGH); digitalWrite(Motor2B, LOW);
}
 
void moveBackward() {
  digitalWrite(Motor1A, LOW); digitalWrite(Motor1B, HIGH);
  digitalWrite(Motor2A, LOW); digitalWrite(Motor2B, HIGH);
}
 
void turnLeft() {
  digitalWrite(Motor1A, HIGH); digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2A, LOW); digitalWrite(Motor2B, HIGH);
}
 
void turnRight() {
  digitalWrite(Motor1A, LOW); digitalWrite(Motor1B, HIGH);
  digitalWrite(Motor2A, HIGH); digitalWrite(Motor2B, LOW);
}
 
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando a MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" reintentando en 5 segundos");
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
 
  // Configurar pines de motores
  pinMode(Motor1A, OUTPUT);
  pinMode(Motor1B, OUTPUT);
  pinMode(Motor2A, OUTPUT);
  pinMode(Motor2B, OUTPUT);
  stopMotors();
 
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
 
  // Configurar MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
 
  // Endpoint de healthcheck
  server.on("/status", []() {
    server.send(200, "text/plain", "OK");
  });
 
  // Endpoint de control
  server.on("/control", []() {
    if (!server.hasArg("direction") || !server.hasArg("duration")) {
      server.send(400, "text/plain", "Faltan parámetros");
      return;
    }
 
    String direction = server.arg("direction");
    int duration = server.arg("duration").toInt();
 
    // Validar duración (máximo 5000 ms)
    if (duration < 0) duration = 0;
    if (duration > 5000) duration = 5000;
 
    // Detener movimiento actual
    stopMotors();
    moving = false;
 
    // Ejecutar movimiento si duration > 0
    if (duration > 0) {
      if (direction == "forward") {
        moveForward();
      } else if (direction == "backward") {
        moveBackward();
      } else if (direction == "left") {
        turnLeft();
      } else if (direction == "right") {
        turnRight();
      } else {
        server.send(400, "text/plain", "Dirección inválida");
        return;
      }
 
      // Configurar temporizador para detener
      moving = true;
      endTime = millis() + duration;
    }
 
    // Obtener IP del cliente
    String clientIP = server.client().remoteIP().toString();
 
    // Publicar en MQTT
    String message = "Dirección: " + direction + ", Duración: " + String(duration) + ", Cliente IP: " + clientIP;
    if (mqttClient.connected()) {
      mqttClient.publish(mqtt_topic, message.c_str());
    }
 
    // Enviar respuesta
    server.send(200, "text/plain", "OK");
  });
 
  // Iniciar servidor
  server.begin();
}
 
void loop() {
  // Mantener conexión MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
 
  // Detener movimiento si el temporizador expira
  if (moving && millis() >= endTime) {
    stopMotors();
    moving = false;
  }
 
  // Manejar peticiones HTTP
  server.handleClient();
}
