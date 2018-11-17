/*-----------------------------------------------------------------------------
 **                        � Arturo Jumpa
 ** File: Esp8266DynamoDB.ino
 **
 ** Description:
 ** This code uses an ESP8266 to push and poll data from an Amazon DynamoDB table and uses
 ** that data to update values used for dimming. To succesfully implement AC dimming on this 
 ** processor we need to keep timing with microsecond accuracy, something which isnt natively
 ** supported. To get around this issue, I use some internal registers to time the interrupts
 ** and place those ISRs in RAM for faster execution. Additional hardware is also needed to
 ** perform AC dimming. See details here: <URL>
-------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "Esp8266AWSImplementations.h"
#include "AmazonDynamoDBClient.h"
#include "AWSFoundationalTypes.h"
#include "keys.h"          // Values obtained from IAM console. See readme for details
#include "switch.h"         // structure for holding switch info
//TODO: Add header for function prototypes

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#define DEBUG         1
#define DB_GET_TIME     5     // time (in sec) to poll DB for data 
#define HARD_ISR_PIN    13    // for zero cross detection

#define TIM_DIV1        0   // 80MHz (80 ticks/us - 104857.588 us max). Default ESP8266 clock rate
#define TIM_DIV16       1   // 5MHz (5 ticks/us - 1677721.4 us max)
#define TIM_DIV256      3   // 312.5Khz (1 tick = 3.2us - 26843542.4 us max)
#define TIM_EDGE        0   // TIM_LEVEL not supported 
#define TIM_LOOP        1   // counter will start with the same value after interrupt
#define MICRODELAY      65  // delay between ISRs +- 3us

const char*       pSSID         = "9F35FE"; 
const char*       pPassword       = "55463423"; 

const char*       AWS_ENDPOINT    = "amazonaws.com";
const char*       AWS_REGION      = "us-east-1"; // REPLACE with your region
const char*       TABLE_NAME      = "espControl";  
static const char*  HASH_KEY_NAME   = "devName";  // Primary Partition Key (values are Strings)
static const char*  HASH_KEY_VALUE  = "ESP8266";    // PPK name from which to grab data
static const char*  RANGE_KEY_NAME  = "devID";    // Primary Sort Key    (values are Numbers)
static const char*  RANGE_KEY_VALUE = "1";        // PSK from which to grab data. Corresponds to each switch. TODO: make into wildcard and parse on receiving end
static const int    KEY_SIZE      = 2;        // partition and sort keys

static const char* dbInfoCol1   = "onStatus";   
static const char* dbInfoCol2   = "brightLvl";  
  
static const char* dataCols[]     = {dbInfoCol1, dbInfoCol2};
int            numOfCols      = sizeof(dataCols)/sizeof(dataCols[0]);
char*        fetchedData[2];    // not used

/* State keeping variables */
bool  needToUpdateDB    = false;
bool  needToFetchDB     = false;
int   ticksAfterZeroCross = 0;
bool  zeroCrossed       = false;

/* AC Dimming Calculations at 60Hz
|     .-.           .-.
|    /   \         /   \
|   /     \       /     \
+--/-------\-----/-------\--
| /         \   /         \
|/           '-'
    |8.33ms|8.33ms|8.33ms|
   
  One full sine wave cycle in 60Hz; 1/60 = 0.016666s or 16.66ms per full cycle.
  We have two zero crossings per wave, or ~8.333ms  per zero cross. If we delay 
  toggling pin on for  ~8.33ms after zero cross detection, the light will
  appear to be completly off. If we want 128 levels of brightness we can divide 
  8.33ms/128steps and get 0.065104ms or 65.1uS. 
  */
Ticker              fetchTimer;
Ticker              dimCheckTimer;
Esp8266HttpClient       httpClient;
Esp8266DateTimeProvider dateTimeProvider; 
AmazonDynamoDBClient    ddbClient;
GetItemInput          getItemInput; // structure that holds data obtained from DB
PutItemInput          putItemInput; // structure that holds data to be put on DB
AttributeValue        hashKey;
AttributeValue        rangeKey;     //devID
ActionError           actionError;

