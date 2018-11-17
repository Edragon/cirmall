var http        = require('https');
var AlexaSkill  = require('./AlexaSkill');
var AWS         = require("aws-sdk");         // for writing to DynamoDB

var APP_ID          = undefined;               // not used?
var skillName       = "ESP Control";
var invocationName  = "The House";

/*------  DynamoDB configuration ------ */
var tableName       = "espControl";
var partitionKey    = "devName";    // Primary Partition Key
var partitionKeyVal = "ESP8266";    // PPK Name
var sortKey         = "devID";     // / Primary Sort Key. PSK value depends on command
var dbInfoCol1      = "onStatus";   
var dbInfoCol2      = "brightLvl";   

/*------  
Devices that ESP will be controlling. make sure these correspond to the numbers 
of the arduno program
TODO: Make these names be change-able from Alexa
 ------ */
var deviceOne   = "1st lamp";            
var deviceTwo   = "3rd lamp";
var deviceThree = "second lamp";   // plugged in differently
var deviceAll   = "everything";

AWS.config.update({ region: "us-east-1"  });          // update if needed
var docClient = new AWS.DynamoDB.DocumentClient();

/*------------- Useful functions --------*/
function clamp(val, min, max) {    return val > max ? max : val < min ? min : val;}
function map_range(value, low1, high1, low2, high2) {    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);}


var mySkill   = function () {
  AlexaSkill.call(this, APP_ID);
};

// Extend AlexaSkill
mySkill.prototype = Object.create(AlexaSkill.prototype);
mySkill.prototype.constructor = mySkill;

//----------------------------- Session Started --------------------------------
mySkill.prototype.eventHandlers.onSessionStarted = function (sessionStartedRequest, session) {
  console.log(invocationName + "onSessionStarted requestId: " + sessionStartedRequest.requestId+ ", sessionId: " + session.sessionId);
     // any initialization logic goes here
};

//----------------------------- On Launch event (no arguments passed)---------------------------
mySkill.prototype.eventHandlers.onLaunch = function (launchRequest, session, response) {
  console.log(invocationName + " onLaunch requestId: " + launchRequest.requestId + ", sessionId: " + session.sessionId);
  var speechOutput = "Welcome to " + invocationName + ", what would you like me to do? I can turn devices on, and dim them. Currently, I have " + deviceOne +" , " + deviceTwo + ", and " + deviceThree;
  var repromptText = "Are you still there?";
  response.ask(speechOutput, repromptText);
};

//-------------------------- Session ended event -----------------------------
mySkill.prototype.eventHandlers.onSessionEnded = function (sessionEndedRequest, session) {
  console.log(skillName + " onSessionEnded requestId: " + sessionEndedRequest.requestId + ", sessionId: " + session.sessionId);
  // any cleanup logic goes here
};

