// 1/12/22
// Import required libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "ESPAsyncWebServer.h"
#include <WiFiClient.h>
#include <espnow.h>
#include <stdlib.h>

// Structure example to receive data
// Must match the sender structure

typedef struct testingData { //percentage packet loss for testing, to DS
  double totalJoinDSAve1;//
  double totalJoinSNAve1;//

  double scheduleAve1;//
  double TotalScheduleAve1;//

  double aveDS1;//
  double totalAveDS1;//

  double aveSN1;//
  double totalAveSN1;//

  int MacNum1;
  int periodCount1;
  int currentPeriod1;
} testingData;
testingData testingData1;


typedef struct allData {
  //  int nodeNum; //how many nodes there are
  double windSpeed1;
  int windDir1;
  float tempC1;
  uint16_t capRead1;
  float t1;
  float h1;

  double windSpeed2;
  int windDir2;
  float tempC2;
  uint16_t capRead2;
  float t2;
  float h2;

  double windSpeed3;
  int windDir3;
  float tempC3;
  uint16_t capRead3;
  float t3;
  float h3;
} allData;

allData allData1;
allData allData2;

int periodCount = 0;
int MacNum = 0; // amount of unique mac addresses
int Global_send_status = 1;
int countAlternate = 0;
int T = 0; //period to send to node

unsigned long lastTime = 0;
unsigned long secondsOne = 0;
unsigned long lastTime3 = 0;
unsigned long secondsTen = 0;
unsigned long printTime = 0;

bool wifiStart;
bool sensorDataRec;
bool testingDataRec;
bool newPeriod;
bool firstTime = true;
bool broadcasted = false;

//-------------------------Node MCU Address-------------------------//
//NodeMCUs B,D,F are BROKEN
uint8_t broadcastAddressCH[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //
uint8_t broadcastAddressA[] = {0x4C, 0x75, 0x25, 0x35, 0x85, 0x80}; //NodeMCU A id is 1
uint8_t broadcastAddressC[] = {0x4C, 0x75, 0x25, 0x35, 0xB0, 0x11}; //NodeMCU C id is 3
uint8_t broadcastAddressE[] = {0x4C, 0x75, 0x25, 0x36, 0x57, 0x74}; //NodeMCU E id is 5
uint8_t broadcastAddressG[] = {0x58, 0xBF, 0x25, 0xDA, 0x84, 0x08}; //NodeMCU G id is 7
uint8_t broadcastAddressH[] = {0x40, 0x91, 0x51, 0x52, 0xD4, 0xB3}; //NodeMCU H id is 8
uint8_t broadcastAddressI[] = {0x58, 0xBF, 0x25, 0xDA, 0xA9, 0x19}; //NodeMCU I id is 9
uint8_t broadcastAddressJ[] = {0x58, 0xBF, 0x25, 0xD9, 0xE4, 0x13}; //NodeMCU J id is 10



void storeData(int ID3) {
  Serial.println();
  Serial.print("ID: ");
  Serial.println(ID3);

  //Init EEPROM
  EEPROM.begin(4096);
  //Write data into eeprom

  int address = ID3 * 100; //spaces out the addresses enough not to interfere
  //Read data from eeprom
  int readId; //previous count
  EEPROM.get(address, readId);
  Serial.print("Old counter = ");
  Serial.println(readId);
  int counter = readId + 1; //incrementer

  //    EEPROM.put(address, 0);  //used for setting counter back to 0
  EEPROM.put(address, counter); //regular counting
  EEPROM.commit();
  EEPROM.end();

  Serial.print("New Counter: ");
  Serial.println(counter);
  Serial.println();
}


//for the data send function
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  char macStr[18];
  //  Serial.print("Packet to:");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  //  Serial.print(macStr);
  //  Serial.print(" send status: ");

  Global_send_status = sendStatus;
  if (sendStatus == 0) {
    Serial.println("Period Delivery success");
    newPeriod = false;
    //        Global_send_status = 1;
  }
  else {
    Serial.println("Period Delivery fail");
  }
}





