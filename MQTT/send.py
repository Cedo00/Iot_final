import paho.mqtt.publish as publish

HOST = "m14.cloudmqtt.com"

publish.single("pi", "payload", hostname=HOST, port=16761,
               auth={'username': "uwreopem", 'password':"fzy16W0799Vg"})
