
# Control de Carro con HTTP, MQTT y Sensor Ultrasónico HC-SR04

Proyecto para controlar un carro con ESP32 mediante un servidor HTTP y publicación en MQTT, además de la lectura real del sensor ultrasónico HC-SR04 para medir distancias y enviarlas periódicamente a un tópico MQTT.

## Funcionalidad General
- Controlar los movimientos del carro (adelante, atrás, izquierda, derecha) desde peticiones HTTP.
- Publicar cada comando recibido en un topic MQTT.
- Leer los valores del sensor HC-SR04 periódicamente y publicarlos en un topic MQTT independiente.
- Centralizar la configuración de pines, red y MQTT mediante variables de preprocesador.

## Endpoints
- **/status** (GET): Verifica si el servidor está activo. Respuesta: "OK".
- **/control** (GET): Controla el movimiento.
  - Parámetros:
    - direction: forward, backward, left, right (string, requerido)
    - speed: 0-255 (int, requerido)
    - duration: 0-5000 ms (int, requerido)

## MQTT
- Broker: test.mosquitto.org:1883
- Topic: car/instructions
- Topic 2: car/telemetry/distance
- Mensaje ejemplo: ```cpp "Direction: forward, Speed: 200, Duration: 3000, Client IP: 192.168.1.100" ```
- Mensaje de ejemplo 2: ```[DIST] -> car/telemetry/distance: { "distance_cm": 112.64, "ts": 602903} (OK)```

  ## Sensor Ultrasónico (HC-SR04)
  - El sistema cuenta con una función para leer la distancia del sensor HC-SR04:
    float leerDistancia();
    Esta función se ejecuta periódicamente (por ejemplo, cada 2 segundos) y publica el valor obtenido en el topic car/sensor.

  ## Variables de Preprocesador
  Las constantes del sistema se definen con #define en el archivo geneal ControlSensor.ino, que se usan en el código principal.
 ```cpp
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
#define PIN_ECHO                     18
 
// Distancia válida (cm) y timeout del pulso
#define DIST_MIN_CM                  5.0f
#define DIST_MAX_CM                  300.0f
#define PULSE_TIMEOUT_US             30000UL  // 30ms
 
// Publicación periódica de distancia
#define PUBLISH_INTERVAL_MS          500UL
 
// /control: duración máxima permitida (ms)
#define MAX_MOVE_DURATION_MS         5000
```
## Instalación
- Librerías: WebServer y PubSubClient.

## Documentación
- **Colección Postman**: Disponible en [`/docs/docs/control.postman_collection.json`](./docs/control.postman_collection.json).
- **Codigo inicial**: Disponible en [`/docs/Control.ino`](./docs/Control.ino).
- **Codigo Con sensor**: Disponible en [`/docs/ControlSensor.ino`](./docs/ControlSensor.ino).
- **Diagrma**: Disponible en [`/docs/image.svg`](./docs/image.svg).
- **Imagenes**: Disponible en [`/ima`](./ima).
## Colaboradores

- Profesor: [@fabianpaeri](https://github.com/fabianpaeri) (Docente).

- Colaborador 1: Maria Fernanda Rodriguez Chaparrro [@maferodriguezch06](https://github.com/maferodriguezch06) (Estudiante)

- Colaborador 2: Eduard Meza Salazar [@eduardmesa09](https://github.com/eduardmesa09) (Estudiante)

- Colaborador 3: Laura Valentina Rairan Gavilan [@LauraRairan](https://github.com/LauraRairan) (Estudiante)
 
