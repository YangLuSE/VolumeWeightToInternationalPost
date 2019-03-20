#include "WiFiEsp.h"


#include <Key.h>
#include <Keypad.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>



// Emulate Serial1 on pins 10/11 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

char ssid[] = "ip7lu";            // your network SSID (name)
char pass[] = "0765571966";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
// Declare and initialise global variables/arrays for Thingspeak connection
const char server[] = "thingspeak.com";
const char thingspeakAPIKey[] = "E1FS6HZCX5LV90C2";
long postingInterval = 3000;
float data=0.0;
// Declare global variable for timing
long lastConnectionTime;

// Declare and initialise data variable
long myData = 0;

// Create client object
WiFiEspClient client;

const byte ROWS = 4; // declare the rows of the keypad
const byte COLS = 3; // declare the columns of the keypad
const String VER = "V1.02";
LiquidCrystal_I2C lcd(0x3F,16,2);
String keyin="";

String value[3]="   ";

// Define the Keymap
char keys[ROWS][COLS] = 
{
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};

byte rowPins[ROWS] = {3,4,5,6}; // Connect keypad’s pins to these Arduino pins.
byte colPins[COLS] = {7,8,9};   // Connect keypad’s pins to these Arduino pins.

// Create the KeyPad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void keypadEvent(KeypadEvent eKey)
{
    // When the KeyPad will be pressed, the results will appear on the serial monitor.
    switch (keypad.getState())
    {
    case PRESSED:

        lcd.clear();
        keyin+=eKey;
        if(eKey=='*'){
          if(keyin=="*"){keyin="";}
          keyin=keyin.substring(0,keyin.length()-2);
        }
        if(eKey=='#'){
          keyin=keyin.substring(0,keyin.length()-1);
          for(int i=1; i<=keyin.length()%3;i++)
          {
            keyin+=" ";
          }
        }
        if(keyin.length()>9 && eKey=='0'){
          keyin="";
        }
        delay(10);
          value[0]=keyin.substring(0,3);
        if(keyin.length()>3){
          value[1]=keyin.substring(3,6);
        }else{value[1]="   ";}
        if(keyin.length()>6){
          value[2]=keyin.substring(6,9);
        }else{value[2]="   ";}
        
        lcd.setCursor(0,0);
        lcd.print("X:"+value[0]);
        lcd.setCursor(5,0);
        lcd.print("Y:"+value[1]);
        lcd.setCursor(10,0);
        lcd.print("Z:"+value[2]);
        lcd.setCursor(0,1);
        lcd.print("VW:"+String(value[0].toFloat() * value[1].toFloat() * value[2].toFloat()/1000000*280)+"kg");
        data=value[0].toFloat() * value[1].toFloat() * value[2].toFloat()/1000000*280;
        
        Serial.println(data);
        
        if(keyin.length()>=9 && eKey=='#'){
          lcd.setCursor(0,0);
          
          sendThingspeak(data);
          lcd.print("OK.                 ");
 
        }
        
        
        Serial.print("Enter: ");
        Serial.println(eKey);
        Serial.println(keyin);
        
        break;
    }
}


void setup()
{
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent);
  lcd.init();  //initialize the lcd
    lcd.backlight();  //open the backlight 
    lcd.setCursor(0,0);
    lcd.print("VolWeight  "+VER);
    lcd.setCursor(0,1);
    Serial1.begin(9600);
    

  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");





  // Print the SSID of the network
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

    lcd.print("Ready.");


}

void loop() {


    keypad.getKey();
    delay(20);

    
}


void sendThingspeak(float value) {
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println("GET /update?api_key=" + String(thingspeakAPIKey) +
                   "&field1=" + String(value) + " HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
    Serial.print("We have sent to the server : ");
    Serial.println(value);
    Serial.println();
    client.flush();
   client.stop();
  }
}
