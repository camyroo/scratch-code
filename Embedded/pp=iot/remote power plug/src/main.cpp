#include <Arduino.h>
#include <PsychicMqttClient.h>
#include <mbedtls/x509_crt.h>
#include <esp_idf_version.h>

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <map>

PsychicMqttClient mqttClient;

// Replace with your network credentials
const char *ssid = "SJHouse";
const char *password = "mahalo123";

const char *PARAM_INPUT_1 = "output";
const char *PARAM_INPUT_2 = "state";

const char *eclipse_root_ca = "-----BEGIN CERTIFICATE-----\n"
                              "MIIEKzCCAxOgAwIBAgIUSBsSYgSAJorY66VoJNcJqea5AMowDQYJKoZIhvcNAQEL\n"
                              "BQAwgaQxCzAJBgNVBAYTAlVTMRMwEQYDVQQIDApDYWxpZm9ybmlhMREwDwYDVQQH\n"
                              "DAhTYW4gSm9zZTEYMBYGA1UECgwPQ2FtZXJvbiBIb21lIENBMREwDwYDVQQLDAhz\n"
                              "LXNlcnZlcjEYMBYGA1UEAwwPQ2FtZXJvbiBIb21lIENBMSYwJAYJKoZIhvcNAQkB\n"
                              "FhdsZWVjYW1lcm9uMjQzQGdtYWlsLmNvbTAeFw0yNjA4MjIyMzQ3MTlaFw0yNzA4\n"
                              "MjIyMzQ3MTlaMIGkMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTER\n"
                              "MA8GA1UEBwwIU2FuIEpvc2UxGDAWBgNVBAoMD0NhbWVyb24gSG9tZSBDQTERMA8G\n"
                              "A1UECwwIcy1zZXJ2ZXIxGDAWBgNVBAMMD0NhbWVyb24gSG9tZSBDQTEmMCQGCSqG\n"
                              "SIb3DQEJARYXbGVlY2FtZXJvbjI0M0BnbWFpbC5jb20wggEiMA0GCSqGSIb3DQEB\n"
                              "AQUAA4IBDwAwggEKAoIBAQCytWwG2u5YipjTddG8X6IS5UaziTXXmH8WoR3KlcwA\n"
                              "lRFkUvicpwEDOCDqJ7Ef1BMwM8Y3fhmsAOB1uIumLjSg+99XnlBvwqtG8Ma+g7W2\n"
                              "a5C59/jRqcI62pEQKJnddV1xpW5+5nIi/u0qx3KyaD5L8/CpJhWpscDS/fz7tL/p\n"
                              "EKrQoBSsSEFCB8oQMfHnHKg27PIPF5x0gFSgdkm5VsEalLrBaxieW3Qv+ymyNB3a\n"
                              "hLr7m8xKU++Cds0TPMmKFRNRA+h6L88ianQLFfG76jwZBnREBsltIZKAK6kF8G5G\n"
                              "K6XOQM54D15IP4HMmu66C4MtEZbgdln5mp1z1vdAPwbfAgMBAAGjUzBRMB0GA1Ud\n"
                              "DgQWBBR9TFG1PJukUX6Nk7h/wtGRY04jEzAfBgNVHSMEGDAWgBR9TFG1PJukUX6N\n"
                              "k7h/wtGRY04jEzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAU\n"
                              "zTcWLJm9wTFRDjMyD20eX/ksssUG6HoFrUQEV3SRglA7hKgtCHrmAjzPUkdZtcCi\n"
                              "olz6ZiiXtCYBhAqYQ5O7QEH45Ph97/fSmfH7B+g2MZ+Z082TYZWZLmVPjrJNM0M2\n"
                              "xi7Moxg5P7IbZS7wUQuUlD++rOpu2y/Dd9zjtIZ3TZMsA0/jzyM1Zgk4wvDkel7j\n"
                              "iZgYh44oohhht301QSeNJFXoObYxqpux4g3RrjPHjOPYRwjeUf/El/Hmjy9avNNO\n"
                              "u9uahnEVD9mfaSH8xOa3YWwHa8naC/hzf8lTZDh3RAVNWcIukYlTA4NCU/KiTJZj\n"
                              "OyaGU8+KOorUhcWSHLnH\n"
                              "-----END CERTIFICATE-----\n";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

String outputState(int output);
void publishAllStates();

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    buttons += "<h4>Output - GPIO 16</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"16\" " + outputState(16) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 17</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"17\" " + outputState(17) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 21</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"21\" " + outputState(21) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 22</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"22\" " + outputState(22) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output)
{
  if (digitalRead(output))
  {
    return "checked";
  }
  else
  {
    return "";
  }
}

void init_wifi()
{

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  configTime(0, 0, "pool.ntp.org");
  Serial.print("Waiting for NTP time sync");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) 
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();
  Serial.print("Current time: ");
  Serial.println(ctime(&now));
}

void init_http()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      String html = String(index_html);
      html.replace("%BUTTONPLACEHOLDER%", processor("BUTTONPLACEHOLDER"));
      request->send(200, "text/html", html); });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  String inputMessage1;
  String inputMessage2;
  if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
    inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
    inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
    digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    publishAllStates();
  }
  else {
    inputMessage1 = "No message sent";
    inputMessage2 = "No message sent";
  }
  Serial.print("GPIO: ");
  Serial.print(inputMessage1);
  Serial.print(" - Set to: ");
  Serial.println(inputMessage2);
  request->send(200, "text/plain", "OK"); });

  // Start server
  server.begin();
}

