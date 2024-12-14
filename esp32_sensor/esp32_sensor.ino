#include <DHT11.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DHT11PIN 4
DHT11 dht11(DHT11PIN);

const char* ssid = "Calin_IOT";
const char* password = "caliniot";
String url = "https://0e89-81-196-141-57.ngrok-free.app/api/data";
HTTPClient http;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void getTempHumidity(int& temp, int& humidity) {
    int result = dht11.readTemperatureHumidity(temp, humidity);
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        Serial.println(DHT11::getErrorString(result));
    }
}

void sendTempHumidity(int temp, int humidity) {
    StaticJsonDocument<200> doc;
    doc["temp"] = temp;
    doc["humidity"] = humidity;
    
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
    } else {
        Serial.print("Error in HTTP request: ");
        Serial.println(httpResponseCode);
    }
    
    http.end();
}

String getCurrentTime() {
    http.begin("https://timeapi.io/api/time/current/zone?timeZone=Europe/Bucharest");
    int httpResponseCode = http.GET();
    String date;

    if (httpResponseCode <= 0) {
        Serial.print("Error in HTTP request: ");
        Serial.println(httpResponseCode);
        http.end();
        return "";
    }

    String response = http.getString();
    // Serial.println("DateTime Response: ");
    // Serial.println(response);

    StaticJsonDocument<200> timeDoc;
    DeserializationError error = deserializeJson(timeDoc, response);
    if (error) {
        Serial.print("Error parsing time API response: ");
        Serial.println(error.c_str());
        http.end();
        return "";
    }

    date = timeDoc["year"].as<String>() + 
    "-" + timeDoc["month"].as<String>() + 
    "-" + timeDoc["day"].as<String>() + "T"
    " " + timeDoc["hour"].as<String>() +
    ":" + timeDoc["minute"].as<String>() +
    ":" + timeDoc["seconds"].as<String>();
    Serial.print("Date: ");
    Serial.println(date);
    http.end();

    return date;
}

void loop() {
    int temperature = -1;
    int humidity = -1;

    getTempHumidity(temperature, humidity);

    StaticJsonDocument<200> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["date"] = getCurrentTime();
    String jsonData;
    serializeJson(doc, jsonData);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    Serial.print("Sending to ");
    Serial.println(url);

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Response: ");
        Serial.println(response);
    } else {
        Serial.print("Error in HTTP request: ");
        Serial.println(httpResponseCode);
    }

    delay(2000);
}