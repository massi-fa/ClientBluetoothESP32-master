#include "mqtt.h"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

namespace awsMqtt{
    void setup()
    {
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
    
    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.setServer(AWS_IOT_ENDPOINT, 8883);
    
    // Create a message handler
    client.setCallback(messageHandler);
    
    Serial.println("Connecting to AWS IOT");
    
    while (!client.connect(THINGNAME))
    {
        Serial.print(".");
        delay(100);
    }
    
    if (!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        return;
    }
    
    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    
    Serial.println("AWS IoT Connected!");
    }

    void pubSubErr(int8_t MQTTErr){
    if (MQTTErr == MQTT_CONNECTION_TIMEOUT)
        Serial.print("Connection tiemout");
    else if (MQTTErr == MQTT_CONNECTION_LOST)
        Serial.print("Connection lost");
    else if (MQTTErr == MQTT_CONNECT_FAILED)
        Serial.print("Connect failed");
    else if (MQTTErr == MQTT_DISCONNECTED)
        Serial.print("Disconnected");
    else if (MQTTErr == MQTT_CONNECTED)
        Serial.print("Connected");
    else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)
        Serial.print("Connect bad protocol");
    else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)
        Serial.print("Connect bad Client-ID");
    else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)
        Serial.print("Connect unavailable");
    else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS)
        Serial.print("Connect bad credentials");
    else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)
        Serial.print("Connect unauthorized");

    }

    void publishMessage(float t, float h, int l){
        DynamicJsonDocument jsonBuffer(JSON_OBJECT_SIZE(3) + 100);
        JsonObject root = jsonBuffer.to<JsonObject>();
        JsonObject state = root.createNestedObject("state");
        JsonObject state_reported = state.createNestedObject("reported");
        JsonObject value = state_reported.createNestedObject("value");
        value["humidity"] = std::to_string(h);
        value["temperature"] = std::to_string(t);
        value["light"] = std::to_string(l);
        Serial.printf("Sending  [%s]: ", AWS_IOT_PUBLISH_TOPIC);
        serializeJson(root, Serial);
        Serial.println();
        char shadow[measureJson(root) + 1];
        serializeJson(root, shadow, sizeof(shadow));
        if (!client.publish(AWS_IOT_PUBLISH_TOPIC, shadow, false)){
            Serial.println("Qualcosa non va nella pubblicazione");
            pubSubErr(client.state());
        } else {
            Serial.println("Published new state");
        }
    }
    
    void messageHandler(char* topic, byte* payload, unsigned int length)
    {
        Serial.print("incoming: ");
        Serial.println(topic);
        
        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload);
        const char* message = doc["message"];
        Serial.println(message);
    }
    void loop(){
        client.loop();
    }   
}