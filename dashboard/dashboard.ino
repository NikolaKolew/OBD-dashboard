#include <WiFi.h>
#include "ELMduino.h"
#include <LiquidCrystal_I2C.h> 
#include <RBD_Timer.h>
#include <RBD_Button.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const char* ssid = "WiFi_OBDII";


//IP Adress of your ELM327 Dongle
IPAddress server(192, 168, 0, 10);
WiFiClient client;
ELM327 myELM327;

int rpm_gauge;    // value for the RPM gauge
int voltage_gauge;    // value for the VOLTAGE gauge
int eng_temp;
//float engineCoolantTemp();

uint32_t rpm = 0;
uint32_t vol = 0;
uint32_t cool = 0;
char buffer[40];

RBD::Button button(4);


//int button_select_clicked = 0; //only perform action when button is clicked, and wait until another press
int current_screen = 0; 


void setup() {

  rpm_gauge = 600;  // set "random" value for rpm gauge
  voltage_gauge = 12;  // set "random" value for voltage gauge
  eng_temp = 20; // set "random" value for Coolant temp gauge

  lcd.init();
  lcd.backlight();
  lcd.print("starting");
  Serial.begin(115200);
  Serial.print("Connecting test...");
  Serial.println(ssid);
 

  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("Trying to connect");
  }

  Serial.println("");
  Serial.println("Connected to Wifi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (client.connect(server, 35000))
    Serial.println("connected");
  else
  {
    Serial.println("connection failed");
    while(1);
  }

  myELM327.begin(client, true, 2000);
  
}
  


void loop() {

  if(button.onPressed()) {
    if (current_screen == 0) {
      current_screen = 1;
      lcd.clear();
    }
    else if (current_screen == 1) {
      current_screen = 2;
      lcd.clear();
    }
    else {
      current_screen = 0;
      lcd.clear();
    }
  }
  
  if (current_screen == 0) { // MENU SCREEN
    
    int tempRPM = myELM327.rpm();
    
    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
      rpm = (uint32_t)tempRPM;
      lcd.print("RPM: "); lcd.println(rpm);
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      myELM327.printError();

    lcd.setCursor(0,0);                         // move cursor to top left
    sprintf(buffer, "RPM: %4d%", rpm);    //
    lcd.print(buffer);  
    
  }

  else if (current_screen == 1) { // SCREENSHOTS SCREEN
    int voltage = myELM327.batteryVoltage();

    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
      vol = (uint32_t)voltage;
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      myELM327.printError();
      lcd.setCursor(0,1);                         // move cursor to top left
      sprintf(buffer, "Voltage:%2d%", vol);    //
      lcd.print(buffer); 
    
  }

  else if (current_screen == 2) { // SCREENSHOTS SCREEN
    int coolant = myELM327.engineCoolantTemp();

    if (myELM327.nb_rx_state == ELM_SUCCESS)
    {
      cool = (uint32_t)coolant;
    }
    else if (myELM327.nb_rx_state != ELM_GETTING_MSG)
      myELM327.printError();
      lcd.setCursor(0,1);                         // move cursor to top left
      sprintf(buffer, "Water:%2d%", cool);    //
      lcd.print(buffer); 
     
    
  }
  
}
