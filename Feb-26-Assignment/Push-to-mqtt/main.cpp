#define MQTTCLIENT_QOS2 1
#include <mbed.h>
#include <MQTTClientMbedOs.h>
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"

DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
Thread t;
EventQueue queue(5 * EVENTS_EVENT_SIZE);
Serial pc(USBTX, USBRX);
WiFiInterface *wifi;
TCPSocket socket;
MQTTClient client(&socket);

char* hostname = "mqtt.netpie.io";
char* clientID = "a306c380-9f11-4d10-b514-a34201a8db27";
char* username = "Q7Twq7qFerR33BNURwtqKJ1bE8XFihPb";
char* password = "";
char* topic = "@msg/stm32";
float version = 0.6;

int arrivedcount = 0;


void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    pc.printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
    pc.printf("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}
MQTT::Message message;

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_wifi() {
    WiFiAccessPoint *ap;

    printf("Scan:\n");
    int count = wifi->scan(NULL,0);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;
    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;   

    return count; 
}
void pressed_handler() {
    int count;

    count = scan_wifi();
    if (count == 0) {
        pc.printf("No WIFI APs found - can't continue further.\n");
        return;
    }

    pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        pc.printf("\nConnection error: %d\n", ret);
        return;
    }

    pc.printf("Success\n\n");
    pc.printf("MAC: %s\n", wifi->get_mac_address());
    SocketAddress a;
    wifi->get_ip_address(&a);
    pc.printf("IP: %s\n", a.get_ip_address());
    wifi->get_netmask(&a);
    pc.printf("Netmask: %s\n", a.get_ip_address());
    wifi->get_gateway(&a);
    pc.printf("Gateway: %s\n", a.get_ip_address());
    pc.printf("RSSI: %d\n\n", wifi->get_rssi());

//*******************************************************************************************
	MQTTNetwork mqttNetwork(wifi);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    int port = 1883;
    pc.printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
        pc.printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = clientID;
    data.username.cstring = username;
    data.password.cstring = password;
    if ((rc = client.connect(data)) != 0)
        pc.printf("rc from MQTT connect is %d\r\n", rc);


    MQTT::Message message;

    // QoS 0
    char buf[100];
    sprintf(buf, "MQQT TEST MESSAGE");
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
	pc.printf("Publishing...");
    while (arrivedcount < 1)
        client.yield(100);
	
	// QoS 1
    sprintf(buf, "MQQT TEST MESSAGE QOS1");
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 2)
        client.yield(100);

    // QoS 2
    sprintf(buf, "MQQT TEST MESSAGE QOS2");
    message.qos = MQTT::QOS2;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 3)
        client.yield(100);
	
	mqttNetwork.disconnect();

    wifi->disconnect();
    pc.printf("\nDone\n");    
}

int main() {
    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(pressed_handler));
    pc.printf("Starting\n");
    while(1) {
        led = !led;
        ThisThread::sleep_for(500);
    }
}