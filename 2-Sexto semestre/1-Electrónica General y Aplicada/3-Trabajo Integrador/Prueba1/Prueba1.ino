#define led 13                  //Led conectado al pin 13
#define start_btn 3            //Pulsador de arranque en el pin 2
#define stop_btn 2             //Pulsador de parada en el pin 3

typedef enum {reposo, ciclo}state;      //Enum para el estado del led (0 para reposo y 1 para operación)
volatile state cycle_state = ciclo;     //Por defecto está en operación

unsigned int tcycle = 1000;             //Tiempo del ciclo
unsigned long int tprev;                //Marca de tiempo del instante de pulso anterior
unsigned long int tnow;                 //Marca de tiempo del instante pulso actual

unsigned int counter = 0;               //Contador de pulsos

String query = "";                      //String para la consigna
void setup() {

  /*Como se conectan los pullups internos. Suponiendo que se utilizan pulsadores NA la logica se invierte respecto del ejemplo.
  Definimos que cuando el stop se pone en LOW se detienen las pulsaciones y en esta condición cuando start se pone en LOW las pulsaciones
  se reinician. Si ambos start y stop están en LOW en una ejecución del loop se pasa al estado complementario, es decir, si se estaba en 
  operación se pasa a reposo y viceversa. EN DEFINITIVA EL COMPORTAMIENTO ES SEGÚN LA LÓGICA DE UN T latch*/
  
  pinMode(led, OUTPUT);                 //Se define como salida el pin del LED
  pinMode(start_btn, INPUT_PULLUP);     //Se define como entrada el pin del pulsador de inicio y se activa el pullup interno
  pinMode(stop_btn, INPUT_PULLUP);      //Se define como entrada el pin del pulsador de parada y se activa el pullup interno
  Serial.begin(9600);                   //Se define la velocidad de la comunicacion serial
  digitalWrite(led, HIGH);              //Empezamos con el led encendido
  tprev = millis();                     //Toma el tiempo cuando se inicia el programa
  query.reserve(10);                    //Se reserva un espacio de 10 carácteres para la consigna
  
  attachInterrupt(digitalPinToInterrupt(start_btn), startcycle, FALLING); 
  attachInterrupt(digitalPinToInterrupt(stop_btn), stopcycle, FALLING);   //Atamos interrupciones a los botones de arranque y parada
}


void loop(){
  if (cycle_state == ciclo){
    /*Si se está en la condición de ciclo hay que pulsar,
    actualizar el contador y verificar el pulsador de parada*/
    
    tnow = millis();                        //En este caso se toma la marca de tiempo del instante de pulso
    if ((tnow - tprev) >= tcycle/2){        //Si se cumple un medio ciclo
      digitalWrite(led, !digitalRead(led)); //Se complementa la salida del LED
      counter += digitalRead(led);          //Se actualiza el contador
      tprev = tnow;                         //Se actualiza la marca de tiempo del instante de pulso previo
    }
  }
}
void serialEvent(){
  /*Se lee el buffer de entrada en tanto hayan carácteres en el buffer y en tanto
  no se exceda el tiempo de un semiciclo (millis()-tnow)<tcycle/2 en caso de que se este en ciclo*/
  
  while(Serial.available()&&(!cycle_state || ((millis()-tnow) < tcycle/2))){
    query += (char)Serial.read();               //Se incorpora un carácter a la consigna
    if (query[query.length() - 1]=='\r'){       //Si el último carácter incorporado es el fin de linea
      if (query[0] == 'C'){                     //Se evalúa sí se trata de la consulta de conteo
        Serial.print('C');Serial.print(counter);Serial.print('\r');   //Se replica
      }
      else if(query[0] == 'T'){                 //Se evalúa si se trata de la consigna de tiempo
        tcycle = 2*atoi(&query[1]);             //Se actualiza el tiempo del ciclo
        Serial.print('T');Serial.print(tcycle/2);Serial.print('\r');   //Se replica
      }
      query = "";                               //Se vacía la variable de la consigan para almacenar una nueva
    }
  }
}
void startcycle(){
  //Si esta en reposo y se aprieta el botón de marcha se pone en ciclo
  if (cycle_state == reposo) cycle_state = ciclo;
}
void stopcycle(){
  //Si está en ciclo y se aprieta el botón de reposo se pone en reposo
  if (cycle_state == ciclo) cycle_state = reposo;
}