espSwitch         mySwitch1 = {.outPin=12, .devID={.val=1, .dbName="devID"}, .onStatus={ .val=0, .dbName="onStatus"}, .brightLvl= {.val=0, .dbName="brightLvl"}};
espSwitch         mySwitch2 = {.outPin=15, .devID={.val=2, .dbName="devID"}, .onStatus={ .val=0, .dbName="onStatus"}, .brightLvl= {.val=0, .dbName="brightLvl"}};
espSwitch         mySwitch3 = {.outPin=16, .devID={.val=3, .dbName="devID"}, .onStatus={ .val=0, .dbName="onStatus"}, .brightLvl= {.val=0, .dbName="brightLvl"}};

espSwitch         *switchArray[] = { &mySwitch1, &mySwitch2, &mySwitch3 };  // Put all your switches here     
const int         numOfSwitches  = sizeof(switchArray)/sizeof(switchArray[0]);
AttributeValue      switchIDs[numOfSwitches];
GetItemInput          getSwitchStat[numOfSwitches];      // Make objects now to avoid doing it everytime the function is called. TODO: Use single query


/*-----------------------------------------------------------------------------
 ** Function prototypes
 **---------------------------------------------------------------------------*/
void fetchCheckFunc();          // software ISR, sets neetToFetchDB to true
void checkDimState();         // software ISR, handles dimming
void zeroCrossISR();          // hardware ISR, triggers when zero cross is detected
void putValue(espSwitch switchName, attrib attributeToChange, int newVal);
void getValue();
void handleHardware();

char tmpBuff[5];
//---------------------Setup()---------------------------------
void setup() {
  Serial.begin(115200);
  Serial.print("\nAttempting to connect to SSID: ");    Serial.println(pSSID);
  WiFi.hostname("ESP8266");
  WiFi.begin(pSSID, pPassword);    
  while (WiFi.status() != WL_CONNECTED) { //TODO: Have LED blink while connecting. Green if success
      Serial.print(".");
      delay(500);
  }
  Serial.println("\nConnected to Wifi");

  //----------- Initialize Amazon DynamoDB stuffs -------------------
  ddbClient.setAWSRegion(AWS_REGION);
  ddbClient.setAWSEndpoint(AWS_ENDPOINT);
  ddbClient.setAWSSecretKey(awsSecKey);
  ddbClient.setAWSKeyID(awsKeyID);
  ddbClient.setHttpClient(&httpClient);
  ddbClient.setDateTimeProvider(&dateTimeProvider);

  // get DB info related
  hashKey.setS(HASH_KEY_VALUE);
  rangeKey.setN(RANGE_KEY_VALUE);
  MinimalKeyValuePair < MinimalString, AttributeValue > hashAttrib(HASH_KEY_NAME,   hashKey);
  MinimalKeyValuePair < MinimalString, AttributeValue > rangeAttrib(RANGE_KEY_NAME, rangeKey);
  MinimalKeyValuePair < MinimalString, AttributeValue > keyArray[] = { hashAttrib, rangeAttrib };

  MinimalString         attributesToGet[numOfCols];
  for(int i = 0; i < numOfCols; i++){    attributesToGet[i] = dataCols[i];  }   // sets up items in dataCols to be fetched

  for(int i = 0; i < numOfSwitches; i++){                     
    switchIDs[i].setN(itoa(switchArray[i]->devID.val, tmpBuff, 10)); //AttributeValue
    MinimalKeyValuePair < MinimalString, AttributeValue > tmpSwitchKey(RANGE_KEY_NAME, switchIDs[i]);
    MinimalKeyValuePair < MinimalString, AttributeValue > switchKeyArray[] = { hashAttrib, tmpSwitchKey }; // each switch with its corresponding key
    getSwitchStat[i].setAttributesToGet( MinimalList < MinimalString > (attributesToGet, numOfCols));
    getSwitchStat[i].setKey(MinimalMap < AttributeValue >              (switchKeyArray, KEY_SIZE));    
    getSwitchStat[i].setTableName(TABLE_NAME);            
}

  getItemInput.setAttributesToGet( MinimalList < MinimalString > (attributesToGet, numOfCols));
  getItemInput.setKey(MinimalMap < AttributeValue > (keyArray, KEY_SIZE));
  getItemInput.setTableName(TABLE_NAME);
  
  // put DB related
  putItemInput.setTableName(TABLE_NAME);

  //------------------- Initialize hardware ---------------------------
  pinMode(mySwitch1.outPin, OUTPUT); digitalWrite(mySwitch1.outPin, LOW);
  pinMode(mySwitch2.outPin, OUTPUT); digitalWrite(mySwitch2.outPin, LOW);
  pinMode(mySwitch3.outPin, OUTPUT); digitalWrite(mySwitch3.outPin, LOW);
  
  pinMode(HARD_ISR_PIN, INPUT_PULLUP);
  fetchTimer.attach(DB_GET_TIME, fetchCheckFunc);     // (TimeInSeconds, functionCallback )

  timer1_disable();  
  timer1_attachInterrupt(checkDimState);
  timer1_isr_init();
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);       // DIV16= 5MHz (5 ticks/us - 1677721.4 us max)
  timer1_write((clockCyclesPerMicrosecond() / 16) * MICRODELAY); 
  attachInterrupt(digitalPinToInterrupt(HARD_ISR_PIN), zeroCrossISR, RISING);

  Serial.println("Setup complete");

  for(int i = 0; i < numOfCols; i++){
    Serial.print("dataCols["); Serial.print(i); Serial.print("]= "); Serial.println(dataCols[i]);
  }
  Serial.print("Number of switches: "); Serial.println(numOfSwitches);
  //TODO: Start by reading values from DB
}

