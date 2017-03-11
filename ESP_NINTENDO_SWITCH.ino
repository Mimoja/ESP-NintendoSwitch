
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

DNSServer dnsServer;
ESP8266WebServer webServer(80);


String HTML_CONTENT =
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>Captive Portal!</h1><p>This is a template. "
  "Use <a href='/content'> the content site </a> to change this site</p></body></html>";


const char* AP_SSID="SwitchHaxx";
const char* EX_SSID = "Northern Lights";
const char* PASSWD = "technikeristinformiert";
const IPAddress myIP(10, 0, 0, 42);
const IPAddress subnetmask(255, 255, 255, 0);

const char* events[] = {
    "WIFI_EVENT_STAMODE_CONNECTED ",
    "WIFI_EVENT_STAMODE_DISCONNECTED",
    "WIFI_EVENT_STAMODE_AUTHMODE_CHANGE",
    "WIFI_EVENT_STAMODE_GOT_IP",
    "WIFI_EVENT_STAMODE_DHCP_TIMEOUT",
    "WIFI_EVENT_SOFTAPMODE_STACONNECTED",
    "WIFI_EVENT_SOFTAPMODE_STADISCONNECTED",
    "WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED",
    "WIFI_EVENT_MAX",

};

void WiFiEvent(WiFiEvent_t event) {
    

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case WIFI_EVENT_SOFTAPMODE_STACONNECTED:
            Serial.println("WIFI Client connected");
            break;
        case WIFI_EVENT_SOFTAPMODE_STADISCONNECTED:
            Serial.println("WIFI Client disconnected");
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("Wifi disconnected");
            break;
        case WIFI_EVENT_STAMODE_CONNECTED:
             Serial.println("Wifi connected");
        case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
            break;
        default:
            Serial.printf("[WiFi-event] event: %d %s\n", event, events[event]);
            break;
    }
}

void setup() {
  // Encode quotes for HTML

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Starting AP...");
  WiFi.onEvent(WiFiEvent);
  WiFi.softAPConfig(myIP, myIP, subnetmask);
  WiFi.softAP(AP_SSID);
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.println("Connecting to external AP");

  WiFi.begin(EX_SSID, PASSWD);

  dnsServer.start(53, "*", myIP);
  
  webServer.on("/content", []() {
    webServer.send(200, "text/html",   "<!DOCTYPE html><html><head><title>CaptivePortal Content Editor</title></head><body>"
    "<form action='/'><input type='submit' value='back' style=\"width:90px; height:40px;\"/></form>"
    "<form method='POST' action='contentsave'><h4>Set Captive portal content</h4>"
    "<textarea cols='120' rows='40' name='content' style=\"width:1500px; height:900px;\" >" + HTML_CONTENT +"</textarea>"
    "<br /><input type='submit' value='commit' style=\"width:90px; height:40px;\"/></form></body></html>");
  });

  
  webServer.on("/contentsave", []() {
      Serial.println("new content save");
      HTML_CONTENT = webServer.arg("content");
      webServer.sendHeader("Location", "content", true);
      webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      webServer.sendHeader("Pragma", "no-cache");
      webServer.sendHeader("Expires", "-1");
      webServer.send ( 302, "text/plain", ""); 
      webServer.client().stop(); 
      Serial.println(HTML_CONTENT);
  });
  
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", HTML_CONTENT);
  });
  webServer.begin();
  
  Serial.println("Waiting for connections. ");
  
}
  
void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}

