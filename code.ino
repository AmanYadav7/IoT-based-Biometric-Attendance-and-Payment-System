#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>

#define led1 D4 //RED
#define led2 D5 //GREEN
#define led3 D6 //YELLOW
#define led4 D7 //WHITE 


SoftwareSerial mySerial(D1, D2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const char* ssid     = "";   //replace with your own SSID
const char* password = "";    //replace with your own password
const char* host = "api.pushingbox.com"; 

boolean loopaction = true;
String member = "";
int member2;
int flag = 0;
uint8_t response; //to ask user's action.
uint8_t id;

/***** Prototype Declaration ******/

void scanfinger(); //modified program for scanning finger of user
void deletefinger();

/***** Prototype Declaration End ******/



/*****Class Declaration ******/
class Employee
{
  public:
    int UniqueID;
    int State;
    String Name;
};
/*****Class Declaration ******/

Employee emp[128];


/*****Function To Get Finger Id Using Finger Print ******/

int getFingerprintID()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

/****Function To Get Finger Id Using Finger Print End****/

/*****Function For Attendance******/

// Dependent Over getFingerprintID()

void attendance()
{
  int fingerprintID = -1;
  while(fingerprintID == -1)
  {
    fingerprintID = getFingerprintID();
    Serial.println(".");
  } 
  if(fingerprintID != -1)
  {
    Serial.println("Captured !");
    Serial.println("Your ID is:" + fingerprintID);
    if(emp[fingerprintID].State==0)
     {
        digitalWrite(led4, HIGH);
        emp[fingerprintID].State = 1;
        connecthost(emp[fingerprintID].Name, emp[fingerprintID].UniqueID, emp[fingerprintID].State);
        digitalWrite(led4, LOW);
     }
  
     else if(emp[fingerprintID].State==1)
     {
        digitalWrite(led4, HIGH);
        emp[fingerprintID].State = 0;
        connecthost(emp[fingerprintID].Name, emp[fingerprintID].UniqueID, emp[fingerprintID].State);
        digitalWrite(led4, LOW);
     }
  }

}

/******Function For Attandance End*******/



/**** Function To SEND Employee Data Using Pushingbox API ******/

void connecthost(String data, int uniqueid, int state)
{
  
  if (flag == 0)
  {
    digitalWrite(led3, HIGH);
    member = data;
    member2 = uniqueid;
    Serial.print("connecting to ");
    Serial.println(host);
    WiFiClient client;
    const int httpPort = 80;
    
    if (!client.connect(host, httpPort)) 
    {
      digitalWrite(led1, HIGH);
      Serial.println("connection failed");
      delay(1000);
      digitalWrite(led1, LOW);
      return;
    }

    String url = "/pushingbox?";
    url += "devid=";
    url += "";
    url += "&name=" + String(member) + "&id=" + String(member2) + "&status=" + String(state);
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) 
    {
        if (millis() - timeout > 5000) 
        {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
    }
    
    while (client.available())
    {
      digitalWrite(led2, HIGH);
      String line = client.readStringUntil('\r');
      Serial.print(line);
      Serial.print("Data Sent!");
      digitalWrite(led2, LOW);
      delay(1000);
    }
    digitalWrite(led3, LOW);
    Serial.println();
    Serial.println("closing connection");
  }
}

/*****Function To SEND Employee Data Using Pushingbox API End ******/

/***** Function To get Unique ID *******/

int getUniqueID()
{
    int i = 0;

    while(i == 0)
    {
      while(!Serial.available());
      i = Serial.parseInt();
    }
    return i;
}

/***** Function To get Unique ID End ******/

/***** Function To get Name End ******/

void getName()
{
  String namee = "";

  while(namee=="")
  {
    while(Serial.available()==0);
    namee = Serial.readString();
  }

  emp[id].Name = namee;
  //return namee;    
}

/***** Function To get Name End ******/

/***** Function To Read a Number *******/

uint8_t readnumber(void) 
  {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
  }

/***** Function To Read a Number End ******/

/***** Function to capture Fingerprint and Save it !*****/

  uint8_t getFingerprintEnroll()
  {
  
    digitalWrite(led4, HIGH);
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
    
    while (p != FINGERPRINT_OK) 
    {
      p = finger.getImage();
      switch (p) 
      {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.println(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    }
    // OK success!
  
    p = finger.image2Tz(1);
    switch (p) 
    {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    
    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    
    while (p != FINGERPRINT_NOFINGER) 
    {
      p = finger.getImage();
    }
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK)
    {
      p = finger.getImage();
      switch (p) 
      {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    }
  
    // OK success!
  
    p = finger.image2Tz(2);
    switch (p)
    {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
    
    // OK converted!
    Serial.print("Creating model for #");  Serial.println(id);
    
    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
      digitalWrite(led2, HIGH);
      Serial.println("Prints matched!");
      delay(500);
      digitalWrite(led2, LOW);
      
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
      digitalWrite(led1, HIGH);
      Serial.println("Communication error");
      delay(500);
      digitalWrite(led1, LOW);
      return p;
    } 
    else if (p == FINGERPRINT_ENROLLMISMATCH) 
    {
      digitalWrite(led1, HIGH);
      Serial.println("Fingerprints did not match");
      delay(500);
      digitalWrite(led1, LOW);
      return p;
    } 
    else
    {
      Serial.println("Unknown error");
      return p;
    }   
    Serial.print("ID "); Serial.println(id);
    p = finger.storeModel(id);                      //This will store that finger model with that id
    if (p == FINGERPRINT_OK) 
    {
      digitalWrite(led2, HIGH);
      Serial.println("Stored!");
      delay(1000);
      digitalWrite(led2, LOW);
      
      Serial.println("Enter Unique ID number for figerprint ID " + String(id)+":"); 
      emp[id].UniqueID = getUniqueID(); //Enter Aadhar ID or UniqueID
      Serial.println("Your unique ID is: "); Serial.print(emp[id].UniqueID);
      Serial.println("");
  
      int encid = 1000 + int(id);
      emp[id].Name = "MGNREGA"+ String(encid);
      emp[id].State = 0;
      connecthost(emp[id].Name, emp[id].UniqueID, emp[id].State);
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR) 
    {
      digitalWrite(led1, HIGH);
      Serial.println("Communication error");
      delay(1000);
      digitalWrite(led1, LOW);
      return p;
    }
    else if (p == FINGERPRINT_BADLOCATION) 
    {
      Serial.println("Could not store in that location");
      return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
      digitalWrite(led1, HIGH);
      Serial.println("Error writing to flash");
      delay(500);
      digitalWrite(led1, LOW);
      return p;
    }
    else
    {
      digitalWrite(led1, HIGH); 
      Serial.println("Unknown error");
      return p;
      delay(1000);
      digitalWrite(led1, LOW); 
    }
    digitalWrite(led4, LOW); 
  }

/***** Function to capture Fingerprint and Save it ! End *******/
  
/***** Function CODE FOR ENROLLMETNT OF FINGERPRINT******/

void scanfinger()
{
  digitalWrite(led3, HIGH);

  //Serial.println("Welcome to Fingerprint enrolement.");
  uint8_t action = 1;
  
  while(action == 1)
  {
    //retry:
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    
    id = readnumber();
    
    if (id == 0) 
    {
       //ID #0 not allowed, try again!
       //goto retry;
       Serial.println("Id 0 is not Allowed");
       return;
    }
    
    Serial.print("Enrolling ID #");
    Serial.println(id);
    
    
    getFingerprintEnroll();
    Serial.println("To enroll a new finger press 1, and to exit press 2 ");
    digitalWrite(led1, LOW);
    action = readnumber();
  }
 digitalWrite(led3, LOW);
} 

/***** Function CODE FOR ENROLLMETNT OF FINGERPRINT End*****/




/****** Function to Delete Fingerprint Adafruit ********/

uint8_t deleteFingerprint(uint8_t id)
  {
    uint8_t p = -1;
  
    p = finger.deleteModel(id);
  
    if (p == FINGERPRINT_OK)
    {
      Serial.println("Deleted!");
    } 
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
      Serial.println("Communication error");
      return p;
    } 
    else if (p == FINGERPRINT_BADLOCATION)
    {
      Serial.println("Could not delete in that location");
      return p;
    } 
    else if (p == FINGERPRINT_FLASHERR) 
    {
      Serial.println("Error writing to flash");
      return p;
    } 
    else 
    {
      Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
      return p;
    }
}
/****** Function to Delete Fingerprint Adafruit End *******/

/****** Function to Delete Fingerprint********/

void deletefinger()
{
  digitalWrite(led3, HIGH);

  Serial.println("Welcome to user deltetion portal.");
  uint8_t action = 1;
  
  while(action == 1)
   {
      //retry:
      Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
      
      id = readnumber();
      
      if (id == 0) 
      {// ID #0 not allowed, try again!
         //goto retry;
         return;
      }
      Serial.print("Enrolling ID #");
      Serial.println(id);
      
      deleteFingerprint(id);
      
      Serial.println("To delete another user press 1, and to exit to attendace portal press 2 ");
      digitalWrite(led1, LOW);
      action = readnumber();
      
   }
   digitalWrite(led3, LOW);
 
}

/****** Function to Delete Fingerprint End ********/




void setup()
{
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();
  
  digitalWrite(led4, HIGH);
  Serial.print("Connecting to WiFi...       ");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led4, LOW);

  digitalWrite(led2, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1500);
  digitalWrite(led2, LOW);

  while (!Serial);
  
  digitalWrite(led4, HIGH);
  delay(100);
    
  Serial.println("\n\n Waiting for Fingerprint Sensor");

  delay(1500);
  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Found Successfully");
    digitalWrite(led2, HIGH);

    delay(1500);
    digitalWrite(led2, LOW);
  }
  else
  {
    Serial.println("Fingerprint sensor not found!!!");

      digitalWrite(led1, HIGH);
    while (1)
    {
      delay(1);
    }
  }
  digitalWrite(led4, LOW);

  digitalWrite(led3, HIGH);
}

void loop()
{
  if(loopaction==true)
  {  
    int choice;
    do
    {
      digitalWrite(led3, HIGH);
      Serial.println(">Welcome to MGNREGA Attendance Monitoring Wizard !");
      Serial.println(">Enter Your Chooice(0 to exit ):");
      Serial.println("1.Enroll");
      Serial.println("2.Attendance");
      Serial.println("3.Leave Job");
      choice = readnumber();
      digitalWrite(led3, LOW);
      switch(choice)
      {
        case 1:
          scanfinger();
          break;
        case 2:
          attendance();
          break;
        case 3:
          deletefinger(); 
          break;
        default:
          digitalWrite(led1, HIGH);
          delay(1000);
          Serial.println("Wrong Choice");
          digitalWrite(led1, LOW);       
      }
      digitalWrite(led3, HIGH);
      Serial.println(">Press 0 to Exit From the MGNREGA Portal(Non-0 to continue)");
      choice = readnumber();
      digitalWrite(led3, LOW);
    }while(choice !=0 );
  
  }
}
