#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <SPI.h>
#include <Arduino.h>
//#include <TFT_eSPI.h>
#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_DC 2
#define TFT_CS 15
#define TFT_RST  4
// initialize ILI9341 TFT library
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
 

 
 
#define REPORTING_PERIOD_MS     1000
 
float temperature, humidity, BPM, SpO2, bodytemperature;
 
/*Put your SSID & Password*/
const char* ssid = "HEXABYTE_67DED1";  // Enter SSID here
const char* password = "B967DED1";  //Enter Password here
const char* www_username = "admin";
const char* www_password = "esp32";
int k = 100 ;  
int p,w ;

String a; 
PulseOximeter pox;
uint32_t tsLastReport = 0;
 
 
//TFT_eSPI tft = TFT_eSPI(); 
WebServer server(80);             
 
void onBeatDetected()
{
  Serial.println("Beat!");
  digitalWrite (5, HIGH); //turn buzzer on
  delay(100);
  digitalWrite (5, LOW);  //turn buzzer off
   
}
 
void setup() {
  Serial.begin(115200);
 tft.begin();
 // tft.init();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  
 //  pinMode(2, INPUT); // Setup for leads off detection LO +
//  pinMode(4, INPUT); // Setup for leads off detection LO -
  //pinMode(19, OUTPUT);
    
  pinMode (5, OUTPUT);
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
 
  //check wi-fi is connected to wi-fi network
  if (WiFi.status() != WL_CONNECTED) {
  delay(10000);
 Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
   tft.setCursor(40, 10);
       tft.setTextSize(2);
       tft.setTextColor(ILI9341_RED);
       tft.print("Got IP:");
       tft.println(WiFi.localIP());
    //   tft.fillTriangle(70, 110, 100, 110, 70, 160, ILI9341_RED);
    //   tft.fillRect(150, 120, 20,  100, ILI9341_RED);
   //     tft.fillRect(150, 100, 100,  20, ILI9341_RED);
     //  tft.fillTriangle(110, 110, 170, 110, 50, 130, ILI9341_RED);
    //   tft.fillRect(130, 120, 150,  120, ILI9341_RED);
 
  server.on("/", []() {
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }
    handle_OnConnect();
  });
  server.onNotFound(handle_NotFound);
 
  server.begin();
  Serial.println("HTTP server started");
 
  Serial.print("Initializing pulse oximeter..");
 
  if (!pox.begin()) {
    Serial.println("FAILED");
   // for (;;);
  } else {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
     
  }
 
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
  // Register a callback for the beat detection
 
}
void loop() {
  pox.update();
//  ArduinoOTA.handle(); 
  server.handleClient();
   
      
     pox.update();

   
 // tft.fillScreen(ILI9341_BLACK);
  
  
  
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) 
  {   
       pox.update();
      tft.setCursor(20, 50);
       tft.setTextSize(3);
       tft.setTextColor(ILI9341_BLUE);
       tft.println("PR(BPM)");
       tft.setCursor(190, 50);
       tft.setTextSize(3);
       tft.setTextColor(ILI9341_BLUE);
       tft.println("SPO2(%)");

        pox.update();
      tft.setCursor(20, 90);
       tft.setTextSize(4);
       tft.setTextColor(ILI9341_BLACK);
       tft.println(BPM);
      tft.setCursor(190, 90);
        tft.setTextColor(ILI9341_BLACK);
        tft.setTextSize(4);
       tft.println(SpO2);
        pox.update();
     BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
    
    Serial.print("BPM: ");
    Serial.println(BPM);
       tft.setCursor(20, 90);
       tft.setTextSize(4);
       tft.setTextColor(ILI9341_GREEN);
       tft.println(BPM);
       
        pox.update();
   
    
    Serial.print("SpO2: ");
   Serial.print(SpO2);
    Serial.println("%");
          tft.setCursor(190, 90);
        tft.setTextColor(ILI9341_GREEN);
        tft.setTextSize(4);
       tft.println(SpO2);
 
   
   
       pox.update();
       
       
    
     
    
    Serial.println("*********************************");
    Serial.println();
 
    tsLastReport = millis();
    pox.update();
  }
   
//  tft.fillRect(0,0,400,400,ILI9341_BLACK);
//  if ((digitalRead(2) == 1) || (digitalRead(4) == 1)) {
//    Serial.println('!');
//  }
//  else{
//    // send the value of analog input 0:
//      Serial.println(analogRead(A0));
//  
//  
//}
//delay(10);
 pox.update();
}
 
