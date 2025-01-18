#include <DHT11.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DHT11PIN 4
#define LED_PIN 2
DHT11 dht11(DHT11PIN);

const int sensor_id = 3;
const char *ssid = "Calin_IOT";
const char *password = "caliniot";
String pastebin_url = "https://pastebin.com/raw/i0PiJ2Np";
String url;
int activity = 0;

HTTPClient http;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        Serial.print(".");
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
    Serial.println("\nConnected to WiFi!");

    http.begin(pastebin_url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        url = http.getString();
        Serial.print("URL found: ");
        if (url.indexOf("ngrok") != -1)
        {
            activity = 1;
        }
        Serial.println(url);
    }
    else {
        Serial.print("Didnt find the url: ");
        Serial.println(httpResponseCode);
    }
}

void getTempHumidity(int &temp, int &humidity) {
    int result = dht11.readTemperatureHumidity(temp, humidity);
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    }
    else {
        Serial.println(DHT11::getErrorString(result));
    }
}

void sendTempHumidity(int temp, int humidity) {
    StaticJsonDocument<200> doc;
    doc["temperature"] = temp;
    doc["humidity"] = humidity;
    doc["sensor_id"] = sensor_id;

    String jsonData;
    serializeJson(doc, jsonData);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Response: ");
        Serial.println(response);
    }
    else {
        Serial.print("Error in HTTP request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void loop()
{
    int temperature = -1;
    int humidity = -1;

    digitalWrite(LED_PIN, HIGH);
    if (activity)
    {
        getTempHumidity(temperature, humidity);
        sendTempHumidity(temperature, humidity);
    }
    digitalWrite(LED_PIN, LOW);

    delay(4000);
}