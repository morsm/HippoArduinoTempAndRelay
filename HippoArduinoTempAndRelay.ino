#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"

Adafruit_AM2320 am2320 = Adafruit_AM2320();

char COMMAND = '_';


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // hang out until serial port opens
  }

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  
  Serial.println("AM2320 Query and 4x relay app");
  am2320.begin();
}


void loop()
{
  int serRead = Serial.read();
  if (serRead != -1) handleSerialByte(serRead);
}

void handleSerialByte(int serRead)
{
  if (serRead == '_')
  {
    COMMAND = '_'; // reset
  }
  else if (COMMAND == '?')
  {
    handleQuery(serRead);
    COMMAND = '_'; // reset
  }
  else if (COMMAND == 'O')        // O for open
  {
    handleRelayOff(serRead);
    COMMAND = '_'; // reset
  }
  else if (COMMAND == 'C')        // C for closed
  {
    handleRelayOn(serRead);
    COMMAND = '_'; // reset
  }
  else COMMAND = (char) serRead;  // New command
}

void handleRelayOn(int serRead)
{
  handleRelay(serRead, HIGH);
}

void handleRelayOff(int serRead)
{
  handleRelay(serRead, LOW);
}

void handleRelay(int serRead, int hiLo)
{
  // Should be ASCII value between 49 ('1') and 52 ('4')
  if (serRead < 49 || serRead > 52)
  {
    Serial.write("_ERR_");
    return;  
  }

  int pin = serRead - 47;   // Relay 1, given by ASCII 49, is digital pin 2
  digitalWrite(pin, hiLo);

  Serial.write("_OK_");
}


void handleQuery(int serRead)
{
  String response = "ERR";
  
  switch ((char) serRead)
  {
    case 'T':
      response = String(getTemp());
      break;

    case 'H':
      response = String(getHumidity());
      break;

    case '1':
    case '2':
    case '3':
    case '4':
      response = String(getRelayStatus(serRead - 47));
      break;
  }

  Serial.write("_");  // SOH
  Serial.write(response.c_str());
  Serial.write("_"); // EOT
}

int getRelayStatus(int pin)
{
  return digitalRead(pin);
}

float getTemp()
{
  return am2320.readTemperature();
}

float getHumidity()
{
  return am2320.readHumidity();
}
