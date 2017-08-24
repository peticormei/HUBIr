#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include <IRremoteESP8266.h> //use v1.2.0
#include <ProntoHex.h> //https://github.com/probonopd/ProntoHex

const char *ap_ssid = "HUBIr";
const char *ap_pwd = "ufrpefeliz";
const char *static_json = "{\"status\":\"ok\"}";

ESP8266WebServer server(80);

IRsend irsend(D2); //an IR led is connected to GPIO pin 4 (D2)
ProntoHex ph = ProntoHex();

void setup() {
  delay(1000);
  Serial.begin(115200);
  irsend.begin();

  try_connect("", ""); //blank str for the first init, the program use the previous WiFi config
  
  server.on("/new_ssid", new_ssid);
  server.on("/ir", send_ir); //Associate the handler function to the path
  
  server.begin(); //Start the server
  Serial.println();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); //Handling of incoming requests
}


void send_ir() { //Handler for the body path
  String string_json = server.arg("plain");
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(string_json);
  
  String pronto_hex = json["command"];
  ph.convert(pronto_hex);
  irsend.sendRaw(ph.convertedRaw, ph.length, ph.frequency);

  Serial.println();
  Serial.println("* IR command received *");

  server.send(200, "text/plain", static_json);
}


void new_ssid() { //Handler for the body path
  String string_json = server.arg("plain");
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(string_json);
  
  String ssid = json["ssid"];
  String pwd = json["pwd"];

  Serial.println();
  Serial.println("* New wifi config received *");

  server.send(200, "text/plain", static_json);

  delay(1000);
  try_connect(ssid, pwd);
}


void try_connect(String ssid, String pwd) {
  if (ssid.equals("") or pwd.equals("")) { //PENSANR EM ALGO MELHOR =)
    //first init
    Serial.println();
    Serial.println("Try to connect saved wifi config");
    
    WiFi.begin();
  
    for (int i=0; i <= 10; i++){
      if (WiFi.status() != WL_CONNECTED) {  //Wait for connection
        delay(1000);
        Serial.println("Waiting to connect...");
      } else {
        break;
      }
    }
  } else {
    //config wifi with json file
    //WiFi.softAPdisconnect(false);
    WiFi.enableAP(false);
    Serial.println("");
    Serial.println("Turn off AP mode");
    delay(500);
  
    Serial.println("");
    WiFi.disconnect();
    Serial.println("Disconnect from previous WiFi");
    delay(500);
  
    Serial.println();
    Serial.print("Try to connect: ");
    Serial.print(ssid);
    Serial.println();
    WiFi.begin(ssid.c_str(), pwd.c_str());  //Connect to the WiFi network
    for (int i=0; i <= 10; i++){
      if (WiFi.status() != WL_CONNECTED) {  //Wait for connection
        delay(1000);
        Serial.println("Waiting to connect...");
      } else {
        break;
      }
    } 
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.print("Configuring access point...");
    
    WiFi.softAP(ap_ssid, ap_pwd);
    
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  } else {
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //Print the local IP
    
    //Persist wifi config
    WiFi.persistent(true);
    delay(1000);
  }
}
