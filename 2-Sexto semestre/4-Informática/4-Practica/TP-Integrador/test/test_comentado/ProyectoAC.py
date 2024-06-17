#imports
import RPi.GPIO as GPIO
import time
import os
import sys
from datetime import datetime
import socket
import encodings
from threading import Thread

os.system('modprobe w1-gpio')     #cargamos los drivers del dispositivo
os.system('modprobe w1-therm')

#Carpeta del Sensor de Temperatura
Carpeta_Sensor = '/sys/bus/w1/devices/28-0316b5ed2dff'

#Conexion TCP/IP
HOST = '192.168.100.49'
PORT = 60000
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) ##Se crea el socket del server
server.bind((HOST, PORT)) ##Se bindea al puerto y direccion indicados
server.listen() ##Se lo dispone para escuchar nuevas conexiones

#Pinouts
p_pulsador = 3
p_verde = 22
p_rojo = 27
p_amarillo = 17

#Inicializacion
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(p_pulsador, GPIO.IN)     ## Pulsador como entrada
GPIO.setup(p_verde, GPIO.OUT)       ## Ledverde como salida
GPIO.setup(p_rojo, GPIO.OUT)        ## Ledrojo como salida
GPIO.setup(p_amarillo, GPIO.OUT)    ## Ledamarillo como salida

GPIO.output(p_verde, False)         ## Apago el ledverde
GPIO.output(p_rojo, False)          ## Apago el ledrojo
GPIO.output(p_amarillo, False)      ## Apago el ledamarillo

#############################################################################################

#Variables iniciales

t_ciclo = 2                 ##Tiempo de ciclo
t_ant_ciclo = 0
t_inicial_boton = 0         ##Toma el tiempo en que se pulsa el botón
t_aux_leds = 0
ultimos = list()            ##Lista que contiene las últimas N lecturas
percent_variacion = 0.05    ##5%
n_lecturas = 5              ##Cantidad de lecturas (N)
flag_boton = False          ##Indica el pulsado del boton
flag_leds = False           ##Indica el encendido de todos los leds
led_ant = "nan"
tblink = 0.1                ##Tiempo de destello      

Informe = 'Informe.txt'     ##Nombre del archivo donde se guardan las lecturas

CONEXION = False            ##Indica la conexión de un cliente
cliente = False             ##Socket asociado al cliente
address = False             ##Dirección remota


#___________________________________LECTURA DE TEMPERATURA__________________________________
def leer_temp():
    fSensor = open(Carpeta_Sensor + '/w1_slave','r') ##Se abre el archivo del sensor
    linSensor = fSensor.readlines()         ##Se lee el archivo
    posTemp = linSensor[1].find('t=')       ##En la segunda linea se busca la posición del dato
    strTemp = linSensor[1][posTemp+2:]      ##Se obtiene la posición
    temperatura = int(strTemp)/1000         ##Se convierte a tipo numérico y se aplica la escala
    fSensor.close()
    ultimos.append(temperatura)             ##Guarda la lectura en la lista de temperaturas
    if len(ultimos) > n_lecturas:
        ultimos.pop(0)                      ##El lecturas supera las necesarias elimina la mas vieja
        
#______________________________________MODIFICADOR LEDS_____________________________________
def led_state(val):
    GPIO.output(p_verde, val=="ver" or val=="all")
    GPIO.output(p_rojo, val=="roj" or val=="all")
    GPIO.output(p_amarillo, val=="ama" or val=="all")
    
#_______________________________________PROMEDIO DE TEMPERATURAS____________________________
def sacar_promedio():
    resultado = 0
    for temp in ultimos:
        resultado += temp
    resultado /= len(ultimos)
    return resultado
    
#_______________________________MODIFICADOR DE ESTADO RESPECTO A LA TENDENCIA_______________
def cambiar_estado_leds(prom, temp_actual):
    global led_ant
    desviacion = temp_actual - prom
    if desviacion > prom*percent_variacion:
        ##Caso de tendencia en alta
        led_ant = "roj"
        led_state("roj")
        return 'alta'
    elif desviacion < (-1)*(prom*percent_variacion):
        ##Caso de tendencia en baja
        led_ant = "ver"
        led_state("ver")
        return 'baja'
    else:
        ##Caso de tendencia estable
        led_ant = "ama"
        led_state("ama")
        return 'estable'
        
