# Control de Carro con HTTP y MQTT

Proyecto para controlar un carro con ESP32 usando un servidor HTTP y publicación en MQTT.

## Endpoints
- **/status** (GET): Verifica si el servidor está activo. Respuesta: "OK".
- **/control** (GET): Controla el movimiento.
  - Parámetros:
    - direction: forward, backward, left, right (string, requerido)
    - speed: 0-255 (int, requerido)
    - duration: 0-5000 ms (int, requerido)
  - Ejemplo: GET /control?direction=forward&speed=200&duration=3000
  - Al recibir, publica en MQTT y mueve el carro por la duración especificada.

## MQTT
- Broker: test.mosquitto.org:1883
- Topic: car/instructions
- Mensaje ejemplo: "Direction: forward, Speed: 200, Duration: 3000, Client IP: 192.168.1.100"

## Instalación
- Librerías: ESPAsyncWebServer.
- Cambia SSID y password en el código.

## Documentación
- **Colección Postman**: Disponible en [`/docs/docs/control.postman_collection.json`](./docs/control.postman_collection.json).
-  **Diagrma**: Disponible en [`/docs/image.svg`](./docs/image.svg).
-  **Imagenes**: Disponible en [`/ima`](./ima).


