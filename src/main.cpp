// Set up ESP8266 network functionality
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Set up DS18B20 support
#include <DallasTemperature.h>
#include <OneWire.h>
// Data wire is connteced to pin 2
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature DS18B20(&oneWire);

// Set up sensors
#include "Sensors.h"
#define SENSOR_COUNT 3
// Production Sensors
Sensors sensors[SENSOR_COUNT] = {
	{"Front", 0, {0x28, 0xD2, 0x2E, 0x07, 0xD6, 0x01, 0x3C, 0x79}, false},
	{"Back", 0, {0x28, 0xF7, 0xFC, 0x7, 0xD6, 0x1, 0x3C, 0xF9}, false},
	{"Top", 0, {0x28, 0x28, 0x22, 0x7, 0xD6, 0x1, 0x3C, 0xE5}, false}
};

// WiFi credentials
const char* ssid = "Verati_24";
const char* password = "GU14Elv1ra!8RW";

ESP8266WebServer server(80);

void setCrossOrigin() {
    // Prevent problems with cross-site rules when using the HTML
    // to grab the data with Javascript on the client
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

String formatAddress(DeviceAddress addr) {
    String out;
    byte i;
    for (i = 0; i < 8; i++) {
        if (addr[i] < 16) {
            out = out + '0';
        }
        out = out + String(addr[i], HEX);
        if (i < 7) {
            out = out + ":";
        }
    }
    return out;
}

String formatDSAddress(DeviceAddress addr) {
    String out;
	String temp;
    byte i;
    for (i = 0; i < 8; i++) {
        out = out + "0x";
        if (addr[i] < 16) {
            out = out + '0';
        }
		temp = String(addr[i], HEX);
		temp.toUpperCase();
        out = out + temp;
        if (i < 7) {
            out = out + ", ";
        }
    }
    return out;
}

String getSensorJson() {
	// Simple function to construct the JSON for output
    String out = "[\n";
    for (int ii = 0; ii < SENSOR_COUNT; ii++) {
        out = out + "  {\n";
        out = out + "    \"name\": \"" + sensors[ii].Name + "\",\n";
        out = out + "    \"address\": \"" + formatAddress(sensors[ii].Address) + "\",\n";
        out = out + "    \"temperature\": " + String(sensors[ii].Temperature) + ",\n";
        out = out + "    \"present\": " + String(sensors[ii].Present) + "\n";
        if (ii == (SENSOR_COUNT - 1)) {
            out = out + "  }\n";
        } else {
            out = out + "  },\n";
        }
    }
    out = out + "]\n";
    return out;
}

void handleRoot() {
    DS18B20.requestTemperatures();
    for (int ii = 0; ii < SENSOR_COUNT; ii++) {
        sensors[ii].Present = DS18B20.requestTemperaturesByAddress(sensors[ii].Address);
		sensors[ii].Temperature = DS18B20.getTempC(sensors[ii].Address);
    }

	// Sent the output back to the requestor
	String json = getSensorJson();
	// Serial.println(json);
    setCrossOrigin();
    server.send(200, "text/html", json);
}

void handleSensorList() {
	DeviceAddress sensorAddress;
	float Temperature;
	String out;
	out = "<html><body><h2>Discovered sensors:</h2><ul>";
    DS18B20.requestTemperatures();
    for (int ii = 0; ii < SENSOR_COUNT; ii++) {
		DS18B20.getAddress(sensorAddress, ii);
		Temperature = DS18B20.getTempCByIndex(ii);
		out = out + "<li>" + ii + ": " + formatDSAddress(sensorAddress) + " = " + Temperature + "C</li>";
	}
	out = out + "</ul></body></html>";
    server.send(200, "text/html", out);
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.println();

    // Should really set IP address with DHCP reservation in Pi-Hole but for now...
    IPAddress ip(10, 0, 0, 82);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress gateway(10, 0, 0, 1);
    IPAddress dns(10, 0, 0, 4);
    WiFi.config(ip, gateway, subnet, dns);
    Serial.print("Connecting to " + String(ssid));
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected at ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting HTTP server");
    server.on("/", handleRoot);
	server.on("/sensors", handleSensorList);
    server.begin();
    Serial.println("HTTP server started");

    // Set resolution to 10-bit (0.25 degrees) for all DS18B20
    for (int ii = 0; ii < SENSOR_COUNT; ii++) {
        DS18B20.setResolution(sensors[ii].Address, 10);
    }

    DS18B20.begin();
    Serial.println("Sensors started");
}

void loop() {
    server.handleClient();
}