//for the Data Receive function
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  Serial.println();
  Serial.print("bytes: ");
  Serial.println(len);


  if (len == 4) {
    char macStr[18];
    Serial.print("Broadcast received from: CH");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);

    if (millis() - lastTime3 > 0) {
      for ( int z = 0; z < 6; ++z ) {
        broadcastAddressCH[z] = mac_addr[z], HEX;
        //        Serial.println();
        //        Serial.println(MacList[MacNum][z]), HEX;
        //        Serial.println(mac_addr[z]), HEX;
      }
    }
    lastTime3 = millis();
    double Test = T;
    broadcasted = true;
    esp_now_send(broadcastAddressCH, (uint8_t *) &Test, sizeof(Test));
  }



  if (len == 96) {
    secondsTen = millis();
    sensorDataRec = true;
    char macStr[18];
    Serial.print("Sensor Data received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    memcpy(&allData1, incomingData, sizeof(allData1));
    storeData(allData1.windSpeed1); //stores counter for sensor node type
  }




  if (len == 80) {
    wifiStart = true;
    testingDataRec = true;
    secondsTen = millis();


    char macStr[18];
    Serial.print("Testing Data received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    memcpy(&testingData1, incomingData, sizeof(testingData1));

    Serial.println();
    Serial.print("Total join requests received from DS: ");
    Serial.print(testingData1.totalJoinDSAve1); Serial.println("%");

    Serial.print("Total join requests received from present SNs: ");
    Serial.print(testingData1.totalJoinSNAve1); Serial.println("%");

    Serial.print("Current schedules sent successfully: ");
    Serial.print(testingData1.scheduleAve1); Serial.println("%");

    Serial.print("Total schedules sent successfully: ");
    Serial.print(testingData1.TotalScheduleAve1); Serial.println("%");

    Serial.print("Current aggregated data received at DS: ");
    Serial.print(testingData1.aveDS1); Serial.println("%");

    Serial.print("Total aggregated data received at DS: ");
    Serial.print(testingData1.totalAveDS1); Serial.println("%");

    Serial.print("Current data received from clustered SNs: ");
    Serial.print(testingData1.aveSN1); Serial.println("%");

    Serial.print("Total data received from clustered SNs: ");
    Serial.print(testingData1.totalAveSN1); Serial.println("%");


    Serial.print("Number of clustered Sensor Nodes ");
    Serial.print(testingData1.MacNum1); Serial.println();

    Serial.print("Current Period Count: ");
    Serial.print(testingData1.periodCount1); Serial.println();

    Serial.print("Current Period: ");
    Serial.print(testingData1.currentPeriod1); Serial.println(" seconds");
    Serial.println();
  }
}

//-------------------------Web Server Start-------------------------//
//to store the Sensor ID input
const char* PARAM_INPUT_ID = "inputID";
const char* PARAM_INPUT_Period = "inputPeriod";
int id = 1;

// Replace with your network credentials
const char* ssid = "Danville_HiFi";
const char* password = "1BigHappyFamily";

//const char* ssid = "UCSC-Guest";
//const char* ssid = "ApartmentA";
//const char* password = "MilkmansRevenge";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
//-------------------------Web Server End-------------------------//

String sendWindSpeed() {
  return String(allData1.windSpeed1);
}
String sendWindDir() {
  return String(allData1.windDir1);
}
String sendTempC() {
  return String(allData1.tempC1);
}
String sendCapRead() {
  return String(allData1.capRead1);
}
String sendTemp() {
  return String(allData1.t1);
}
String sendHumidity() {
  return String(allData1.h1);
}


String sendA() {
  return String(testingData1.totalJoinDSAve1);
}
String sendB() {
  return String(testingData1.totalJoinSNAve1);
}
String sendC() {
  return String(testingData1.scheduleAve1);
}
String sendD() {
  return String(testingData1.TotalScheduleAve1);
}
String sendE() {
  return String(testingData1.aveDS1);
}
String sendF() {
  return String(testingData1.totalAveDS1);
}
String sendG() {
  return String(testingData1.aveSN1);
}
String sendH() {
  return String(testingData1.totalAveSN1);
}
String sendI() {
  return String(testingData1.MacNum1);
}
String sendJ() {
  return String(testingData1.periodCount1);
}
String sendK() {
  return String(testingData1.currentPeriod1);
}











const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>SENSOR DATA</h2>
  <form action="/get">
    Sensor ID: <input type="number" name="inputID" min="1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    Sensor Period: <input type="number" name="inputPeriod" min="1">
    <input type="submit" value="Submit">
  </form>
  <p>
    <i class="fas fa-wind"></i> 
    <span class="dht-labels">Wind Speed</span> 
    <span id="windSpeed">%WINDSPEED%</span>
    <sup class="units">km/hr</sup>
  </p>
  <p>
    <i class="fas fa-compass"></i> 
    <span class="dht-labels">Wind Direction</span>
    <span id="windDirection">%WINDDIRECTION%</span>
    <sup class="units">&deg;</sup>
  </p>
  <p>
    <i class="fas fa-thermometer"></i> 
    <span class="dht-labels">Soil Temperature</span>
    <span id="soilTemp">%SOILTEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint"></i> 
    <span class="dht-labels">Soil Moisture</span>
    <span id="soilHumidity">%SOILHUMIDITY%</span>
  </p>
  <p>
    <i class="fas fa-thermometer"></i> 
    <span class="dht-labels">Ambient Temperature</span>
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint"></i> 
    <span class="dht-labels">Relative Ambient Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>



  <h2>TESTING DATA</h2>
  <p>
    <span class="dht-labels">Number of paired Sensor Nodes</span> 
    <span id="percI">%PERCI%</span>
  </p>
  <p>
    <span class="dht-labels">Period Count</span> 
    <span id="percJ">%PERCJ%</span>
  </p>
  <p>
    <span class="dht-labels">Current Period</span> 
    <span id="percK">%PERCK%</span>
    <span class="dht-labels">seconds</span> 
  </p>
  
  <p>
    <span class="dht-labels">Total join reqs from DS</span> 
    <span id="percA">%PERCA%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Total join reqs from SNs</span> 
    <span id="percB">%PERCB%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Current schedules sent</span> 
    <span id="percC">%PERCC%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Total schedules sent</span> 
    <span id="percD">%PERCD%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Current Data received at DS</span> 
    <span id="percE">%PERCE%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Total Data received at DS</span> 
    <span id="percF">%PERCF%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Current Data received from SNs</span> 
    <span id="percG">%PERCG%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <span class="dht-labels">Total Data received from SNs</span> 
    <span id="percH">%PERCH%</span>
    <sup class="units">&percnt;</sup>
  </p>
</body>






<script>
//Wind Speed
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("windSpeed").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/windSpeed", true);
  xhttp.send();
}, 1000 ) ;

