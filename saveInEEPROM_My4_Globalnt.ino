
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266Ping.h>

#define relay 2

ESP8266WebServer server;

char* mySsid = "ESP";
char* myPassword ="12345678";

IPAddress ip(192,168,11,1);
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);

  String essid;
  String epass;
  String epingTarget;
  String etime;
  String ectr;
  String ects;
  

WiFiClient client;

unsigned long previousMillis = 0;


void setup()
{
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  
  Serial.begin(115200);

  //----------Reading int from EEPROM-------//
  
  EEPROM.begin(512);
  delay(10);
  Serial.println("Reading EEPROM ssid");
  
 //-----------Reading strings-------------//

 // SSID
 
  for (int i = 0; i < 32; ++i)
    {
      essid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(essid.c_str());//оператор, отдаёт содержимое строки в типе char
  Serial.println("Reading EEPROM pass");

//  PASSWORD

  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass);

//  PING TARGET


  for (int i = 96; i < 128; ++i)
    {
      epingTarget += char(EEPROM.read(i));
    }  
  Serial.print("PING TARGET: ");
  Serial.println(epingTarget.c_str());

// TIME TO CHECK

  for (int i = 128; i < 130; ++i)
    {
      etime += char(EEPROM.read(i));
    }  
  Serial.print("TIME TO CHECK: ");
  Serial.println(etime);

// COUNT TO RESTART

  for (int i = 130; i < 132; ++i)
    {
      ectr += char(EEPROM.read(i));
    }  
  Serial.print("COUNT TO RESTART: ");
  Serial.println(ectr);

// COUNT TO STOP

  for (int i = 132; i < 134; ++i)
    {
      ects += char(EEPROM.read(i));
    }  
  Serial.print("COUNT TO STOP: ");
  Serial.println(ects);

  
  //----------end of reading data---------//
  
  //----------starting AT or STA----------//

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(essid.c_str(),epass.c_str());

  byte attemps = 10;
  
  while (--attemps && WiFi.status()!=WL_CONNECTED) // пытаемся раз десять подключиться к роутеру
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status()!=WL_CONNECTED){

    Serial.println();
    Serial.println("WiFi_AP_BEGIN");
    WiFi.disconnect(); // отключаем предыдущий режим
    WiFi.mode(WIFI_AP_STA);//для обнаружения других точек
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP(mySsid, myPassword);
    
    }
  else{
      Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  }
  
//------processing requests------//

  server.on("/", messageGetIn);//при обращении получаем форму
  server.on("/WGC", argsGetOut);//отправляем запрос на перезапись параметров и рестарт esp
  server.begin();

  /*for (int i = 0; i < WiFi.scanNetworks();i++){

  Serial.println (WiFi.SSID(i));
  }*/
  
}

void loop()
{
  server.handleClient();
  byte resetCounter = 0;
  byte stopCounter = 0;
  boolean pingState;

  if (stopCounter <= ects.toInt()){
    
    if (resetCounter <= ectr.toInt()){

      if (millis() - previousMillis >= etime.toInt()*1000) {
      boolean pingState = Ping.ping(epingTarget.c_str());
      if(pingState != 0){
      resetCounter = 0;
      digitalWrite (relay, HIGH);
      Serial.println (millis() - previousMillis);
      previousMillis = millis();
      }
    }
      else{
      resetCounter++;
      Serial.println (millis() - previousMillis);
      previousMillis = millis();
      digitalWrite (relay, LOW);
      }
    }
    else {
     digitalWrite (relay, LOW);
     delay(1000);
     digitalWrite (relay, HIGH);
     delay(1000);
  }
}
  else {
    while(pingState != 1){
      Serial.println ("check your network");
    }
    }
    delay (10000);
  }


