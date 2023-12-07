#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include "MQTTClient.h"
#include "AnalogIn.h"
#include "myGPIO.h"
using namespace std;

#define ADDRESS "tcp://io.adafruit.com"
#define CLIENTID "Beagle1"
#define TOPIC "pb600159/feeds/project.temperature"
#define switchTOPIC "pb600159/feeds/project.status-indicator"
#define AUTHMETHOD "pb600159"
#define AUTHTOKEN "aio_MFGM05IWFHUZaUOZ4seL9ORTssFA"
#define QOS 1
#define TIMEOUT 10000L

float getTemperature(int adc_value)
{                                                     // from the TMP36 datasheet
   float cur_voltage = adc_value * (1.80f / 4096.0f); // Vcc = 1.8V, 12-bit
   float diff_degreesC = (cur_voltage - 0.75f) / 0.01f;
   return (25.0f + diff_degreesC);
}

int main(int argc, char *argv[])
{

   // Temperature sensor portion
   AnalogIn AIN(0);
   cout << "Attempting to upload temperature data from AIN" << AIN.getNumber() << endl;
   char str_payload[100]; // Set your max message size here
   MQTTClient client;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS)
   {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   sprintf(str_payload, "%f", getTemperature(AIN.readADCSample()));
   pubmsg.payload = str_payload;
   pubmsg.payloadlen = strlen(str_payload);
   pubmsg.qos = QOS;
   pubmsg.retained = 0;
   MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
   cout << "Waiting for up to " << (int)(TIMEOUT / 1000) << " seconds for publication of " << str_payload << " \non topic " << TOPIC << " for ClientID: " << CLIENTID << endl;
   rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
   if (rc == MQTTCLIENT_SUCCESS)
   {
      cout << "Message with token " << (int)token << " delivered." << endl;
   }
   else
   {
      cout << "Did not complete with error code: " << rc << endl;
      // MQTTCLIENT_SUCCESS 0           MQTTCLIENT_FAILURE -1
      // MQTTCLIENT_DISCONNECTED -3     MQTTCLIENT_MAX_MESSAGES_INFLIGHT -4
      // MQTTCLIENT_BAD_UTF8_STRING -5  MQTTCLIENT_NULL_PARAMETER -6
      // MQTTCLIENT_TOPICNAME_TRUNCATED -7   MQTTCLIENT_BAD_STRUCTURE -8
      // MQTTCLIENT_BAD_QOS   -9        MQTTCLIENT_SSL_NOT_SUPPORTED   -10
   }

   // portion for push button
   cout << "Attempting to upload push button data" << endl;
   MYGPIO switchGPIO(46);
   switchGPIO.setDirection(INPUT);
   cout << "The value of the input is: " << switchGPIO.getValue() << endl;

   sprintf(str_payload, "%d", switchGPIO.getValue());
   pubmsg.payload = str_payload;
   pubmsg.payloadlen = strlen(str_payload);
   pubmsg.qos = QOS;
   pubmsg.retained = 0;
   MQTTClient_publishMessage(client, switchTOPIC, &pubmsg, &token);
   cout << "Waiting for up to " << (int)(TIMEOUT / 1000) << " seconds for publication of " << str_payload << " \non topic " << switchTOPIC << " for ClientID: " << CLIENTID << endl;
   rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
   if (rc == MQTTCLIENT_SUCCESS)
   {
      cout << "Message with token " << (int)token << " delivered." << endl;
   }
   else
   {
      cout << "Did not complete with error code: " << rc << endl;
      // MQTTCLIENT_SUCCESS 0           MQTTCLIENT_FAILURE -1
      // MQTTCLIENT_DISCONNECTED -3     MQTTCLIENT_MAX_MESSAGES_INFLIGHT -4
      // MQTTCLIENT_BAD_UTF8_STRING -5  MQTTCLIENT_NULL_PARAMETER -6
      // MQTTCLIENT_TOPICNAME_TRUNCATED -7   MQTTCLIENT_BAD_STRUCTURE -8
      // MQTTCLIENT_BAD_QOS   -9        MQTTCLIENT_SSL_NOT_SUPPORTED   -10
   }

   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}
