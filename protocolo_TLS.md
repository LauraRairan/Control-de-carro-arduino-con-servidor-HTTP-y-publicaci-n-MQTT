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

## Si el sketch necesita conectarse a múltiples dominios con certificados generados por CAs distintas, ¿qué alternativas hay?
Si un sketch de ESP32 necesita conectarse a múltiples dominios con certificados emitidos por diferentes Autoridades de Certificación (CA), existen varias alternativas para manejar la validación TLS: se puede incluir múltiples certificados raíz directamente en el firmware del dispositivo, crear un bundle de certificados que contenga todas las CA confiables.

## ¿Cómo se puede obtener el certificado para un dominio?
Se puede obtener de dos formas:
- Gratuita: usando servicios como Let’s Encrypt.
- Pagada: comprando un certificado a una CA comercial (ej. DigiCert, GoDaddy).
El proceso implica generar una clave privada, una CSR y validar la propiedad del dominio.

## ¿A qué se hace referencia cuando se habla de llave pública y privada en el contexto de TLS?
- Llave privada: se mantiene secreta y sirve para firmar o descifrar datos.
- Llave pública: se comparte libremente y se usa para verificar firmas o cifrar datos que solo la llave privada podrá descifrar.
Ambas forman un par criptográfico y son la base de la seguridad de TLS.

## ¿Qué pasará con el código cuando los certificados expiren?
Cuando un certificado digital expira, deja de ser considerado confiable por los clientes o dispositivos que intentan establecer una conexión segura, ya que su validez temporal ha terminado. En el caso del ESP32, esto significa que las conexiones TLS —por ejemplo, hacia un servidor MQTT— fallarán automáticamente, ya que el dispositivo rechazará el certificado vencido para evitar riesgos de seguridad. En consecuencia, el código que dependía de esa conexión dejará de funcionar correctamente hasta que se renueve el certificado y se actualice en el firmware o en el servidor.

## ¿Qué teoría matemática es el fundamento de la criptografía moderna?
La criptografía moderna se basa principalmente en:
- Teoría de números: En criptografía, se usa especialmente para operaciones de modularidad, factorización y exponenciación, que son la base de algoritmos como RSA o Diffie-Hellman.
- Álgebra abstracta: Analiza estructuras matemáticas como grupos, anillos y campos, que permiten definir operaciones seguras y predecibles sobre los datos cifrados.
- Funciones de una sola vía: Son funciones matemáticas que son fáciles de calcular en una dirección, pero muy difíciles de invertir sin información adicional. Se utilizan en la construcción de hashes criptográficos, firmas digitales y protocolos de autenticación.
- Teoría de la información: Es una base esencial porque define conceptos como entropía, ruido y confusión/difusión, que permiten medir la incertidumbre y seguridad de los sistemas criptográficos.
- algoritmos de cifrado: se apoyan en distintas ramas matemáticas como la teoría de números, el álgebra modular y la aritmética de curvas elípticas. Por tanto, al mencionarlos junto con su base matemática, estás reforzando la idea de que la criptografía moderna combina teoría matemática pura con modelos teóricos de información y computación.

  ## ¿Cuáles son las posibles implicaciones de la computación cuántica para los métodos de criptografía actuales?
  La computación cuántica representa un cambio profundo en la forma de procesar información, ya que utiliza qubits en lugar de bits tradicionales. Los qubits pueden existir en múltiples estados simultáneamente gracias a principios de la superposición y el entrelazamiento cuántico, lo que les permite resolver ciertos problemas matemáticos de manera exponencialmente más rápida que los computadores clásicos. En el ámbito de la criptografía moderna, esta capacidad tiene implicaciones significativas, ya que muchos sistemas de seguridad actuales —como RSA, Diffie-Hellman y ECC (Elliptic Curve Cryptography) dependen de problemas matemáticos considerados difíciles, como la factorización de números grandes o el logaritmo discreto.

Con un computador cuá

## Referencias
- Cloudflare (sf). ¿Qué es TLS (Transport Layer Security). https://www.cloudflare.com/es-es/learning/ssl/transport-layer-security-tls/
- Splashtop. (2025, 2 octubre). Cifrado TLS explicado: cómo funciona y por qué lo necesitas. https://www.splashtop.com/es/blog/tls-encryption
- CuriosidadesDeHackers. (2024, 29 agosto). TLS Downgrade(Ataque de degradación de TLS) ; Explicación y PoC. Curiosidades de Hackers. https://curiosidadesdehackers.com/tls-downgradeataque-de-degradacion-de-tls-explicacion-y-poc/#:~:text=Esta%20manipulaci%C3%B3n%20se%20aprovecha%20de,y%20sistemas%20expuestos%20al%20riesgo.
- digicert (2025, 2 jumio). ¿Qué es una CA?: Autoridades de Certificación para Principiantes. https://www.digicert.com/blog/what-is-a-certificate-authority
- Gaff, T. (2024, 24 julio). ¿Qué es la cadena de confianza de los certificados? Keyfactor. https://www.keyfactor.com/es/blog/certificate-chain-of-trust/
-  Cloudflare (sf). ¿Qué es la autenticación mutua? | Autenticación en dos fases. [https://www.cloudflare.com/es-es/learning/ssl/transport-layer-security-tls/](https://www.cloudflare.com/es-es/learning/access-management/what-is-mutual-authentication/)
- Llamas, L. (2025, 31 enero). Qué es y cómo usar autentificación en ESP32. Luis Llamas. https://www.luisllamas.es/autentificacion-en-esp32/
- digicert (sf). ¿Qué es un Certificado Multidominio (SAN)?. https://www.cloudflare.com/es-es/learning/ssl/transport-layer-security-tls/](https://www.digicert.com/es/faq/public-trust-and-certificates/what-is-a-multi-domain-san-certificate)
- Lorena. (2024, 1 octubre). Certificados SSL gratuitos Let's Encrypt en Webempresa - Preguntas Frecuentes webempresa.com. Preguntas Frecuentes webempresa.com. https://guias.webempresa.com/preguntas-frecuentes/dominio-webempresa-instalar-certificado-ssl-lets-encrypt/
- SSL Corp. (2023, 15 junio). Claves privadas y públicas - SSL.com. SSL.com. https://www.ssl.com/es/art%C3%ADculo/claves-privadas-y-p%C3%BAblicas/
- Ohayon, S. (2024, 26 septiembre). The Risks of Expired SSL Certificates Explained - CrowdStrike. CrowdStrike. https://www.crowdstrike.com/en-us/blog/the-risks-of-expired-ssl-certificates/#:~:text=Once%20an%20SSL%20certificate%20expires,encryption%20mechanisms%20down%20the%20line.
- El papel de las matemáticas en la criptografía y la seguridad de los datos. (s. f.). https://clasesdematematicasenlinea.com/el-papel-de-las-matematicas-en-la-criptografia-y-la-seguridad-de-los-datos/
- 8 Quantum Computing Cybersecurity Risks [+ protection tips]. (s. f.). Palo Alto Networks. https://www.paloaltonetworks.co.uk/cyberpedia/what-is-quantum-computings-threat-to-cybersecurity#:~:text=La%20computaci%C3%B3n%20cu%C3%A1ntica%20amenaza%20la,las%20computadoras%20cu%C3%A1nticas%20sean%20viables.