/*-----------------------------------------------------------------------------
 ** putValue(). Writes or updates value to DynamoDB
 **---------------------------------------------------------------------------*/
void putValue(espSwitch switchName, attrib attributeToChange, int newVal) {
  AttributeValue col1Attrib, col2Attrib;
  AttributeValue colArray[numOfCols];   // for mapping into array. not used
  char dbArray[numOfCols];          // for mapping into array. not used
  char  col1[4], col2[4];
  
  int   cappedNewVal;
  
  cappedNewVal = constrain(newVal, 0, 1023);

  // for(int i = 0; i < numOfCols; i++){    
  //  if( attributeToChange.dbName == dataCols[i] ) { sprintf(dbArray[i], "%d", cappedNewVal); }
  //  else                      { sprintf(dbArray[i], "%d", switchName.onStatus.val); }
  // }  
  
    // If we're not updating the value, leave it the same as it was 
  if (attributeToChange.dbName == dbInfoCol1) sprintf(col1, "%d", cappedNewVal);
  else                                sprintf(col1, "%d", switchName.onStatus.val);
    
  if (attributeToChange.dbName == dbInfoCol2) sprintf(col2, "%d", cappedNewVal);
  else                                sprintf(col2, "%d", switchName.brightLvl.val);

  col1Attrib.setS(col1);
  col2Attrib.setS(col2);

  // for(int i = 0; i < numOfCols; i++){
  //   colArray[i].setS(col1);
  // }
//
//  MinimalKeyValuePair < MinimalString, AttributeValue > itemArray[numOfCols+2]; // array with attribPairs for DB
//    itemArray[0] =  MinimalKeyValuePair < MinimalString, AttributeValue > temp(HASH_KEY_NAME,  hashKey); 
//    itemArray[1] = MinimalKeyValuePair < MinimalString, AttributeValue > rangeAttrib(RANGE_KEY_NAME, rangeKey);
//  for(int i = 2; i < numOfCols+2 ; i++){    
//    itemArray[i]  = MinimalKeyValuePair < MinimalString, AttributeValue > tmp(dataCols[i],  colArray[i]);
//  }   

MinimalKeyValuePair < MinimalString, AttributeValue > hasAttrib(HASH_KEY_NAME,          hashKey); 
MinimalKeyValuePair < MinimalString, AttributeValue > rangeAttrib(RANGE_KEY_NAME,       rangeKey);
MinimalKeyValuePair < MinimalString, AttributeValue > att3(switchName.onStatus.dbName,  col1Attrib);
MinimalKeyValuePair < MinimalString, AttributeValue > att4(switchName.brightLvl.dbName, col2Attrib);    
  
MinimalKeyValuePair < MinimalString, AttributeValue > itemArray[] = { hasAttrib, rangeAttrib, att3 , att4};

putItemInput.setItem(MinimalMap < AttributeValue > (itemArray, numOfCols + 2));     // add columns for 2 keys    
PutItemOutput putItemOutput = ddbClient.putItem(putItemInput, actionError);       /* Perform putItem */

switch (actionError) {
    case NONE_ACTIONERROR:
        Serial.println("PutItem succeeded!");                        break;
    case INVALID_REQUEST_ACTIONERROR:
        Serial.print("ERROR: Invalid request");
        Serial.println(putItemOutput.getErrorMessage().getCStr());               break;
    case MISSING_REQUIRED_ARGS_ACTIONERROR:
        Serial.println("ERROR: Required arguments were not set for PutItemInput"); break;
    case RESPONSE_PARSING_ACTIONERROR:
        Serial.println("ERROR: Problem parsing http response of PutItem");         break;
    case CONNECTION_ACTIONERROR:
        Serial.println("ERROR: Connection problem");                     break;
    }
    delay(750);  
}/*---- End of putValue() --------------*/


 /*-----------------------------------------------------------------------------
 ** getValue(). Polls values from AmazonDB. Don't call this function too frequently. Anything over
  5 polls per second will cost money?
 **---------------------------------------------------------------------------*/
