# Control de Carro con HTTP, MQTT y Sensor Ultrasónico HC-SR04

Proyecto para controlar un carro con ESP32 mediante un servidor HTTP y publicación en MQTT, además de la simulación o lectura real del sensor ultrasónico HC-SR04 para medir distancias y enviarlas periódicamente a un tópico MQTT.

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
- Mensaje ejemplo: "Direction: forward, Speed: 200, Duration: 3000, Client IP: 192.168.1.100"

  ## Sensor Ultrasónico (HC-SR04)
  - El sistema cuenta con una función para leer la distancia del sensor HC-SR04:
    float leerDistancia();
    Esta función se ejecuta periódicamente (por ejemplo, cada 2 segundos) y publica el valor obtenido en el topic car/sensor.

  ## Variables de Preprocesador
  Las constantes del sistema se definen con #define en un archivo config.h, que se incluye en el código principal.
 ```cpp
// Pines de los motores
#define ENA 14
#define ENB 32
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

// Pines del sensor HC-SR04
#define TRIG_PIN 5
#define ECHO_PIN 18

// WiFi
#define WIFI_SSID "TuSSID"
#define WIFI_PASS "TuPassword"

// MQTT
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_TOPIC_CMD "car/instructions"
#define MQTT_TOPIC_SENSOR "car/sensor"

// Intervalo de lectura del sensor (ms)
#define SENSOR_READ_INTERVAL 2000
```




## Instalación
- Librerías: WebServer y PubSubClient.
- Cambia SSID y password en el código.

## Documentación
- **Colección Postman**: Disponible en [`/docs/docs/control.postman_collection.json`](./docs/control.postman_collection.json).
- **Codigo**: Disponible en [`/docs/Control.ino`](./docs/Control.ino).
- **Diagrma**: Disponible en [`/docs/image.svg`](./docs/image.svg).
- **Imagenes**: Disponible en [`/ima`](./ima).
## Colaboradores

- Profesor: [@fabianpaeri](https://github.com/fabianpaeri) (Docente).

- Colaborador 1: Maria Fernanda Rodriguez Chaparrro [@maferodriguezch06](https://github.com/maferodriguezch06) (Estudiante)

- Colaborador 2: Eduard Meza Salazar [@eduardmesa09](https://github.com/eduardmesa09) (Estudiante)

- Colaborador 3: Laura Valentina Rairan Gavilan [@LauraRairan](https://github.com/LauraRairan) (Estudiante)
 

