# Protocolo TLS y certificados

## ¿Qué es el protocolo TLS?
 Transport Layer Security es un protocolo de seguridad que garantiza la privacidad de la comunicación entre dos dispositivos conectados a través de una red, como un navegador y un servidor web. Es la evolución de SSL (Secure Sockets Layer) y se encarga de cifrar los datos transmitidos, verificar la identidad del servidor (y opcionalmente del cliente), y asegurar la integridad de los mensajes.

 ## ¿Cuál es su importancia?
 TLS es importante porque:
 - Protege la confidencialidad: evita que terceros puedan leer los datos transmitidos.
 - Garantiza la integridad: asegura que los datos no sean modificados durante la transmisión.
 - Autentica las partes: confirma que el servidor (y en algunos casos el cliente) son quienes dicen ser.
  Sin TLS, la información (como contraseñas, datos bancarios o mensajes) puede ser interceptada o manipulada.

## ¿Qué es un certificado en este contexto?
Es un archivo digital emitido por una Autoridad de Certificación (CA) que contiene la clave pública del servidor, su identidad (dominio) y la firma digital de la CA. Permite que el ESP32 verifique que el servidor MQTT es legítimo antes de establecer una conexión TLS segura.

## ¿A qué riesgos se está expuesto si no se usa TLS?
Sin TLS, la conexión queda sin cifrado, lo que expone a:
- Ataques “Man in the Middle” (MitM): un atacante puede interceptar y modificar los datos.
- Robo de información sensible (credenciales, números de tarjeta, etc.).
- Suplantación de identidad (phishing o servidores falsos).
- Pérdida de integridad y confianza en el sistema o aplicación.

## ¿Qué es una CA (Certificate Authority)?
Una CA (Autoridad de Certificación) es una entidad confiable que emite certificados digitales. Actúa como un tercero de confianza, validando que quien solicita el certificado es realmente el propietario del dominio o identidad que declara

## ¿Qué es una cadena de certificados y cuál es la vigencia promedio de los eslabones?
La cadena de certificados conecta el certificado del servidor con una CA raíz de confianza.
Generalmente incluye:
- Certificado del servidor (end-entity).
- Certificados intermedios (emitidos por la CA intermedia).
- Certificado raíz (de la CA principal).
Vigencias típicas:
- Certificados de servidor: 90 días a 1 año
- Certificados intermedios: 2 a 5 años.
- Certificados raíz: 10 a 25 años.

## ¿Qué es un keystore y qué es un certificate bundle?.
- Keystore: archivo que almacena claves privadas y certificados asociados. Es usado por servidores o dispositivos (como el ESP32) para identificar y autenticar su identidad.
- Certificate bundle: conjunto de certificados empaquetados en un solo archivo, para permitir que un cliente valide toda la cadena de confianza.

## ¿Qué es la autenticación mutua en el contexto de TLS?
Es un mecanismo de seguridad en el que tanto el cliente como el servidor se autentican entre sí mediante certificados digitales válidos. Esto significa que el servidor presenta su certificado para demostrar su identidad al cliente, y el cliente también presenta el suyo para demostrar que está autorizado a comunicarse. Este proceso garantiza una comunicación más segura, ya que impide que dispositivos no autorizados establezcan conexión, y se utiliza especialmente en entornos donde se requiere alta confianza, como sistemas IoT o conexiones entre servicios críticos.

## ¿Cómo se habilita la validación de certificados en el ESP32?
En el ESP32, al usar librerías como ``` WiFiClientSecure ```, se habilita la validación cargando el certificado raíz del servidor.
Ejemplo en código: 
```
#include <WiFiClientSecure.h>

WiFiClientSecure client;

client.setCACert(root_ca);  // Cargar certificado raíz
if (!client.connect("example.com", 443)) {
  Serial.println("Conexión fallida");
}

```
Esto permite que el ESP32 verifique la autenticidad del servidor antes de enviar datos.






## Referencias
- Cloudflare (sf). ¿Qué es TLS (Transport Layer Security). https://www.cloudflare.com/es-es/learning/ssl/transport-layer-security-tls/
- Splashtop. (2025, 2 octubre). Cifrado TLS explicado: cómo funciona y por qué lo necesitas. https://www.splashtop.com/es/blog/tls-encryption
- CuriosidadesDeHackers. (2024, 29 agosto). TLS Downgrade(Ataque de degradación de TLS) ; Explicación y PoC. Curiosidades de Hackers. https://curiosidadesdehackers.com/tls-downgradeataque-de-degradacion-de-tls-explicacion-y-poc/#:~:text=Esta%20manipulaci%C3%B3n%20se%20aprovecha%20de,y%20sistemas%20expuestos%20al%20riesgo.
- digicert (2025, 2 jumio). ¿Qué es una CA?: Autoridades de Certificación para Principiantes. https://www.digicert.com/blog/what-is-a-certificate-authority
- Gaff, T. (2024, 24 julio). ¿Qué es la cadena de confianza de los certificados? Keyfactor. https://www.keyfactor.com/es/blog/certificate-chain-of-trust/
-  Cloudflare (sf). ¿Qué es TLS (Transport Layer Security). https://www.cloudflare.com/es-es/learning/ssl/transport-layer-security-tls/
- Llamas, L. (2025, 31 enero). Qué es y cómo usar autentificación en ESP32. Luis Llamas. https://www.luisllamas.es/autentificacion-en-esp32/