void getValue(){   
  AttributeValue brightLvl;   // class properties in AmazonDynamoDBClient.h
  AttributeValue onStatus;

  for(int i = 0; i < numOfSwitches; i++){           // queary for each switch. Ew, do a single query

    GetItemOutput getItemOutput = ddbClient.getItem(getSwitchStat[i], actionError);    // perform getItem for specific switch :-(
    
    switch (actionError) {    
    case NONE_ACTIONERROR:
    {     
    MinimalMap < AttributeValue > attributeMap = getItemOutput.getItem(); // class properties in AWSFoundationalTypes.h 

    // for (int i = 0; i < numOfCols; i++){
    //  attributeMap.get(dataCols[i], fetchedData[i]);  // map to fetched array
    // }

    attributeMap.get(dbInfoCol1,   onStatus);
    attributeMap.get(dbInfoCol2,   brightLvl);       
    
    switchArray[i]->onStatus.val  = atoi(onStatus.getS().getCStr());
    switchArray[i]->brightLvl.val = atoi(brightLvl.getS().getCStr());
    
    #if DEBUG
    Serial.print("switch#= ");  Serial.print(i);
    Serial.print(" | onStatus= ");  Serial.print(onStatus.getS().getCStr());
    Serial.print(" | brightLvl = ");  Serial.println(brightLvl.getS().getCStr());   
    #endif     
          
    }
    break;
      
    case INVALID_REQUEST_ACTIONERROR:
      Serial.print("ERROR: ");
      Serial.println(getItemOutput.getErrorMessage().getCStr());
      break;
      
    case MISSING_REQUIRED_ARGS_ACTIONERROR:
      Serial.println("ERROR: Required arguments were not set for GetItemInput");
      break;
      
    case RESPONSE_PARSING_ACTIONERROR:
      Serial.println("ERROR: Problem parsing http response of GetItem\n");
      break;
      
    case CONNECTION_ACTIONERROR:
      Serial.println("ERROR: Connection problem");
      break;
  }

    delay(500);  // for safety!
  }


  // GetItemOutput getItemOutput = ddbClient.getItem(getItemInput, actionError); // perform getItem

  // switch (actionError) {    
  //   case NONE_ACTIONERROR:
  //   {     
  //   MinimalMap < AttributeValue > attributeMap = getItemOutput.getItem(); // class properties in AWSFoundationalTypes.h 

  //   // for (int i = 0; i < numOfCols; i++){
  //   //  attributeMap.get(dataCols[i], fetchedData[i]);  // map to fetched array
  //   // }

  //   attributeMap.get(dbInfoCol1,   onStatus);
  //   attributeMap.get(dbInfoCol2,   brightLvl);       
    
  //   mySwitch2.onStatus.val   = atoi(onStatus.getS().getCStr());
  //   mySwitch2.brightLvl.val  = atoi(brightLvl.getS().getCStr());
    
  //   #if DEBUG
  //   Serial.print("onStatus= ");  Serial.print(onStatus.getS().getCStr());
  //   Serial.print(" | brightLvl = ");  Serial.println(brightLvl.getS().getCStr());   
  //   #endif     
          
  //   }
  //   break;
      
  //   case INVALID_REQUEST_ACTIONERROR:
  //     Serial.print("ERROR: ");
  //     Serial.println(getItemOutput.getErrorMessage().getCStr());
  //     break;
      
  //   case MISSING_REQUIRED_ARGS_ACTIONERROR:
  //     Serial.println("ERROR: Required arguments were not set for GetItemInput");
  //     break;
      
  //   case RESPONSE_PARSING_ACTIONERROR:
  //     Serial.println("ERROR: Problem parsing http response of GetItem\n");
  //     break;
      
  //   case CONNECTION_ACTIONERROR:
  //     Serial.println("ERROR: Connection problem");
  //     break;
  // }
}/*--- end of  GetValue() -----------*/

