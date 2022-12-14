
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
#define SCREEN_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int numberOfNetworks;
unsigned long lastAusgabe;
const unsigned long intervall = 5000;

const char *ssid = "YOUR-SSID";
const char *password = "YOUR-PSK";




//Specifying the Webserver instance to connect with HTTP Port: 80
ESP8266WebServer server(80);




String HTML()
{
  String msg="<!DOCTYPE html> <html>\n";
  msg+="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  msg+="<title>BlackLeakz Control</title>\n";
  msg+="<style>html{font-family:Helvetica; display:inline-block; margin:0px auto; text-align:center;}\n";
  msg+="body{margin-top: 50px; background-color: #838383; color: #00FF83;} h1{color: #00FF83; margin: 50px auto 30px;} h3{color:#00FF83; margin-bottom: 50px;}\n";
  msg+=".button{display:block; width:80px; background-color:#3B3B3B; border:none; color: #00FF83; padding: 13px 30px; text-decoration:none; font-size:25px; margin: 0px auto 35px; cursor:pointer; border-radius:4px;}\n";
  msg+=".button-on{background-color:#202020;}\n";
  msg+=".button-on:active{background-color:#202020;}\n";
  msg+=".button-off{background-color:#202020;}\n";
  msg+=".button-off:active{background-color:#202020;}\n";
  msg+="</style>\n";
  msg+="</head>\n";
  msg+="<body>\n";
  msg+="<h1>ESP8266 Web Server</h1>\n";
  msg+="<h3>Using Station (STA) Mode</h3>\n";
  msg+="<p>Scanner</p><a class=\"button button-on\" href=\"/scan\">Scan</a>\n";
  msg+="<p>LED Control</p><a class=\"button button-on\" href=\"/ledon\">ON</a>\n";
  msg+="<p>LED Control</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>\n";



  msg+="</body>\n";
  msg+="</html>\n";
  return msg;
}

void defaultdisp() {
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("Your SSID: ");
  display.display();
  display.setCursor(0,10);
  display.print(ssid);
  display.display();
  display.setCursor(0,20);
  display.print("Your password: ");
  display.display();
  display.setCursor(0,30);
  display.print(password);
  display.display();
  display.setCursor(0,40);
  display.print("IP address<>>: ");
  display.display();
  display.setCursor(0,50);
  IPAddress IP = WiFi.localIP();
  display.print(IP);
  display.display();
}

void lcdanzeige() {
  
    if (numberOfNetworks <= 0)
    {
        numberOfNetworks = WiFi.scanNetworks();
        Serial.print(numberOfNetworks);
        Serial.println(F(" gefundene Netzwerke"));
    }
    else if (millis() - lastAusgabe > intervall)
    {
        numberOfNetworks--;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Network-name: ");
        display.setCursor(0, 10);
        display.print(WiFi.SSID(numberOfNetworks));
        display.setCursor(0, 20);
        display.print("Signal strength: ");
        display.setCursor(0, 30);
        display.print(WiFi.RSSI(numberOfNetworks));
        display.display();
        Serial.print("Network name: ");
        Serial.println(WiFi.SSID(numberOfNetworks));
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(numberOfNetworks));
        Serial.println("-----------------------");
        lastAusgabe = millis();
  }
}


void handle_OnConnect()
{
  Serial.println("ConsoleLog<>> Client connected.");
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("Client connected. ");
  display.display();
  server.send(200, "text/html", HTML());
  delay(2000);
  defaultdisp();
}

void handle_ledon()
{
  Serial.println("ConsoleLog<>> LED ON.");
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("Set LED ON");
  display.display();
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "text/html", HTML());
  delay(3000);
  defaultdisp();
}

void handle_ledoff()
{
  Serial.println("ConsoleLog<>> LED Off.");
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  display.print("Set LED Off");
  display.display();
  digitalWrite(LED_BUILTIN, LOW);
  server.send(200, "text/html", HTML());
  delay(3000);
  defaultdisp();
}

void handle_scan()
{   
    display.clearDisplay();
    display.display();
    display.setCursor(0,0);
    display.print("Scanning");
    display.display();
    Serial.println("SCAN");
    delay(2000);
    display.clearDisplay();
    display.display();
    lcdanzeige();
    server.send(200, "text/html", HTML());
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}



void setup() {
  //Starting the serial communication channel
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("ConsoleLog<>> started.");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();
  delay(1000);




  //Connecting to the local WiFi network
  WiFi.begin(ssid ,password);

  //Keep checking the WiFi status until it is connected to the wifi network

  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi Connected with IP Address: ");
  Serial.println(WiFi.localIP());
  display.setCursor(0,0);
  display.print("IP address<>>: ");
  display.display();
  display.setCursor(0,10);
  IPAddress IP = WiFi.localIP();
  display.print(IP);
  display.display();

  //Specifying the functions which will be executed upon corresponding GET request from the client
  server.on("/",handle_OnConnect);
  server.on("/ledon",handle_ledon);
  server.on("/ledoff",handle_ledoff);
  server.on("/scan",handle_scan);
  server.onNotFound(handle_NotFound);

  //Starting the Server
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {

  if(WiFi.status()==WL_CONNECTED)//Check if ESP8266 is still connected to the internet
  {
    //Assign the server to handle the clients
    server.handleClient();


  }

    //Keep checking the WiFi status until it is connected to the wifi network
    while(WiFi.status()!=WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi Connected with IP Address: ");
    Serial.println(WiFi.localIP());
}
