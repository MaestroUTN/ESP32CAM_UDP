#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"

// Configuración de Red
const char* ssid = "INFINITUM02C5";
const char* password = "5RCm9StdXh";
const char* udpAddress = "192.168.1.74"; // IP de tu PC con Java
const int udpPort = 4444;

WiFiUDP udp;
#define PACKET_SIZE 1432 // Tamaño óptimo para evitar fragmentación IP

// Pines AI-Thinker
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void configurarSensor() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    config.frame_size = FRAMESIZE_SVGA; 
    config.jpeg_quality = 10; // Calidad máxima         
    config.fb_count = 2; // Reducido a 1 para asegurar una sola captura limpia

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Error de inicio: 0x%x", err);
        return;
    }

    sensor_t * s = esp_camera_sensor_get();
    s->set_brightness(s, 0);
    s->set_sharpness(s, 2);
    s->set_denoise(s, 0);
    s->set_vflip(s, 0);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {Serial.print(".");delay(500);}
    Serial.println("WiFi conectado");
    udp.begin(udpPort);
    
    configurarSensor(); // Usa la configuración UXGA/JPEG que ya validamos
}

void enviarImagenUDP() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) return;

    size_t total_len = fb->len;
    uint8_t * data = fb->buf;

    // Enviar encabezado de inicio con el tamaño total
    udp.beginPacket(udpAddress, udpPort);
    udp.print("START|");
    udp.print(total_len);
    udp.endPacket();

    // Enviar los datos en fragmentos
    size_t sent_len = 0;
    while (sent_len < total_len) {
        size_t chunk_size = min((size_t)PACKET_SIZE, total_len - sent_len);
        udp.beginPacket(udpAddress, udpPort);
        udp.write(data + sent_len, chunk_size);
        udp.endPacket();
        sent_len += chunk_size;
        delay(2); // Pequeño delay para no saturar el buffer del receptor
    }

    esp_camera_fb_return(fb);
}

void enviarImagenRapida() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) return;

    // Enviamos el tamaño en un paquete pequeño inicial
    udp.beginPacket(udpAddress, udpPort);
    udp.write((uint8_t*)&fb->len, 4/*sizeof(fb->len)*/); // Envío binario, más rápido que String
    udp.endPacket();

    // Transmisión por bloques sin delays innecesarios
    size_t sent = 0;
    while (sent < fb->len) {
        size_t chunk = min((size_t)PACKET_SIZE, fb->len - sent);
        udp.beginPacket(udpAddress, udpPort);
        udp.write(fb->buf + sent, chunk);
        udp.endPacket();
        sent += chunk;
        delayMicroseconds(800);//delayMicroseconds(500)
    }
    esp_camera_fb_return(fb);
}

void loop() {
    //enviarImagenUDP();
    enviarImagenRapida();
    //delay(10); // Una foto cada 5 segundos
}