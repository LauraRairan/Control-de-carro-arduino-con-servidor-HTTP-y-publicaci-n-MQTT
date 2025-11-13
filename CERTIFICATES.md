# CERTIFICATES.md

## Pruebas de conexión segura MQTT con ESP32

### Etapa 1:
Modificar el código de conexión a MQTT para usar un puerto seguro. Sin hacer más cambios verificar que la conexión sigue  funcionando (¿o no?).

  * Conexión exitosa pero insegura
    Evidencia: <img width="1919" height="1146" alt="image" src="https://github.com/user-attachments/assets/8de01d90-f5fb-4576-8d04-aa3c04b4206f" />
               <img width="1919" height="1142" alt="image" src="https://github.com/user-attachments/assets/96dd1a2d-0aba-42be-b429-daaba1efa7eb" />

    Vemos que, al usar un puerto seguro como el 8883 y al usar espClient.setInsecure(). La conexión se establece correctamente pero sin verificar el certificado.
    
### Etapa 2:
Realizar el cambio para validar certificados, verificar que sin más cambios la comunicación falla (sin cargar el certificado al ESP32).
  
  * Fallo esperado — sin CA cargada  
    Evidencia: <img width="1919" height="1141" alt="image" src="https://github.com/user-attachments/assets/f3255597-97f1-4c65-960c-d9a6f05df33d" />

    Vemos que, al quitar espClient.setInsecure(). Se valida el certificado al momento de hacer la conexión pero como no lo hemos cargado a la ESP32, la conexión
    simplemente falla.
     
### Etapa 3: Validación sin certificados
  
  * Conexión segura con CA validada 
    Evidencia: <img width="1919" height="1141" alt="image" src="https://github.com/user-attachments/assets/ac6ac9ad-e22c-486d-b327-38766258cb55" />
               <img width="1919" height="1139" alt="image" src="https://github.com/user-attachments/assets/2c19a7a4-e3d1-4f29-bb64-619462634292" />
               <img width="1919" height="1145" alt="image" src="https://github.com/user-attachments/assets/04c3efec-4c98-428b-a55d-7ab0e6d39ade" />

    Vemos que, al cargar el certificado en la ESP32. La conexión se se establece correctamente y esta vez de forma segura.
