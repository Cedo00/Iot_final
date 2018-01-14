# -*- coding: utf-8 -*-
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("pi")

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))


client=mqtt.Client()
client.username_pw_set("uwreopem","fzy16W0799Vg")#替换成你的用户名和密码
client.on_connect = on_connect
client.on_message = on_message
client.connect("m14.cloudmqtt.com", 16761, 60)
client.loop_forever()
