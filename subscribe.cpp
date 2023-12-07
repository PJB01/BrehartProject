#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "LED.h"
#include "myGPIO.h"
using namespace std;

#define ADDRESS "tcp://io.adafruit.com:1883"
#define CLIENTID "Beagle2"
#define TOPICTEMP "pb600159/feeds/project.temperature"
#define TOPICBLINK "pb600159/feeds/project.text-box-output"
#define TOPICTOGGLE "pb600159/feeds/project.toggle-device"
#define AUTHMETHOD "pb600159"
#define AUTHTOKEN "aio_MFGM05IWFHUZaUOZ4seL9ORTssFA"
#define QOS 1
#define TIMEOUT 10000L
#define THRESHOLD 30

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{

    LED USR0 = LED(0); // for temperature if below threshold turn on
    LED USR3 = LED(3); // for text box
    MYGPIO outputGPIO(60); //set too p9.12 for ac toggle
    outputGPIO.setDirection(OUTPUT); // set to output
    int i;
    char *payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = (char *)message->payload;

    // turn on USR0 if TEMP is less than THRESHOLD
    if (strcmp(topicName, "pb600159/feeds/project.temperature") == 0)
    {
        float temperature = atof(payloadptr);
        printf("The temperature is %f\n", temperature);
        if (temperature < THRESHOLD)
        {
            USR0.turnOn();
        }
        else
        {
            USR0.turnOff();
        }
    }

    // blink USR3 x amount of times
    if (strcmp(topicName, "pb600159/feeds/project.text-box-output") == 0)
    {
        int numBlinks = atoi(payloadptr);
        printf("Blinking USR3 %d times\n", numBlinks);
        USR3.blink(numBlinks);
    }

    // toggle AC device
    if (strcmp(topicName, "pb600159/feeds/project.toggle-device") == 0)
    {
        float temperature = atof(payloadptr);
        if (strcmp(payloadptr, "ON") == 0)
        {
            printf("Turning ON device\n");
            outputGPIO.setValue(HIGH);
        }
        else
        {
            printf("Turning OFF device\n");
            outputGPIO.setValue(LOW);
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\n and %s for client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n",
           TOPICTEMP, TOPICBLINK, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPICBLINK, QOS);
    MQTTClient_subscribe(client, TOPICTEMP, QOS);
    MQTTClient_subscribe(client, TOPICTOGGLE, QOS);

    do
    {
        ch = getchar();
    } while (ch != 'Q' && ch != 'q');
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