//----------------------------- Intent Handlers --------------------------------
mySkill.prototype.intentHandlers = {
  
  "onOffIntent": function(intent, session, response){
    var deviceDbID, dimValue, onOrOffBinary, brightResponse;
    var everythingVar = 0;
    // Slot values could be blank. If it exists assign it, otherwise make var "-"
    var deviceName  = intent.slots.deviceName    ? intent.slots.deviceName.value   : "-";
    var onOrOff     = intent.slots.onOrOff       ? intent.slots.onOrOff.value      : "-";    
    var brightCmd   = intent.slots.brightnessCmd ? intent.slots.brightnessCmd.value: "0";    // could be undefined or string
    
    if (brightCmd == undefined){  // must've commanded on or off
      brightCmd      = "100";     // full brightness
      brightResponse = 0;
    }else{                        // brightCmd will have a value
      brightResponse = 1;
      onOrOff        = "on";
    }

    switch(deviceName){    
      case deviceOne:       deviceDbID    = 1;        break;
      case deviceTwo:       deviceDbID    = 2;        break;
      case deviceThree:     deviceDbID    = 3;        break;
      case deviceAll:       everythingVar = 1;        break;  // device is 'everything'
      default:              deviceDbID    = 0;        break;
    }
    switch( onOrOff ){      
      case "on":        onOrOffBinary = "1";       break;
      case "off":       onOrOffBinary = "0";       break;
      default:          onOrOffBinary = "0";       break;
    }

    var maximum = 4;
    var minimum = 1;
    var randomNum = Math.floor(Math.random() * (maximum - minimum + 1)) + minimum;        

    /* ----------- Linearize brightness command 
      Brightness should be linearly related to the power going into the bulb. This should be
      proportional to the area under half a sine wave. If we integrate a sine wave from 0 to pi we get a value of 2.
      To find the area under a curve we use : Integral{0}^{x} sin(x) dx. This the Area at x = 1-cox(x).
      At time Pi, we get 1-cos(pi) = 2, or the full half wave. At time pi/2 we get 1-cos(pi/2) = 0.5, or half the wave.
      Now we need to map this value toor ADU on the chip; 0 to pi is equivalent to 0-128 units.
      Our final equation becomes: ADUdelay = arccos(1-2*dimPercentage)*120/pi

    ------------------------------------------------*/
    dimValue = clamp(Number(brightCmd), 0, 100)/100;                   // in perc. 0 will map to off, 100 to full brightness
    dimValue = 128-Math.round(Math.acos(1-2*dimValue)*120.0/3.1415);

    // dimValue = 105-Math.round(map_range(dimValue, 0, 100, 0, 105));   // linear map 0-100% to 105 (~off) to 0 (full brightness)
    dimValue = dimValue.toString();                                                  

    if (deviceDbID == 0){
      response.tell("Sorry, I don't know " + deviceName );
      console.log("Slots: ",  intent.slots);
      console.log("CMD: " + deviceName + " ,  " + onOrOff + " , " + brightCmd + "(" + dimValue + "). EverythingFlag:" + everythingVar );
    }else{        // We know this  device

    var params = {                            // What gets sent to DynamoDB 
      TableName: tableName,
      Key:{
        [partitionKey]: partitionKeyVal,        
        [sortKey]     : deviceDbID            // Number
      },
      UpdateExpression: "set " + dbInfoCol1 + "= :col1, " + dbInfoCol2 + "= :col2",
      ExpressionAttributeValues:{
        ":col1": onOrOffBinary,               // String
        ":col2": dimValue                     // String
      },
      ReturnValues: "UPDATED_NEW"
    };

    console.log("Slots: ",  intent.slots);
    console.log("CMD: " + deviceName + " ,  " + onOrOff + " , " + brightCmd + "(" + dimValue + "). EverythingFlag:" + everythingVar );
    console.log("Param created: ", params);

    if (everythingVar == 0){ 
      docClient.update(params, function(err, data) {                                        // updates once
        if (err) {
          console.error("Unable to update item. Error JSON:", JSON.stringify(err, null, 2));
          response.tell("Error updating the database");
        } 
        else {      
          console.log("UpdateItem succeeded:", JSON.stringify(data, null, 2));

          if (brightResponse == 0 ){       // cmd was turn on/off
            switch(randomNum){
                case 1:   var speechOutput = "Ok, turning the " + deviceName  + " " + onOrOff ; break;
                case 2:   var speechOutput = "Alright, if you say so" ;                         break;
                case 3:   var speechOutput = "Fine, if you insist" ;                            break;
                case 4:   var speechOutput = "Your wish is my command" ;                        break;
                default:  var speechOutput = "Ok, turning the " + deviceName  + " " + onOrOff ; break;
            }
          response.tellWithCard(speechOutput, invocationName, "Turned the "+ deviceName + " " + onOrOff );
          }else{                        // cmd was to dim
            switch(randomNum){
                case 1:  var speechOutput = "Ok, dimming " + deviceName + " to " + brightCmd + " percent" ; break;
                case 2:  var speechOutput = "Alright, but I won't like it" ;                                break;
                case 3:  var speechOutput = "Fine, if you really want me to" ;                              break;
                case 4:  var speechOutput = "Your wish is my command" ;                                    break;
                default: var speechOutput = "Ok, turning the " + deviceName  + " " + onOrOff ;              break;
            } 
          response.tellWithCard(speechOutput, invocationName, "The " + deviceName+ " has been dimmed to " + brightCmd );
          } 

        } // Enf of else, (!err)
      });// End of docClient.update()

  }else{ // (everythingVar == 1)
    // TODO: use ConditionExpression or BatchWriteItem instead ?
     var param1 = {                            // What gets sent to ESP device
      TableName: tableName,
      Key:{
        [partitionKey]: partitionKeyVal,        
        [sortKey]     : 1            // Number
      },
      UpdateExpression: "set " + dbInfoCol1 + "= :col1, " + dbInfoCol2 + "= :col2",
      ExpressionAttributeValues:{
        ":col1": onOrOffBinary,               // String
        ":col2": dimValue                     // String
      },
      ReturnValues: "UPDATED_NEW"
    };
    var param2 = {                            // What gets sent to ESP device
      TableName: tableName,
      Key:{
        [partitionKey]: partitionKeyVal,        
        [sortKey]     : 2            // Number
      },
      UpdateExpression: "set " + dbInfoCol1 + "= :col1, " + dbInfoCol2 + "= :col2",
      ExpressionAttributeValues:{
        ":col1": onOrOffBinary,               // String
        ":col2": dimValue                     // String
      },
      ReturnValues: "UPDATED_NEW"
    };
    var param3 = {                            // What gets sent to ESP device
      TableName: tableName,
      Key:{
        [partitionKey]: partitionKeyVal,        
        [sortKey]     : 3            // Number
      },
      UpdateExpression: "set " + dbInfoCol1 + "= :col1, " + dbInfoCol2 + "= :col2",
      ExpressionAttributeValues:{
        ":col1": onOrOffBinary,               // String
        ":col2": dimValue                     // String
      },
      ReturnValues: "UPDATED_NEW"
    };

    docClient.update(param1, function(err, data) {                                        
        if (err) {          console.error("Unable to update item. Error JSON:", JSON.stringify(err, null, 2));        } 
        else {              console.log("UpdateItem succeeded:", JSON.stringify(data, null, 2));                      }
      });// End of docClient.update()
    docClient.update(param2, function(err, data) {                                        
        if (err) {          console.error("Unable to update item. Error JSON:", JSON.stringify(err, null, 2));        } 
        else {              console.log("UpdateItem succeeded:", JSON.stringify(data, null, 2));                      }
      });// End of docClient.update()
    docClient.update(param3, function(err, data) {                                        
        if (err) {          console.error("Unable to update item. Error JSON:", JSON.stringify(err, null, 2));        } 
        else {              response.tellWithCard("Ok! here goes nothing", invocationName, "All the things are " + onOrOff); }
      });// End of docClient.update()

  }// End of if everythingVar == 1

}// End of we know this device

  },//--- End of onOffIntent ---


  "AMAZON.HelpIntent": function (intent, session, response) {
    response.tell("Welcome to " + skillName + ", what would you like me to do? I can turn devices on and off devices and dim them. Currently, I have " + deviceOne +" , " + deviceTwo + ", and " + deviceThree);
  },

}; //---- End of mySkill.prototype.intentHandlers --------



//-----------------------------Create the handler that responds to the Alexa Request.
exports.handler = function (event, context) {
  var customSkill = new mySkill();
  customSkill.execute(event, context);
};