//wind Direction
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("windDirection").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/windDirection", true);
  xhttp.send();
}, 1000 ) ;

//Soil Temperature
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("soilTemp").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/soilTemp", true);
  xhttp.send();
}, 1000 ) ;

//Soil Humidity
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("soilHumidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/soilHumidity", true);
  xhttp.send();
}, 1000 ) ;

//Temperature
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 1000 ) ;

//Humidity
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 1000 ) ;


//percI
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percI").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percI", true);
  xhttp.send();
}, 1000 ) ;

//percJ
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percJ").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percJ", true);
  xhttp.send();
}, 1000 ) ;

//percK
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percK").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percK", true);
  xhttp.send();
}, 1000 ) ;






//percA
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percA").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percA", true);
  xhttp.send();
}, 1000 ) ;

//percB
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percB").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percB", true);
  xhttp.send();
}, 1000 ) ;

//percC
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percC").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percC", true);
  xhttp.send();
}, 1000 ) ;

//percD
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percD").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percD", true);
  xhttp.send();
}, 1000 ) ;

//percE
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percE").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percE", true);
  xhttp.send();
}, 1000 ) ;

//percF
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percF").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percF", true);
  xhttp.send();
}, 1000 ) ;

//percG
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percG").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percG", true);
  xhttp.send();
}, 1000 ) ;

//percH
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("percH").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/percH", true);
  xhttp.send();
}, 1000 ) ;









