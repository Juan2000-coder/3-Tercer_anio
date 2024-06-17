#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct{
  float temp;
  char hora[10];
  int rep;
}Nodo;

typedef struct lecturas{
  Nodo datos;
  struct lecturas *sig;
}Lectura;


Lectura *creanodo(void) {
  return (Lectura*)malloc(sizeof(Lectura));
}

Lectura *ordenar_nodo(Lectura *l, Lectura *q, float lect){    //Ordenamos la lista según temperaturas
  Lectura *nodo_act=l, *nodo_sig=l;
  //Debe asignar el lugar correcto según la temperatura de lectura
  while((nodo_sig != NULL)&&(lect > (nodo_sig->datos.temp))){
    nodo_act = nodo_sig;
    nodo_sig = nodo_sig->sig;
    }
	//Se enlaza el nodo en una posición intermedia o al final de la lista
	if (nodo_act != nodo_sig){      //Si nodo_act es diferente a nodo_sig de la lista (temp menor)
		nodo_act-> sig = q;    //El siguiente de nodo_act tiene que ser q
   	q->sig = nodo_sig;     //El siguiente de     q    tiene que ser nodo_sig
  }

	else{//Se enlaza el nodo al principio de la lista  (temp menor)
		q->sig = l;
    l = q;
  }
  return l;
}

int lec_repetida(Lectura *l, float lect){    //Corrobora si la lectura ya está en la lista
  Lectura *nodo_apunt = l;
  int rep=1;
  while((nodo_apunt != NULL)){    //Corrobora que la temp no esté en la lista
    if(lect == nodo_apunt->datos.temp){
      rep++;
    }
    nodo_apunt = nodo_apunt->sig;
  }
  return rep;
}

Lectura *agregar(Lectura *l, float tempFloat,char horaChar[10]){
  Lectura *p;
  Nodo x;
  int i=1;
  p = creanodo();

  x.temp = tempFloat;
  strcpy(x.hora,horaChar);
  x.rep = 1;
  
  x.rep = lec_repetida(l, tempFloat);
  p-> datos = x;
  l = ordenar_nodo(l,p,tempFloat);
  return l;
}

void imprimelista(Lectura *l){                          //OPCIONAL, SE PUEDE SACAR
  Lectura *p;
  p = l;
  if (p == NULL){
    printf("  Error, la lista se encuentra vacía!\n\n");
  }
  while (p != NULL){
    printf("  Temperatura:  %f  , Hora:", p->datos.temp);
    printf("  %s  ,  Repeticiones:", p->datos.hora);
    printf("  %d\n", p->datos.rep);
    p = p->sig;
  }
  printf("\n");
}