#_____________________________________GUARDA DATOS EN EL ARCHIVO____________________________
def guardar_datos(temperatura, tend):
    fyh_actual = datetime.now()
    fyh_txt = fyh_actual.strftime('%d/%m/%Y %H:%M:%S')
    with open(Informe,'a') as datos:
        ##Abre el archivo y escribe la información dandole formato
        info = str(fyh_txt) + ' ' + str("{:.3f}".format(temperatura)) + ' tendencia:' + tend + '\n'
        datos.write(info)

#__________________________ENVIA INFORMACION AL CLIENTE CUANDO ESTA CONECTADO__________________
def enviarInfo(message, sock):
    try:
        global CONEXION
        global cliente
        ##Codifica el mensaje
        encodedMessage = message.encode('utf-8')
        ##Envia el mensaje
        sock.sendall(encodedMessage)
    except:
        #Excepcion en sock.sendall() por desconexion
        CONEXION = False
        cliente = False
        print("DESCONEXION")
       
#__________________________ESPERA A LA CONEXIÓN DEL CLIENTE____________________________________
def realizar_conexion():
    #Se ejecuta en un hilo secundario
    global cliente
    global address
    global CONEXION

    while 1:
        if not CONEXION:
            ##El hilo se bloquea en la siguiente llamada hasta que haya un cliente
            cliente, address = server.accept() ##Accepta una nueva conexión
            print(f"Conectado por {address}")  ##Se informa por pantalla 
            CONEXION = True
    
####------------------------########################----------------------####################

try:
    #Se crea e inicia el hilo que maneja las conexiones de los clientes
    hilo1 = Thread(target = realizar_conexion)
    hilo1.start()

    tend = ''
    led_state("all")            ##Antes de definir la tendencia todos los leds encendidos

    print("actual    |   promedio")
    print("______________________")
    
    while 1:
        t_act = time.time()         ##Tiempo actual
        if (t_act - t_ant_ciclo) > t_ciclo and not flag_boton:
            ##Cuando se cumple el tiempo del ciclo y no se apreta el botón

            t_ant_ciclo = t_act     ##Actualiza el tiempo

            if(flag_leds):
                flag_leds = False
                led_state(led_ant)

            leer_temp()             ##Se lee una temperatura y se pone en la lista
            temp_actual = ultimos[len(ultimos) - 1] ##Se obtiene la temperatura de la lista
            data = str(temp_actual) ##Se convierte en un string para enviar la información
            prom = sacar_promedio()

            ##Se muestra por pantalla
            print("{:.3f}".format(temp_actual),"   |  ","{:.3f}".format(prom))

            ##Se envia informacion al cliente
            if CONEXION:
                enviarInfo(data, cliente)
            
            ##Se guarda en el archivo
            if len(ultimos) >= n_lecturas:
                tend = cambiar_estado_leds(prom, temp_actual)
                guardar_datos(temp_actual, tend)

        elif (t_act - t_ant_ciclo) > (t_ciclo - tblink) and len(ultimos) >= n_lecturas and not flag_leds and not flag_boton:
            ##Ocurre tblink antes del tiempo del ciclo cuando no se apreta el boton
            ##y no están prendidos todos los leds
            led_state("all")
            flag_leds = True
       
       ###################################Secuencia para pulsador###########################################
        if not GPIO.input(p_pulsador) and not flag_boton:   ##Cuando se pulsa el boton
            led_state("nan")
            t_inicial_boton = t_act
            flag_boton = True

        elif flag_boton and GPIO.input(p_pulsador): ##Cuando se suelta el boton
            flag_boton = False
            t_ciclo = t_act - t_inicial_boton       ##Se actualiza el tiempo del ciclo

            if t_ciclo < 1:
                enviarInfo('fin', cliente)
                print("Ejecucion finalizada")
                led_state("nan")
                sys.exit()
            elif t_ciclo < 2.5:
                t_ciclo = 2.5
            elif t_ciclo > 10:
                t_ciclo = 10

            print("tiempo de ciclo: ", t_ciclo)

        elif flag_boton:    ##Mientras se apreta el boton maneja el destello de los leds
            if (t_act - t_aux_leds) > (1 + tblink):
                led_state("nan")
                t_aux_leds = t_act
            elif (t_act - t_aux_leds) > 1:
                led_state("all")
                flag_leds = True
    
#############################################################################################
except KeyboardInterrupt:
        pass

finally:
    GPIO.cleanup()