void messageGetIn (){
 Serial.begin(115200);
 delay(10);

  //----------Reading int from EEPROM-------//
  
  Serial.println("Reading EEPROM ssid");
  Serial.println(essid.c_str());//оператор, отдаёт содержимое строки в типе char
  Serial.println("Reading EEPROM pass");
  Serial.print("PASS: ");
  Serial.println(epass);
  Serial.print("PING TARGET: ");
  Serial.println(epingTarget);
  Serial.print("TIME: ");
  Serial.println(etime);
  Serial.print("COUNT TO RESTART: ");
  Serial.println(ectr);
  Serial.print("COUNT TO STOP: ");
  
  
  //----------end of reading data---------//
  
String webPage = "";
 /* webPage += "<p style=\"text-align:center\"><strong>WATCHDOG CONFIGURATOR 2</strong></p>\n\r";
  webPage += "<form action=\"WGC\" enctype=\"text/plain\" id=\"WGC\" method=\"get\" name=\"WGC\">\n\r";
  webPage += "<p style=\"text-align:center\">SSID</p>\n\r";
  webPage += "<p style=\"text-align:center\"><input maxlength=\"32\" name=\"SSID\" type=\"text\" /></p>\n\r";
  webPage += "<p style=\"text-align:center\">PASSWORD</p>\n\r";
  webPage += "<p style=\"text-align:center\"><input maxlength=\"64\" name=\"PASSWORD\" type=\"password\" /></p>\n\r";
  webPage += "<p style=\"text-align:center\">TIME TO CHECK</p>\n\r";
  webPage += "<p style=\"text-align:center\"><select name=\"TIMETOCHECK\"><option value=\"1\">1</option><option value=\"5\">5</option><option value=\"10\">10</option><option value=\"15\">15</option><option value=\"20\">20</option></select></p>\n\r";
  webPage += "<p style=\"text-align:center\"><input name=\"SAVE\" type=\"submit\" value=\"SAVE\" /></p>\n\r";
  webPage += "</form>\n\r";
  webPage += "<p>&nbsp;</p>\n\r";*/
webPage += "<p style=\"text-align:center\"><strong>WATCHDOG CONFIGURATOR 2</strong></p>\n\r";
webPage += "<form action=\"WGC\" enctype=\"text/plain\" id=\"WGC\" method=\"get\" name=\"WGC\">\n\r";
webPage += "<p style=\"text-align:center\">SSID</p>\n\r";
webPage += "<p style=\"text-align:center\"><select name=\"SSID\" size=\"1\">";

 for (int i = 0; i < WiFi.scanNetworks();i++){//WiFi.scanNetworks()
  String previouString = "";
  while (previouString.equals(WiFi.SSID(i))){
    delay (10);
    }
  previouString = WiFi.SSID(i);
  webPage += "<option value=";
  webPage += WiFi.SSID(i);
  webPage += ">";
  webPage += WiFi.SSID(i);
  }

webPage += "</select></p>\n\r";
webPage += "<p style=\"text-align:center\">PASSWORD</p>\n\r";
webPage += "<p style=\"text-align:center\"><input maxlength=\"64\" name=\"PASSWORD\" type=\"password\" /></p>\n\r";
webPage += "<p style=\"text-align:center\">PING TARGET</p>\n\r";
webPage += "<p style=\"text-align:center\"><input name=\"PINGT\" type=\"text\" /></p>\n\r";
webPage += "<p style=\"text-align:center\">TIME TO CHECK</p>\n\r";
webPage += "<p style=\"text-align:center\"><select name=\"TIMETOCHECK\"><option value=\"1\">1</option><option value=\"5\">5</option><option value=\"10\">10</option><option value=\"15\">15</option><option value=\"20\">20</option><option value=\"30\">30</option><option value=\"45\">45</option><option value=\"60\">60</option></select></p>\n\r";
webPage += "<p style=\"text-align:center\">COUNT TO RESTART</p>\n\r";
webPage += "<p style=\"text-align:center\"><select name=\"CTR\"><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option></select></p>\n\r";
webPage += "<p style=\"text-align:center\">COUNT TO STOP</p>\n\r";
webPage += "<p style=\"text-align:center\"><select name=\"CTS\"><option value=\"1\">1</option><option value=\"2\">2</option><option value=\"3\">3</option><option value=\"4\">4</option><option value=\"5\">5</option><option value=\"6\">6</option><option value=\"7\">7</option><option value=\"8\">8</option><option value=\"9\">9</option><option value=\"10\">10</option></select></p>\n\r";
webPage += "<p style=\"text-align:center\"><input name=\"SAVE\" type=\"submit\" value=\"SAVE&amp;RESET\" /></p>\n\r";
webPage += "</form>\n\r";
webPage += "<p>&nbsp;</p>\n\r";

  server.send(200, "text/html", webPage);
}


