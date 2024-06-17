#define pot A0
#define LED 7
#define conmutacion 512

bool grater;
bool ON = false;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(1200);  
  grater = conmutacion < analogRead(pot);
  if(grater) {digitalWrite(LED, HIGH); ON = true;}
}

void loop() {
  grater = conmutacion < analogRead(pot);
  Serial.print(1);
  if(grater&&!ON){
    digitalWrite(LED, HIGH);
    ON = true;
    Serial.write('E');
  }
  if(!grater&&ON){
    digitalWrite(LED, LOW);
    ON = false;
    Serial.write('A');
  }
  delay(1000);
}
