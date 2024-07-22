#include <ESP8266WiFi.h>        // Подключаем библиотеку для работы с Wi-Fi
#include <ESP8266HTTPClient.h>  // Подключаем библиотеку для HTTP-запросов
#include <WiFiClientSecure.h>   // Подключаем библиотеку для HTTPS-запросов

// Замените эти строки на SSID и пароль вашей Wi-Fi сети
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Bearer токен для авторизации
const char* bearerToken = "YOUR_BEARER_TOKEN";

// Настройки кнопки
const int buttonPin = D1;
bool buttonState = HIGH;  // Состояние кнопки

void setup() {
  // Инициализируем последовательный порт для отладки
  Serial.begin(115200);
  delay(10);

  // Настраиваем пин кнопки как вход
  pinMode(buttonPin, INPUT_PULLUP);

  // Подключаемся к Wi-Fi сети
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int currentState = digitalRead(buttonPin);
  
  if (currentState == LOW && buttonState == HIGH) {
    buttonState = LOW;
    sendSMS();
  } else if (currentState == HIGH) {
    buttonState = HIGH;
  }
}

void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {  // Проверяем соединение с Wi-Fi
    WiFiClientSecure client; // Создаем объект WiFiClientSecure
    client.setInsecure(); // Отключаем проверку сертификата (для простоты, не рекомендуется для production)

    HTTPClient http;

    // Устанавливаем URL для POST-запроса
    http.begin(client, "https://api.exolve.ru/messaging/v1/SendSMS");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", String("Bearer ") + bearerToken);

    // Формируем JSON-данные
    String jsonPayload = "{\"number\":\"SENDER_NUMBER\",\"destination\":\"RECIPIENT_NUMBER\",\"text\":\"СМС отправленный из NodeMCU\"}";

    // Отправляем POST-запрос
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Завершаем запрос
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}