void handleHardware(){  
  // TODO: Handle button press to manually turn the switch on/off

  // if (tempCount++ >= 10){
  //  needToUpdateDB = true;
  //  tempCount = 0;
  // }
  
  // if (mySwitch2.onStatus.val == 1 )  {
  //  analogWrite(mySwitch2.outPin, mySwitch2.brightLvl.val); // 0 to 1023
  // }else{ 
  //  analogWrite(mySwitch2.outPin, 0); 
  // }
}/*-- End handleHardware() --------*/

 /*-----------------------------------------------------------------------------
 ** Main Loop. 
 ** Checks low priority code and writes and fetches to DB
 **---------------------------------------------------------------------------*/
void loop() {
  static int testUpdateDBCount = 0;

  if(needToFetchDB){
    getValue();
    needToFetchDB = false;
    //if(testUpdateDBCount++ >= 20) { needToUpdateDB = true; testUpdateDBCount = 0}
  }

  //handleHardware(); 

//  if(needToUpdateDB ){
//    // Update relevant global variables
//    putValue(mySwitch2, mySwitch2.brightLvl, random(1, 1023));
//    needToUpdateDB = false;
//  }
  
  for(int i = 0; i < numOfSwitches; i++){
//    Serial.print(" i= "); Serial.print(i);
    Serial.print(" SW"); Serial.print(i); Serial.print(" ,lvl: "); Serial.print(switchArray[i]->brightLvl.val); Serial.print(" |");
  }
//  Serial.println("");
  Serial.print("ticks: ");Serial.println(ticksAfterZeroCross);

  delay(1000);  // Careful with removing delay. If we query/submit too much data we'll get charged
}/* -- End of loop() ------ */




 /*-----------------------------------------------------------------------------
 ** fetchCheckFunc()
 ** Sets flag for main code to fetch from database. Can handle/clean up other stuff here
 **---------------------------------------------------------------------------*/
void fetchCheckFunc(){      
  needToFetchDB = true; 
}/* -- End of fetchCheckFunc() ------ */


/*-----------------------------------------------------------------   
  This ISR triggers when we cross zero. Turn the TRIAC off at zero point and wait X amount
  of ticks before turning on TRIAC. Not a good idea to put delays here. Let's tie it to a 
  software timer interrupt
-------------------------------------------------------------------*/
void ICACHE_RAM_ATTR zeroCrossISR(){
  // TODO: change to PORTD = B10101000 ?. much faster
  for(int i = 0; i < numOfSwitches; i++){
    digitalWrite(switchArray[i]->outPin, LOW);     
  }  
  zeroCrossed         = true;
  ticksAfterZeroCross = 0;
}/*-- End zeroCrossISR() --------*/


/*-----------------------------------------------------------------   
  This ISR triggers and counts ticks X number of times between half a sine wave. If a zero cross is 
  detected, it checks that the number of ticks is more than the desired dimValue and then turns the
  pin to high. 
-------------------------------------------------------------------*/
void ICACHE_RAM_ATTR checkDimState() 
{
  if(zeroCrossed){        
    for(int i = 0; i < numOfSwitches; i++){                         // loop through all switches. NOTE: Actually only need to loop through switches that are ON
      if(switchArray[i]->onStatus.val){                        // if switch is on, check it's brightness 
        if (ticksAfterZeroCross >= switchArray[i]->brightLvl.val){  // compare to number of ticks elapsed and turn on after enough have passed
        digitalWrite(switchArray[i]->outPin, HIGH);                 
        }
      }      
    }// End for loop
   ticksAfterZeroCross++;                                           // overflow value should just roll over
//   if(ticksAfterZeroCross++ > 150)    zeroCrossed = true;           // fake zero cross if none ocurred 
  }// End if(zeroCrossed) 
  
}/*-- End checkDimState() --------*/




/* References:
https://dxarts.washington.edu/wiki/ac-dimmer-circuit

*/




