void handle_OnConnect() {
  
  server.send(200, "text/html", SendHTML(5, 5, BPM, SpO2, 5)); 
}
 
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
 
  String SendHTML(float temperature,float humidity,float BPM,float SpO2, float bodytemperature){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>ESP32 Patient Health Monitoring</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<meta http-equiv='refresh' content='10'>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
  ptr +="<style>";
   
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;background-color:#F0FFFF;}";
  ptr +="body{margin: 0px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +="table, th, td {border: 1px solid black;}";
  ptr +=".w3-green {margin-top:30%;}";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +="#side-by-sidee{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  ptr +=".temperature .reading{color: #F29C1F;}";
  ptr +=".humidity .reading{color: #3B97D3;}";
  ptr +=".BPM .reading{color: #FF0000;}";
  ptr +=".SpO2 .reading{color: #955BA5;}";
  ptr +=".bodytemperature .reading{color: #F29C1F;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:65px;}";
  ptr+=".idd{margin-left:18px;}";
  ptr+=".co{background-color:#483D8B;color:white;}";
  ptr+=".op{background-color:#F5F5F5;color:black;}";
  ptr +="</style>";
   ptr +="<script>";
    
         
  ptr +="setInterval(loadDoc,1000);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
   
  ptr +="</script>\n";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1> Patient Health Monitoring</h1>";
  
  ptr +="<div class='container'>";
  
   ptr +="<div class='data Heart Rate'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 40.542 40.541'height=40.541px id=Layer_1 version=1.1 viewBox='0 0 40.542 40.541'width=40.542px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M34.313,20.271c0-0.552,0.447-1,1-1h5.178c-0.236-4.841-2.163-9.228-5.214-12.593l-3.425,3.424";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293c-0.391-0.391-0.391-1.023,0-1.414l3.425-3.424";
  ptr +="c-3.375-3.059-7.776-4.987-12.634-5.215c0.015,0.067,0.041,0.13,0.041,0.202v4.687c0,0.552-0.447,1-1,1s-1-0.448-1-1V0.25";
  ptr +="c0-0.071,0.026-0.134,0.041-0.202C14.39,0.279,9.936,2.256,6.544,5.385l3.576,3.577c0.391,0.391,0.391,1.024,0,1.414";
  ptr +="c-0.195,0.195-0.451,0.293-0.707,0.293s-0.512-0.098-0.707-0.293L5.142,6.812c-2.98,3.348-4.858,7.682-5.092,12.459h4.804";
  ptr +="c0.552,0,1,0.448,1,1s-0.448,1-1,1H0.05c0.525,10.728,9.362,19.271,20.22,19.271c10.857,0,19.696-8.543,20.22-19.271h-5.178";
  ptr +="C34.76,21.271,34.313,20.823,34.313,20.271z M23.084,22.037c-0.559,1.561-2.274,2.372-3.833,1.814";
  ptr +="c-1.561-0.557-2.373-2.272-1.815-3.833c0.372-1.041,1.263-1.737,2.277-1.928L25.2,7.202L22.497,19.05";
  ptr +="C23.196,19.843,23.464,20.973,23.084,22.037z'fill=#26B999 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Heart Rate</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)BPM;
  ptr +="<span class='superscript'>BPM</span></div>";
  ptr +="</div>";
  
  
  ptr +="<div class='data humidity'>";
  ptr +="<div class='w3-container'>";
  ptr +="<table class='styled-table'>";
  ptr +="<tr>";
  ptr += "<th class='co'>Age</th>";
  ptr +="<th class='co'>18-25</th>";
  ptr +="<th class='co'>26-35</th>";
  ptr +="<th class='co'>36-45</th>";
  ptr +="<th class='co'>46-55</th>";
  ptr +="<th class='co'>56-65</th>";
  ptr +="<th class='co'>65+</th>";
  ptr +="</tr>";
  ptr +="<tr>";
  ptr +="<td class='op'>Athlete</td>";
  ptr +="<td class='op'>49-55</td>";
  ptr +="<td class='op'>49-54</td>";
  ptr +="<td class='op'>50-56</td>";
  ptr +="<td class='op'>50-57</td>";
  ptr +="<td class='op'>51-56</td>";
  ptr +="<td class='op'>50-55</td>";
  
  ptr +="</tr>";
  ptr +="<tr>";
  ptr +="<td class='op'>Exellent</td>";
  ptr +="<td class='op'>56-61</td>";
  ptr +="<td class='op'>55-61</td>";
  ptr +="<td class='op'>57-62</td>";
  ptr +="<td class='op'>58-63</td>";
  ptr +="<td class='op'>57-61</td>";
  ptr +="<td class='op'>56-61</td>";
  ptr +="</tr>";
  ptr +="<tr>";
  ptr +="<td class='op'>Good</td>";
  ptr +="<td class='op'>62-65</td>";
  ptr +="<td class='op'>62-65</td>";
  ptr +="<td class='op'>63-66</td>";
  ptr +="<td class='op'>64-67</td>";
  ptr +="<td class='op'>62-67</td>";
  ptr +="<td class='op'>62-65</td>";
  ptr +="</tr>";
  ptr +="<tr>";
  ptr +="<td class='op'>Average</td>";
  ptr +="<td class='op'>70-73</td>";
  ptr +="<td class='op'>71-74</td>";
  ptr +="<td class='op'>71-75</td>";
  ptr +="<td class='op'>72-76</td>";
  ptr +="<td class='op'>72-75</td>";
  ptr +="<td class='op'>70-73</td>";
  ptr +="</tr>";
   ptr +="<tr>";
  ptr +="<td class='op'>Poor</td>";
  ptr +="<td class='op'>82</td>";
  ptr +="<td class='op'>82+</td>";
  ptr +="<td class='op'>83+</td>";
  ptr +="<td class='op'>84+</td>";
  ptr +="<td class='op'>82+</td>";
  ptr +="<td class='op'>80+</td>";
  ptr +="</tr>";
ptr +="</table>"; 
  ptr +="</div>";
  ptr +="</div>";
  
  
  
  ptr +="<div class='data Blood Oxygen'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 58.422 40.639'height=40.639px id=Layer_1 version=1.1 viewBox='0 0 58.422 40.639'width=58.422px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M58.203,37.754l0.007-0.004L42.09,9.935l-0.001,0.001c-0.356-0.543-0.969-0.902-1.667-0.902";
  ptr +="c-0.655,0-1.231,0.32-1.595,0.808l-0.011-0.007l-0.039,0.067c-0.021,0.03-0.035,0.063-0.054,0.094L22.78,37.692l0.008,0.004";
  ptr +="c-0.149,0.28-0.242,0.594-0.242,0.934c0,1.102,0.894,1.995,1.994,1.995v0.015h31.888c1.101,0,1.994-0.893,1.994-1.994";
  ptr +="C58.422,38.323,58.339,38.024,58.203,37.754z'fill=#955BA5 /><path d='M19.704,38.674l-0.013-0.004l13.544-23.522L25.13,1.156l-0.002,0.001C24.671,0.459,23.885,0,22.985,0";
  ptr +="c-0.84,0-1.582,0.41-2.051,1.038l-0.016-0.01L20.87,1.114c-0.025,0.039-0.046,0.082-0.068,0.124L0.299,36.851l0.013,0.004";
  ptr +="C0.117,37.215,0,37.62,0,38.059c0,1.412,1.147,2.565,2.565,2.565v0.015h16.989c-0.091-0.256-0.149-0.526-0.149-0.813";
  ptr +="C19.405,39.407,19.518,39.019,19.704,38.674z'fill=#955BA5 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Blood Oxygen </div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)SpO2;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";
 
  ptr +="<div class='data Body Temperature'>";
  ptr +="<table class='idd'>";
  ptr +="<tr>";
  
  ptr += "<th class='co'>Blood Oxygen Level </th>";
  ptr +="<th class='co'>Indication</th>";
  ptr +="</tr>";
  ptr +="<tr>";
  ptr += "<th class='op'>Healthy </th>";
  ptr +="<th class='op'>95%+</th>";
  ptr +="</tr>";
  ptr += "<th class='op'>Normal in people with COPD</th>";
  ptr +="<th class='op'>88% to 95%</th>";
  ptr +="</tr>";
  ptr += "<th class='op'>Hypoxic</th>";
  ptr +="<th class='op'>85% to 95%</th>";
  ptr +="</tr>";
  ptr += "<th class='op'>Severely Hypoxic</th>";
  ptr +="<th class='op'>Less Than 85% </th>";
  ptr +="</tr>";
  ptr +="</table>"; 
  
  ptr +="<div class='side-by-side reading'>";
   
   
  ptr +="</div>";
  
  ptr +="</div>";
   
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}
