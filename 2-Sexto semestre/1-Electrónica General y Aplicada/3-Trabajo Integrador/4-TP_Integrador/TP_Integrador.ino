// declaracion de variables
#define pot A0 //el potenciometro estará en la entrada analógica A0
#define pulso 7 // los pulsos serán ingresados por la entrada digital 7
#define LED 4 // el LED funcionará como alarma cuando se sobrepase el límite establecido
unsigned long tiempo_sig=0, tiempo_prev=0, deltat;//tiempo_prev es el tiempo desde que se ejecuta el programa hasta un pulso. tiempo_sig es el tiempo desde que se ejecuta el programa hasta el siguiente pulso
float velocidad;
int previo = LOW, limite=1; //al comenzar el programa, el valor previo comenzará como un bajo, luego cambiará de acuerdo a lo que se lea en el pin 7. La variable limite es la velocidad maxima después de la cual la alarma se enciende (valor inicial = 1)
String comando = "";
bool listo= false;
void setup() {
  Serial.begin(9600); // configura el puerto con una velocidad estandar de 9600 Baudios
  pinMode(pulso, INPUT); 
  pinMode(LED, OUTPUT);
  comando.reserve(4);
}

void loop() {
  int grados;
  int potenciometro = analogRead(pot);
  if (digitalRead(pulso) == HIGH && previo == LOW){ //esto sucede siempre en flancos de subida
   tiempo_prev= tiempo_sig; 
   tiempo_sig = millis(); //millis() mide el tiempo en milisegundos entre que se ejecuta el programa hasta que se llama a la funcion.

  }  previo = digitalRead(pulso); //actualizo el valor previo
  deltat=tiempo_sig - tiempo_prev;//es el tiempo entre dos flancos de subida
  velocidad= 1000.0/(deltat)*3.6; //velocidad en km/h
  if (Serial.available()){
    comando = Serial.readString(); //lee el comando ingresado
    for (int i=0;i<=comando.length();i++){
     char listo = comando[i];
     if (listo == '\r'){ //determina el fin de la comunicación, es decir cuando se pone un enter
      switch (comando[0]){ //dependiendo del primer caracter es la funcion que se ejecutara
      case 'D': //devuelve la posicion angular de la veleta
        grados=direccion(potenciometro);
        Serial.print("D");
        Serial.println(grados);
      break;
      case 'M': //modifica el setpoint de la alarma, de 1 a 100 km/h
        limite= atoi(&comando[1]); //atoi convierte en entero los strings. Ese [1] significa que convierte en entero desde la posicion 1 del String en adelante
        if (limite > 100){ 
          limite=100; //si se ingresa un numero mayor a 100, el limite será igual a 100 independientemente del valor ingresado
        }
        if (limite <1){
          limite =1; //si se ingresa un numero menor a 0, el limite será igual a 1 independientemente del valor ingresado
        }
        Serial.print("M");
        Serial.println(limite); //mensaje eco para verificar que se sobreescribio correctamente la variable
        break;
      case 'm': //printea el limite actual
        Serial.println(limite); 
        break;
      case 'V': //printea la velocidad en km/h
        Serial.print("V");
        Serial.println(velocidad);
        break;
      case 'T': //printea la velocidad en km/h y la posicion angular de la veleta
        Serial.print("T");
        Serial.print(velocidad);
        Serial.print(",");
        grados=direccion(potenciometro);
        Serial.println(grados);
        break;
      } 
     }
     
    }
    
  
   }
   
  if (velocidad >=limite){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
  
}
int direccion(int potenciometro){ //funcion para leer la entrada analógica y transformar a grados
  int grados;
  grados = 360.0/1023.0 * potenciometro; // 1023 pues el conversor es de 10 bits y sigue una relación lineal con la posición angular
  return grados;
}
