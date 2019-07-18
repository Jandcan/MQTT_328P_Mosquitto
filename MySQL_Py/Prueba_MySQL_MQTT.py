from __future__ import print_function
from datetime import date, datetime, timedelta
import mysql.connector
import paho.mqtt.client as mqtt
import re
#Parametros Base de datos
host='127.0.0.1'
Database='BASE_MQTT'
User='root'
Pass=''
#Parametros Broker
URL_mqtt="192.168.1.9"
Puerto_mqtt=1883

def AlmacenarMySQL(trama):
    cnx = mysql.connector.connect(host=host,
                                    database=Database,
                                    user=User,
                                    password=Pass)
    cursor = cnx.cursor()

    Trama_MySQL = ("INSERT INTO IOT_Piso_Jandry "
                "(Fecha,ID_Disp, Habitacion, Temp_A, Hum_A, Angulo) "
                "VALUES (NOW(),%s, %s, %s, %s, %s)")
    Datos=re.findall('d(.*?)#',trama)
    print(Datos)
    
    Datos_MySQL = (int(Datos[0]), Datos[1], float(Datos[2]) , float(Datos[3]), Datos[4])

    # Insertamos nuevos valores
    cursor.execute(Trama_MySQL, Datos_MySQL)
    emp_no = cursor.lastrowid

    # Aseguramos que la base de Datos a receptado los datos
    cnx.commit()
    cursor.close()
    cnx.close()
# funcion que conecta al servidor Mosquitto
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    #  Una ves enlazados nos conectamos al topic deseado
    client.subscribe("test")

# funcion que recepta los paquetes que envian al topic MQTT y los almacena en la base de datos
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    AlmacenarMySQL(str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(URL_mqtt, Puerto_mqtt, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()