void argsGetOut (){
   delay(10);

//--------getting message from client---------// 
   String qsidName = server.argName(0);
   Serial.println(qsidName);
   String qsid = server.arg(0);
   Serial.println(qsid);
   
   String qpassName = server.argName(1);
   Serial.println(qpassName);
   String qpass = server.arg(1);
   Serial.println(qpass);
   
   String pingTargetName = server.argName(2);
   Serial.println(pingTargetName);
   String pingTarget = server.arg(2);
   Serial.println(pingTargetName);

   String qtimeName = server.argName(3);
   Serial.println(qtimeName);
   String qtime = server.arg(3);
   Serial.println(qtime);

   String ctrName = server.argName(4);
   Serial.println(ctrName);
   String ctr = server.arg(4);
   Serial.println(ctr);

   String ctsName = server.argName(5);
   Serial.println(ctsName);
   String cts = server.arg(5);
   Serial.println(cts);
   

   

//------ERASE EEPROM-----//
  
     if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 134; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
          
//----------EEPROM WRITE--------//
 
    Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
           {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
    Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
    Serial.println("writing ping target:"); 
          for (int i =0; i < pingTarget.length();++i)
          {
              EEPROM.write(96+i, pingTarget[i]);
              Serial.print("Wrote: ");
              Serial.println(pingTarget[i]);
          }
          
    Serial.println("writing ping time:"); 
          for (int i =0; i < qtime.length();++i)
          {
              EEPROM.write(128+i, qtime[i]);
              Serial.print("Wrote: ");
              Serial.println(qtime[i]);
          }
         
    Serial.println("writing count to reset:"); 
          for (int i =0; i < ctr.length();++i)
          {
              EEPROM.write(130+i, ctr[i]);
              Serial.print("Wrote: ");
              Serial.println(ctr[i]);
          }
          
    Serial.println("writing count to stop:"); 
          for (int i =0; i < cts.length();++i)
          {
              EEPROM.write(132+i, cts[i]);
              Serial.print("Wrote: ");
              Serial.println(cts[i]);
          }
          EEPROM.commit();

          
          
  //--------------------------WEBPAGE-------------------------//
            
  String webPage = "";
  webPage += "<p style=\"text-align:center\"><strong>WATCHDOG CONFIGURATOR 2</strong></p>\n\r";
  webPage += "<form action=\"WGC\" enctype=\"text/plain\" id=\"WGC\" method=\"get\" name=\"WGC\">\n\r";
  webPage += "<p style=\"text-align:center\">SSID</p>\n\r";
  webPage += "<p style=\"text-align:center\"><input maxlength=\"32\" name=\"SSID\" type=\"text\"></p>\n\r";
  webPage += "<p style=\"text-align:center\">PASSWORD</p>\n\r";
  webPage += "<p style=\"text-align:center\"><input maxlength=\"64\" name=\"PASSWORD\" type=\"password\" /></p>\n\r";
  webPage += "<p style=\"text-align:center\">TIME TO CHECK</p>\n\r";
  webPage += "<p style=\"text-align:center\"><select name=\"TIMETOCHECK\"><option value=\"1\">1</option><option value=\"5\">5</option><option value=\"10\">10</option><option value=\"15\">15</option><option value=\"20\">20</option></select></p>\n\r";
  webPage += "<p style=\"text-align:center\"><input name=\"SAVE\" type=\"submit\" value=\"SAVE\" /></p>\n\r";
  webPage += "</form>\n\r";
  webPage += "<p>&nbsp;</p>\n\r";
  
  server.send(200, "text/html", webPage);
  }
  ESP.restart();
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      Serial.println("None");
      break;
    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;
  }
}