</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String & var)
{
  // Serial.println(var);
  if (var == "WINDSPEED")
  {
    return sendWindSpeed();
  }
  else if (var == "WINDDIRECTION")
  {
    return sendWindDir();
  }
  else if (var == "SOILTEMPERATURE")
  {
    return sendTempC();
  }
  else if (var == "SOILHUMIDITY")
  {
    return sendCapRead();
  }
  else if (var == "TEMPERATURE")
  {
    return sendTemp();
  }
  else if (var == "HUMIDITY")
  {
    return sendHumidity();
  }


  else if (var == "PERCA")
  {
    return sendA();
  }
  else if (var == "PERCB")
  {
    return sendB();
  }
  else if (var == "PERCC")
  {
    return sendC();
  }
  else if (var == "PERCD")
  {
    return sendD();
  }
  else if (var == "PERCE")
  {
    return sendE();
  }
  else if (var == "PERCF")
  {
    return sendF();
  }
  else if (var == "PERCG")
  {
    return sendG();
  }
  else if (var == "PERCH")
  {
    return sendH();
  }
  else if (var == "PERCI")
  {
    return sendI();
  }
  else if (var == "PERCJ")
  {
    return sendJ();
  }
  else if (var == "PERCK")
  {
    return sendK();
  }
  return String();
}











void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/html", index_html, processor);
  });


  // Getting the ID input for the sensor needed to be displayed
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage;
    String inputParam;
    // GET ID value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_ID)) {
      inputMessage = request->getParam(PARAM_INPUT_ID)->value();
      inputParam = PARAM_INPUT_ID;
      id = inputMessage.toInt();
    }
    else if (request->hasParam(PARAM_INPUT_Period)) {
      inputMessage = request->getParam(PARAM_INPUT_Period)->value();
      inputParam = PARAM_INPUT_Period;
      T = inputMessage.toInt();
      //------------------------------------------------------------------------------------
      //------------------------------------------------------------

      newPeriod = 1;
      Global_send_status == 1;
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    //Serial.println(id);
    //Serial.println(T);
    request->send(200, "text/html", index_html);
  });

  // Sensors calling function
  server.on("/windSpeed", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendWindSpeed().c_str());
  });
  server.on("/windDirection", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendWindDir().c_str());
  });
  server.on("/soilTemp", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendTempC().c_str());
  });
  server.on("/soilHumidity", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendCapRead().c_str());
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendTemp().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendHumidity().c_str());
  });


  server.on("/percA", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendA().c_str());
  });
  server.on("/percB", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendB().c_str());
  });
  server.on("/percC", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendC().c_str());
  });
  server.on("/percD", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendD().c_str());
  });
  server.on("/percE", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendE().c_str());
  });
  server.on("/percF", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendF().c_str());
  });
  server.on("/percG", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendG().c_str());
  });
  server.on("/percH", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendH().c_str());
  });

  server.on("/percI", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendI().c_str());
  });
  server.on("/percJ", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendJ().c_str());
  });
  server.on("/percK", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send_P(200, "text/plain", sendK().c_str());
  });

  // Start server
  server.begin();

  //  newPeriod = true;
}




void loop() {
  if (millis() - secondsOne > 1000) {
    if (wifiStart) {
      wifiStart = false;
      WiFi.begin(ssid, password);
      Serial.println("WiFi begin");
    }
    Serial.print("WiFi status: ");
    if (WiFi.status() == 0) {
      Serial.println("WiFi not connected");
    } else if (WiFi.status() == 3) {
      Serial.println("WiFi connected");
    } else
      Serial.println("WiFi connecting..." );
      
    secondsOne = millis();

    Serial.println(millis() - secondsTen);
    //    Serial.println(testingDataRec);
    Serial.println();
  }

  //waits 20 seconds after data is rec from CH to start WiFi
  if ((millis() - secondsTen > 20000) && testingDataRec) {
    Serial.println(WiFi.localIP());
    Serial.println("Resetting");
    sensorDataRec = false;
    testingDataRec = false;
    wifiStart = false;

    //none of these work so must sleep
    WiFi.disconnect(true);
    //    WiFi.shutdown();
    WiFi.forceSleepBegin(true);
    WiFi.mode(WIFI_OFF);

    ESP.deepSleep(1e6);


    secondsTen = millis();
    //    WiFi.begin(ssid, password);
  }
}
