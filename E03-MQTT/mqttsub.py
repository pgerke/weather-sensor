#!/usr/bin/python3
import paho.mqtt.client as mqtt # Import the MQTT client

# The host name of the machine running the MQTT broker
host = "<YOUR BROKER HOST>"

# The handler method to be called when the connection is established
def on_connect(client, userdata, flags, rc):
  client.subscribe('/sensor/temperature')
  client.subscribe('/sensor/humidity')
  client.subscribe('/sensor/pressure')
  client.subscribe('/sensor/altitude')
  print("Subscribed to sensor topics")

# The handler method for a received message
def on_message(client, userdata, message):
  msg = str(message.payload.decode("utf-8"))
  print(f"MQTT message received for topic {message.topic}: {msg}")

# Create Client instance
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
 
# Connect to MQTT broker
client.connect(host)
 
print("Connected to MQTT Broker: " + host)
print("Press Ctrl-C to quit.")

# Keep the connection open until a keyboard interrupt is detected.
try:
  client.loop_forever()
except KeyboardInterrupt:
  print("Closing MQTT connection...")
finally:
  client.disconnect(0)

print("Done, bye!")
