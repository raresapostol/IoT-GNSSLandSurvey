

#include "hardware.h"

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig conf;
bool signupOK = false;

// Button Pin
const int interruptPin = 0;
const int nextArrayPin = 2;

bool isButtonPressed = false;
bool isNextArrayPressed = false;
int count = 0;

double lat = 0;
double lon = 0;

char *strings[6]; 
char *ptr = NULL;

// Send data to remote
int count1 = 0;

NMEA_t NMEA;

char read_char(){
  while(!Serial.available());
  return Serial.read();
}

void receive_string(){
  int i = 0;
  
  char c = read_char();
  while(c != end_char){
    if(i >= max_string_length)
       break;
       
    inData[i] = c; 
    i++;
    c = read_char();
  }
  inData[i] = '\0';
  updateLatLong();
}

inline float nmea_convert(float raw_degrees)
{
  int firstdigits = ((int)raw_degrees) / 100;
  float nexttwodigits = raw_degrees - (float)(firstdigits * 100.0f);
  return (float)(firstdigits + nexttwodigits / 60.0f);
}

void updateLatLong(void) {

  	bool end = false;

		// Parse GPGGA messages
    //Serial.println(inData);
			char *token = strtok(inData, ",");
			uint8_t index = 2;

			while (token != NULL) {
        //Serial.print("Token: ");
        //Serial.println(token);

				token = strtok(NULL, ",");
				switch (index) {
				case 2:
					NMEA.gnss.time_h = ((token[0] - 48) * 10) + (token[1] - 48);
					NMEA.gnss.time_m = ((token[2] - 48) * 10) + (token[3] - 48);
					NMEA.gnss.time_s = ((token[4] - 48) * 10) + (token[5] - 48);
					NMEA.gnss.time_ss = ((token[7] - 48) * 10) + (token[8] - 48);
					NMEA.gnss.valid.time = 1;

					break;

				case 3:
					NMEA.gnss.latitude_tmp = (float)atof(token);
					break;

				case 4:
					if (strcmp(token, "S") == 0) {
						NMEA.gnss.latitude_deg = -nmea_convert(NMEA.gnss.latitude_tmp);
            Serial.println(NMEA.gnss.latitude_deg);

						NMEA.gnss.valid.latitude = 1;
					}
				else if (strcmp(token, "N") == 0) {
						NMEA.gnss.latitude_deg = nmea_convert(NMEA.gnss.latitude_tmp);
						NMEA.gnss.valid.latitude = 1;
					}
					break;

				case 5:
					NMEA.gnss.longitude_tmp = (float)atof(token);
					break;

				case 6:
					if (strcmp(token, "W") == 0) {
						NMEA.gnss.longitude_deg = -nmea_convert(NMEA.gnss.longitude_tmp);
						NMEA.gnss.valid.longitude = 1;
					}
					else if (strcmp(token, "E") == 0) {
						NMEA.gnss.longitude_deg = nmea_convert(NMEA.gnss.longitude_tmp);
						NMEA.gnss.valid.longitude = 1;
					}
					break;

				default:
					end = true;
					break;
				}

				index++;
				if (end)
					break;
			}

		end = false;
	
}

void setup() {
  // Initialize UART
  Serial.begin(9600);

  // Delete later
  EEPROM.write(0, 0);
  EEPROM.write(1, 0);

  // Initialize Wifi
  Serial.print("Connecting to ");
  Serial.println(_SSID); 
  WiFi.begin(_SSID, _PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP()); 

  // Configure Firebase
  // Assign the API Key
  conf.api_key = API_KEY;

  // Assign the RTDB URL
  conf.database_url = DATABASE_URL;

  // Sign up
  if (Firebase.signUp(&conf, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  } else{
    Serial.println("nui ok gion");
    Serial.printf("%s\n", conf.signer.signupError.message.c_str());
  }

   /* Assign the callback function for the long running token generation task */
  conf.token_status_callback = tokenStatusCallback;

  Firebase.begin(&conf, &auth);
//  Firebase.reconnectWiFi(true);

  // EEPROM READ
  count = EEPROM.read(0);
  count1 = EEPROM.read(1);

//  // Configure button input
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptFunction, FALLING);
  attachInterrupt(digitalPinToInterrupt(nextArrayPin), nextArrayHandle, FALLING);
  

  Serial.println("Setup finished");
  
}

void loop() {
  receive_string();

  // Firebase Error Handling ************************************************
  if (!Firebase.ready()) {
    delay(500);
    Firebase.begin(&conf, &auth);
    Serial.println("Connection to firebase failed. Reconnecting...");
    delay(500); 
  } else {
    if (isButtonPressed) {
      Serial.println(isButtonPressed);
      Serial.print("Lat: ");
      Serial.println(NMEA.gnss.latitude_deg);
      Serial.print("Long: ");
      Serial.println(NMEA.gnss.longitude_deg);
      Firebase.RTDB.setDouble(&fbdo, "location1/perimeter/geometry/coordinates/" + String(count1) + "/" + String(count) + "/0/", NMEA.gnss.longitude_deg);  
      Firebase.RTDB.setDouble(&fbdo, "location1/perimeter/geometry/coordinates/" + String(count1) + "/" + String(count) + "/1/", NMEA.gnss.latitude_deg);  

      count++;
      EEPROM.write(0, count);
      isButtonPressed = false;
    }
  }

  if (isNextArrayPressed) {
    count1++;
    count = 0;

    EEPROM.write(0, count);
    EEPROM.write(1, count1);
    isNextArrayPressed = false;
  }
}

ICACHE_RAM_ATTR void nextArrayHandle() {
  isNextArrayPressed = true;
}


ICACHE_RAM_ATTR void interruptFunction() {
  isButtonPressed = true; 
}