int main(void) {
  int lecturas=0, i, cent;
  Lectura *l, *apunt1, *apunt2;
  Nodo x;
  FILE *Fd;          //Fd: File descriptor
  char archivo[]="Informe.txt";      //archivo: contiene la ruta de acceso.
  float min=10000, max=-10000, rep=0, tempFloat, sumtemp=0, media, mediana, moda, desv=0, suma=0;
  char linea[100], temp[27], minima[30], maxima[30], tempChar[9], horaChar[10];


  Fd=fopen(archivo,"r");           //fopen: abrir archivo
  
  /*El programa debe leer la cantidad de lineas = cantidad de mediciones tomadas*/
  while(fgets(linea,100,Fd)>0){    //Mientras traiga un string >0, no nulo, hace...
    lecturas=lecturas+1;
    
    i=0;
    while(i<26){            //Este while se encarga de separar la med de los otros datos
      temp[i]=linea[i];  //tempChar va a tener el dato numerico de la medicion
      i=i+1;
    }
    temp[26]='\0';
    
    i=20;
    while(i<26){            //Este while se encarga de separar la med de los otros datos
      tempChar[i-20]=temp[i];  //tempChar va a tener el dato numerico de la medicion
      i=i+1;
    }
    
    i=11;
    while(i<19){            //Este while se encarga de separar la hora de los otros datos
      horaChar[i-11]=temp[i];
      i=i+1;
    }
    horaChar[8]='\0';
    
    tempFloat=atof(tempChar);   //Pasamos de tener un char a tener un float
    l = agregar(l,tempFloat,horaChar);    //Agregamos los datos como un nuevo nodo
    
    if(tempFloat<min){
      strcpy(minima,temp);
      min=tempFloat;
    }
    if(tempFloat>max){
      strcpy(maxima,temp);
      max=tempFloat;
    }

    /* Media: suma de los datos dividido entre el total */
    sumtemp=sumtemp+tempFloat;
    
    /* Mediana: dato que está en el centro de todos los registrados */
    
    /* Moda: dato que más se repite */

  }
  /**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**//**/

  printf("\n        -------------------------------------------------------------------------------------\n");
  printf("        |          LOS DATOS OBTENIDOS DEL ANÁLISIS ESTADÍSTICO SON LOS SIGUIENTES          |\n");
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |    LECTURAS OBTENIDAS                                                             |\n");
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |      La estación realizó %d lecturas                                              |\n",lecturas);
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |    TEMPERATURAS LÍMITES OBSERVADAS                                                |\n");
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |        La mínima temperatura leída fue:     %sºC          |\n",minima);
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |        La máxima temperatura leída fue:     %sºC          |\n",maxima);
  printf("        |-----------------------------------------------------------------------------------|\n");

  media=sumtemp/lecturas;
  printf("        |    ESTADÍSTICOS                                                                   |\n");
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |        La media resultó ser de:    %.3fºC                                       |\n",media);
  printf("        |-----------------------------------------------------------------------------------|\n");
  
  /* Mediana: dato que está en el centro de todos los registrados */
  /* como los datos ya están ordenados, sabemos que la mediana va a depender de si tenemos un número par o impar de lecturas */
  
  apunt1 = l;
  apunt2 = l->sig;
  if(lecturas%2==0){      //Si es par, la mediana será la media entre los dos valores centrales de la lista
    cent=lecturas/2;
    for(int i=1;i<cent;i++){        //Ejemplo:
      apunt1 = apunt1->sig;          //  lecturas=10, mediana = (media entre medicion de puesto 5 y 6 de la lista)
      apunt2 = apunt2->sig;          //  considerando que arrancamos (i en 1) == (posición=0 de la lista) 
      mediana=(apunt1->datos.temp + apunt2->datos.temp)/2;
    }
  }else{                  //Si es impar, la mediana será el valor central de la lista
    cent = lecturas/2;
    for(int i=1;i<=cent;i++){        //Ejemplo:
      apunt1 = apunt1->sig;          //  lecturas=10, mediana = ( medicion de puesto 5 de la lista)
                                     //  considerando que arrancamos (i en 1) == (posición=0 de la lista)                 
      mediana=apunt1->datos.temp;
    }
  }

  printf("        |        La mediana resultó ser de:  %.3fºC                                       |\n",mediana);
  printf("        |-----------------------------------------------------------------------------------|\n");
  
  apunt1 = l;
  for(int i = 0;i<lecturas;i++){
    if((apunt1->datos.rep)>=rep){
      rep=apunt1->datos.rep;
      moda=apunt1->datos.temp;
    }
    apunt1 = apunt1->sig;
  }
  if(rep==1){
    printf("        |        No hay moda, ningún valor se repite                                        |\n");
    printf("        |-----------------------------------------------------------------------------------|\n");
  }else{
    printf("        |        La moda resultó ser de:     %.3fºC                                       |\n",moda);
    printf("        |-----------------------------------------------------------------------------------|\n");
  }

  apunt1 = l;
  for(int i = 0;i<lecturas;i++){
    suma = suma + pow(((apunt1->datos.temp) - media),2);
    apunt1 = apunt1->sig;
  }
  desv=sqrt(suma/(lecturas-1));

  printf("        |    MEDIDA DE DISPERSIÓN                                                           |\n");
  printf("        |-----------------------------------------------------------------------------------|\n");
  printf("        |        La desviación estándar resultó ser de: %.5f                             |\n",desv);
  printf("        -------------------------------------------------------------------------------------\n");
  
  fclose(Fd);                      //fclose: cerrar archivo

  /*Se libera la memoria asignada*/
	while(l != NULL){
		apunt1 = l;
		free(apunt1);
		l = l->sig;
	}
}