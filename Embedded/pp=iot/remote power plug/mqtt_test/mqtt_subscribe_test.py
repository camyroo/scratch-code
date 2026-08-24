#!/usr/bin/env python3
"""Connects to the local MQTT broker over TLS and prints relay state messages."""

import os
import ssl
import sys
import traceback

import paho.mqtt.client as mqtt

BROKER_HOST = "192.168.4.141"
BROKER_PORT = 8883
CA_CERT_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "ca.crt")
USERNAME = "cam"
PASSWORD = "1231"
TOPIC = "home/ps/1/relay/+/state"


def on_connect(client, userdata, flags, reason_code, properties=None):
    if reason_code == 0:
        print(f"[SUCCESS] Connected to {BROKER_HOST}:{BROKER_PORT} over TLS.")
        result, _ = client.subscribe(TOPIC)
        if result == mqtt.MQTT_ERR_SUCCESS:
            print(f"[SUCCESS] Subscribed to topic '{TOPIC}'.")
        else:
            print(f"[FAILURE] Subscribe request failed with code {result}.")
    else:
        print(f"[FAILURE] Connection refused by broker: {reason_code}")


def on_disconnect(client, userdata, flags, reason_code, properties=None):
    print(f"[INFO] Disconnected (reason: {reason_code}).")


def on_message(client, userdata, msg):
    print(f"[MESSAGE] {msg.topic}: {msg.payload.decode(errors='replace')}")


def main():
    if not os.path.isfile(CA_CERT_PATH):
        print(f"[FAILURE] CA certificate not found at {CA_CERT_PATH}")
        sys.exit(1)

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    try:
        client.tls_set(
            ca_certs=CA_CERT_PATH,
            cert_reqs=ssl.CERT_REQUIRED,
            tls_version=ssl.PROTOCOL_TLS_CLIENT,
        )
    except Exception:
        print("[FAILURE] TLS setup failed:")
        traceback.print_exc()
        sys.exit(1)

    try:
        client.connect(BROKER_HOST, BROKER_PORT, keepalive=60)
    except Exception:
        print(f"[FAILURE] Could not connect to {BROKER_HOST}:{BROKER_PORT}:")
        traceback.print_exc()
        sys.exit(1)

    print("[INFO] Listening for messages. Press Ctrl+C to stop.")
    try:
        client.loop_forever()
    except KeyboardInterrupt:
        print("\n[INFO] Interrupted by user, shutting down.")
        client.disconnect()


if __name__ == "__main__":
    main()