struct DictEntry
{
  int key;
  int value;
};

DictEntry relay_to_pin[] = {
    {1, 16},
    {2, 17},
    {3, 21},
    {4, 22}};

int getPin(int relayId)
{
  int arraySize = sizeof(relay_to_pin) / sizeof(relay_to_pin[0]);
  for (int i = 0; i < arraySize; i++)
  {
    if (relay_to_pin[i].key == relayId)
      return relay_to_pin[i].value;
  }
  return -1;
}

int getRelay(String topic)
{
  String cleaned = topic;
  cleaned.replace("home/ps/1/relay/", "");
  cleaned.replace("/set", "");
  cleaned.replace("/state", "");

  if (cleaned.length() >= 1 && isDigit(cleaned[0]))
    return cleaned.toInt();

  return -1;
}

void publishAllStates()
{
  int arraySize = sizeof(relay_to_pin) / sizeof(relay_to_pin[0]);
  for (int i = 0; i < arraySize; i++)
  {
    int relayId = relay_to_pin[i].key;
    int pin = relay_to_pin[i].value;

    String topic = "home/ps/1/relay/" + String(relayId) + "/state";
    const char *state = digitalRead(pin) ? "ON" : "OFF";

    mqttClient.publish(topic.c_str(), 0, true, state);
  }
}

void onMessage(String topic, String payload)
{
  if (!topic.startsWith("home/ps/1/relay/"))
    return;

  int relay = getRelay(topic);
  if (relay == -1)
    return;

  int pin = getPin(relay);
  if (pin == -1)
    return;

  if (payload == "ON")
    digitalWrite(pin, HIGH);
  else if (payload == "OFF")
    digitalWrite(pin, LOW);
  else
    return;

  publishAllStates();
}

void onMqttError(esp_mqtt_error_codes_t error)
{
  if (error.error_type != MQTT_ERROR_TYPE_TCP_TRANSPORT)
    return;

  Serial.printf("[MQTT ERROR] esp_tls_last_esp_err=0x%x esp_tls_stack_err=0x%x esp_tls_cert_verify_flags=0x%x transport_sock_errno=%d\r\n",
                error.esp_tls_last_esp_err,
                error.esp_tls_stack_err,
                error.esp_tls_cert_verify_flags,
                error.esp_transport_sock_errno);

  if (error.esp_tls_cert_verify_flags != 0)
  {
    char info_buf[256];
    int len = mbedtls_x509_crt_verify_info(info_buf, sizeof(info_buf), "  ! ", (uint32_t)error.esp_tls_cert_verify_flags);
    if (len > 0)
    {
      Serial.println("[MQTT ERROR] Certificate verification failure reason(s):");
      Serial.print(info_buf);
    }
  }
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);

  Serial.printf("eclipse_root_ca length: %d characters\r\n", strlen(eclipse_root_ca));

  pinMode(16, OUTPUT); // 1
  digitalWrite(16, LOW);
  pinMode(17, OUTPUT); // 2
  digitalWrite(17, LOW);
  pinMode(21, OUTPUT); // 3
  digitalWrite(21, LOW);
  pinMode(22, OUTPUT); // 4
  digitalWrite(22, LOW);

  init_wifi();

  // All MQTT configuration must happen before connect()
  mqttClient.setServer("mqtts://192.168.4.141:8883");
  // mqttClient.setServer("mqtts://cleelab.duckdns.org:8883");
  mqttClient.setCACert(eclipse_root_ca);
  mqttClient.setCredentials("cam", "1231");
  mqttClient.setWill("home/ps/1/status", 0, true, "offline");

  // mbedtls on this ESP-IDF build doesn't match an IP-literal hostname against
  // an iPAddress SAN entry, even though the broker cert has one. CA-signature
  // verification stays fully enforced; only the CN/SAN hostname check is skipped.
#if ESP_IDF_VERSION_MAJOR == 5
  mqttClient.getMqttConfig()->broker.verification.skip_cert_common_name_check = true;
#else
  mqttClient.getMqttConfig()->skip_cert_common_name_check = true;
#endif

  mqttClient.onConnect([&](bool sessionPresent)
                       {
                         Serial.println("MQTT connected");
                         mqttClient.publish("home/ps/1/status", 0, true, "online");
                         publishAllStates(); });

  mqttClient.onTopic("home/ps/1/relay/+/set", 0, [&](const char *topic, const char *payload, int retain, int qos, bool dup)
                     {
                       Serial.printf("Received Topic: %s\r\n", topic);
                       Serial.printf("Received Payload: %s\r\n", payload);
                       onMessage(topic, payload); });

  mqttClient.onError(onMqttError);

  Serial.printf("Free heap before mqttClient.connect(): %u bytes\r\n", ESP.getFreeHeap());
  mqttClient.connect();

  init_http();
}

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 30000; // 30 seconds

void loop()
{
  unsigned long now = millis();
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL)
  {
    lastHeartbeat = now;
    mqttClient.publish("home/ps/1/heartbeat", 0, false, String(now).c_str());
  }
}

// mosquitto_pub -h <broker_host> -p <port> --cafile <ca_cert_path> -t "<topic>" -m "<message>" -u <username> -P <password>
//  mosquitto_sub -h <broker_host> -p <port> --cafile <ca_cert_path> -t "<topic>" -u <username> -P <password>