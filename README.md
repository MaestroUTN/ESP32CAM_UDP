# ESP32-CAM UDP Video Stream (800x600)

Este proyecto implementa una transmisión de video de baja latencia utilizando el protocolo **UDP** desde un módulo **ESP32-CAM**
hacia un cliente desarrollado en **Java (Swing)**. Está diseñado para aplicaciones de monitoreo en tiempo real, telemetría o 
proyectos educativos de IoT y Logística.

## 🚀 Características
- **Baja Latencia:** Uso de UDP binario puro para evitar el retardo del procesamiento de texto.
- **Resolución SVGA:** Captura y envío de imágenes en 800x600 píxeles.
- **Protocolo Eficiente:** Segmentación manual de paquetes para evitar la fragmentación IP y maximizar la fluidez.
- **Cliente Java Multi-hilo:** Recepción y renderizado independiente para evitar bloqueos en la interfaz.

## 🛠️ Requisitos
### Hardware
- Módulo **ESP32-CAM** (Modelo AI-Thinker).
- Programador FTDI o interfaz serie para carga de firmware.
- Fuente de alimentación estable (5V/2A recomendada).

### Software
- **Firmware:** VS Code con extensión PlatformIO o Arduino IDE.
- **Cliente:** Java JDK 8 o superior.

## 📂 Estructura del Proyecto
- `Firmware_ESP32/`: Código en C++ para el módulo de cámara.
- `Java_Client/`: Código fuente del receptor UDP y visualizador gráfico.

## ⚙️ Configuración e Instalación

### 1. Firmware (ESP32-CAM)
Edite las siguientes constantes en el archivo principal del firmware con sus credenciales:
C++:
  const char* ssid = "TU_WIFI";
  const char* password = "TU_PASSWORD";
  const char* udpAddress = "192.168.1.XX"; // IP de la computadora ejecutando Java
2. Cliente (Java):
Asegúrese de que el puerto 4444 esté libre en su firewall. Ejecute el cliente desde su IDE favorito o mediante terminal:

Bash
javac UdpCameraClient800x600.java
java UdpCameraClient800x600
📊 Optimizaciones Realizadas
Calidad JPEG: Ajustada a un valor de 10 0 12 para equilibrar nitidez y peso de red.

Buffer de Red: El cliente Java utiliza un buffer de sistema de 4MB para prevenir la pérdida de paquetes en 
ráfagas de alta resolución.

Gestión de Frames: Implementación de timeouts para descartar paquetes huérfanos y minimizar "rayas" horizontales
en la imagen.

Desarrollado por Edgar - Proyecto de integración hardware-software para aplicaciones de monitoreo e ingeniería.

**Licencia:** Licencia MIT en lo que a cualquier codigo o propiedad de mi parte: lo demas apliquelo como debe ser. 
