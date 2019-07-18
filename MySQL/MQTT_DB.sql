#Creamos la base de datos
CREATE DATABASE IF NOT EXISTS BASE_MQTT;
#Selecionamos la base de DATOS
USE BASE_MQTT;

#creamos TABLA SENSOR DHT11 Y Actuador
CREATE TABLE IF NOT EXISTS IOT_Piso_Jandry (
    Nro_Muestra INT AUTO_INCREMENT,
    Fecha VARCHAR(50),
    ID_Disp INT NOT NULL,
    Habitacion VARCHAR(50) NOT NULL,
    Temp_A FLOAT,
    Hum_A FLOAT,
    Actuador ENUM('Encendido', 'Apagado') NOT NULL,
    PRIMARY KEY (Nro_Muestra)
)  ENGINE=INNODB;