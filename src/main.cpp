#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OLED_I2C.h>
#include <RTClib.h>


//**************************************
//**************Pines*******************
//**************************************

#define bombaGeneral 15
#define bombaLavado 2
#define macerador 14
#define temperatura 23
#define eco 19
#define trig 18
#define caudalSalida 5
#define caudalEntrada 4
#define SCL 22
#define SDA 21
#define valvulaRefrigeracion 12
#define valvulaDeclorada 13





//**************************************
//******SENSORES DE TEMPERATURA*********
//**************************************


OneWire ow(temperatura);
DallasTemperature sensorT(&ow);


//**************************************
//*********** OLED *********************
//**************************************

OLED  oled(SDA, SCL); // Remember to add the RESET pin if your display requires it...
extern uint8_t SmallFont[];


//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "192.168.1.250";
const int mqtt_port = 1883;
const char *mqtt_user = "guille";
const char *mqtt_pass = "Elmundo21";
const char *root_topic_subscribe = "cerveceria/operaciones/#";



//**************************************
//*********** WIFICONFIG ***************
//**************************************
char* ssid = "Cecilia Lorena 2.4g";
 char* password =  "55858760";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
String m;
int tiempo[3];
String operacion, instruccion;
int temperaturaPrecalentado=0;

bool etapa1=true,etapa2=false,etapa3=false,etapa4=false,inicioMaceracion=false, cronometrar=false,maceracionExtendida=true;
bool primerLupulo=true, segundoLupulo=true, tercerLupulo =true,inicioCoccion=false;
long timerMaceracion,tiempoMaceracionTotal,tiempoCoccion;

//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();
void parada();
double getTemp(int sensor);
String cronometro(long contador);
void setup() {
  ////////////////////////////////////////////////////////////Inicialización de comunicaciones IOT///////////////////////////////////////////////////////////////
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //////////////////////////////////////////////////////////inicialización de Oled////////////////////////////////////////////////////////////////////////////////

   if(!oled.begin(SSD1306_128X64))
  {
    Serial.println("Failed memory allocation...");
    while(1);   // In case the library failed to allocate enough RAM for the display buffer...
  }
    oled.setFont(SmallFont);



  ////////////////////////////////////////////////////////////Inicialización de sensores de temperatura//////////////////////////////////////////////////////////
   sensorT.begin();
 ////////////////////////////////////////////////////////////Inicialización pines//////////////////////////////////////////////////////////////////////////////
   pinMode(bombaGeneral,OUTPUT);
   pinMode(bombaLavado,OUTPUT);
   pinMode(macerador,OUTPUT);
   pinMode(temperatura,INPUT);
   digitalWrite(bombaLavado,HIGH);
   digitalWrite(bombaGeneral,HIGH);
   digitalWrite(macerador,LOW);

}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }

  if (client.connected()){
      
   
//   if (instruccion=="CANCELAR")
//  {
//      parada();
//  }  
 
//   else if (operacion=="cerveceria/operaciones/precalentado")
//  {
//     temperaturaPrecalentado=instruccion.toInt();
//     double temp= getTemp(0);
//     String tObjetivo="T objetivo: "+String(temperaturaPrecalentado)+ " C";
//     String tActual="T Actual: "+String(temp)+ " C";
//     oled.clrScr();
//     oled.print("CERVECERIA ARCANUM", CENTER,0);
//     oled.print("PRECALENTANDO",CENTER,16);
//     oled.print(tObjetivo,LEFT,32);
//     oled.print(tActual,LEFT,48);
//     oled.update();
//      char t[4];
//       dtostrf(temp, 2, 0, t);
//     client.publish("cerveceria/mediciones/sensorOlla",t);

//      if(temp>=temperaturaPrecalentado){
//       String men= "La temperatura de precalentado "+ (String)temperaturaPrecalentado+ " grados celcius se alcanzó correctamente. Por favor Apaque el fuego";
//       char msg[110];
//       men.toCharArray(msg,110);
//       client.publish("cerveceria/mediciones/temperatura/mensaje",msg);
//       oled.clrScr();  
//       oled.print("CERVECERIA ARCANUM", CENTER,0);
//       oled.print("FIN PRECALENTANDO",CENTER,32);
//       oled.update();
//       delay(10000);
//       instruccion="CANCELAR";
//       operacion="";
//   }
//  }
 

//  else if (operacion=="cerveceria/operaciones/maceracion")
//  {
//   double temp;

//    /*Para la maceración escalanada en la primer fase se usarán los siguientes presets (los que deberán modificarse de forma dinámica cuando el sistema final
//     web esté activo):
    
//     Escalon 1 (fitasa, beta-glucanasa, ) 40°C 15 minutos

//     Escalon 2 ( proteasa, peptidasa ) 50°C 20 minutos

//     Escalon 3 (alfa-glucosidasa, dextrinasa, beta-amilasa, alfa-amilasa) 62°C 15 minutos

//     Escalon 4 (alfa-amilasa (optimizado)) 67° C 60 minutos
//     */

//     //medición de la temperatura inicial
     
//       temp=getTemp(1);

//       //Transmisión mqtt
//       char t[4];
//       dtostrf(temp, 2, 0, t);
//       client.publish("ceveceria/mediciones/temperatura/maceracion",t);
//       //Impresión por pantalla
//       oled.clrScr();
//       m="Temp: "+String(temp)+" C | "+cronometro(tiempoMaceracionTotal);
//       oled.print(m,LEFT,16);
//       oled.update();
      
     
//       /////////////////////////Primer etapa de maceración//////////////////////////////////////////////
//       if(etapa1){

         
//           oled.print("MACERANDO Etapa: 1/4",CENTER,0);
//           if (inicioMaceracion)
//           {
//           inicioMaceracion=false;
//           client.publish("ceveceria/mediciones/Maceracion/t1","true");
        
          
//           }
          
//           if (temp>=41 )
//     {
//               //Si la temperatura es mayor a la de inicio de maceración solo restará esperar a que ésta baje a la t° necesaria con la resistencia apagada
//             digitalWrite(macerador,LOW);
              
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: OFF",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","OFF");
//               oled.update();
//     }     else if(temp<=39){
//               // Si la temperatra es menor, comenzará a cronometrarse la maceración, y se prenderá la resistencia.
//               digitalWrite(macerador,HIGH);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: ON",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","ON");
//               oled.update();
//     }     else if (temp>39 && temp<41 && cronometrar==false){
//               cronometrar=true;
//                client.publish("ceveceria/mediciones/Maceracion","MACERACION FITÁSICA");
              
//            client.publish("ceveceria/mediciones/Maceracion/t2","true");
             
//     }     if (cronometrar)
//     {
//               String impresion= "T Proceso: "+ cronometro(timerMaceracion);
//               oled.print(impresion,LEFT,32);
//               oled.update();
             
//               if (tiempoMaceracionTotal>=900)//Equivalente a 15 minutos en segundos
//               {
//                etapa1=false;
//                etapa2=true;
//                cronometrar=false;
//                timerMaceracion=0;
//                client.publish("ceveceria/mediciones/Maceracion","CALENTANDO HASTA MACERACIÓN PROTEICA");
              
//            client.publish("ceveceria/mediciones/Maceracion/t3","true");
//               }
              
//     }
//       }



//        /////////////////////////Segunda etapa de maceración//////////////////////////////////////////////
//       if(etapa2){

//           oled.clrScr();
//           oled.print("MACERANDO Etapa: 2/4",CENTER,0);
//           if (temp>=51 )
//     {
//               //Si la temperatura es mayor a la de inicio de maceración solo restará esperar a que ésta baje a la t° necesaria con la resistencia apagada
//               digitalWrite(macerador,LOW);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: OFF",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","OFF");
//               oled.update();
//     }     else if(temp<=49){
//               // Si la temperatra es menor, comenzará a cronometrarse la maceración, y se prenderá la resistencia.
//               digitalWrite(macerador,HIGH);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: ON",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","ON");
//               oled.update();
//     }     else if (temp>49 && temp<51 && cronometrar==false){
//               cronometrar=true;
//               client.publish("ceveceria/mediciones/Maceracion"," MACERACIÓN PROTEICA");
//               client.publish("ceveceria/mediciones/Maceracion/t4","true");
//     }     if (cronometrar)
//     {
//               String impresion= "T Proceso: "+ cronometro(timerMaceracion);
//               oled.print(impresion,LEFT,32);
//               oled.update();
//               if (tiempoMaceracionTotal>=2100)//Equivalente a 20 minutos en segundos + tiempo anterior
//               {
//                etapa2=false;
//                etapa3=true;
//                cronometrar=false;
//                timerMaceracion=0;
//                client.publish("ceveceria/mediciones/Maceracion","CALENTANDO HASTA MACERACIÓN de SACARIFICACIÓN");
               
//                client.publish("ceveceria/mediciones/Maceracion/t5","true");
//               }
              
//     }
//       }


//        /////////////////////////Tercera etapa de maceración//////////////////////////////////////////////
//       if(etapa3){

         
//           oled.clrScr();
//           oled.print("MACERANDO Etapa: 3/4",CENTER,0);
//           oled.update();
//           if (temp>=63 )
//     {
//               //Si la temperatura es mayor a la de inicio de maceración solo restará esperar a que ésta baje a la t° necesaria con la resistencia apagada
//             digitalWrite(macerador,LOW);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: OFF",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","OFF");
//               oled.update();
//     }     else if(temp<=61){
//               // Si la temperatra es menor, comenzará a cronometrarse la maceración, y se prenderá la resistencia.
//              digitalWrite(macerador,HIGH);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: ON",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","ON");
//               oled.update();
//     }     else if (temp>61 && temp<63 &&cronometrar==false){
//               cronometrar=true;
//               client.publish("ceveceria/mediciones/Maceracion","MACERACIÓN DE SACARIFICACIÓN");
//               client.publish("ceveceria/mediciones/Maceracion/t6","true");
//     }     if (cronometrar)
//     {         
//               String impresion= "T Proceso: "+ cronometro(timerMaceracion);
//               oled.print(impresion,LEFT,32);
//               oled.update();
//               if (tiempoMaceracionTotal>=3000)//Equivalente a 15 minutos en segundos + tiempo anterior
//               {
//                etapa3=false;
//                etapa4=true;
//                cronometrar=false;
//                timerMaceracion=0;
//                client.publish("ceveceria/mediciones/Maceracion","CALENTANDO HASTA MACERACIÓN DE SACARIFICACIÓN CENTRAL");
//                client.publish("ceveceria/mediciones/Maceracion/t7","true");
//               }
              
//     }
//       }



// /////////////////////////Cuarta etapa de maceración//////////////////////////////////////////////
//       if(etapa4){
//           oled.clrScr();
//           oled.print("MACERANDO Etapa: 3/4",CENTER,0);
//           oled.update();
//           if (temp>=69 )
//     {
//               //Si la temperatura es mayor a la de inicio de maceración solo restará esperar a que ésta baje a la t° necesaria con la resistencia apagada
//             digitalWrite(macerador,LOW);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: OFF",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","OFF");
//               oled.update();
//     }     else if(temp<=67){
//               // Si la temperatra es menor, comenzará a cronometrarse la maceración, y se prenderá la resistencia.
//               digitalWrite(macerador,HIGH);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: ON",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","ON");
//               oled.update();
//     }     else if (temp>67 && temp<69 &&cronometrar==false){
//               cronometrar=true;
//               client.publish("ceveceria/mediciones/Maceracion"," MACERACIÓN DE SACARIFICACIÓN CENTRAL");
//               client.publish("ceveceria/mediciones/Maceracion/t8","true");
             
//     }     if (cronometrar)
//     {
//               String impresion= "T Proceso: "+ cronometro(timerMaceracion);
              
//               oled.print(impresion,LEFT,32);
//               oled.update();
//               if (tiempoMaceracionTotal>=6600)//Equivalente a 15 minutos en segundos + tiempo anterior
//               {
//               etapa4=false;
//               cronometrar=false;
//               timerMaceracion=0;
//               client.publish("ceveceria/mediciones/Maceracion","FIN DE MACERACIÓN");
//               client.publish("ceveceria/mediciones/Maceracion/t9","true");
//               instruccion="CANCELAR";
//               oled.clrScr();
//                oled.print("FIN DE MACERACIÓN",CENTER,32);
//                 oled.update();
//                delay(10000);
//               }
              
//               }
              
//     }
//       }
   
    

//  }

//  else if (operacion=="cerveceria/operaciones/maceracionExtendida")
//  {
//   double temp=getTemp(1);

//       //Transmisión mqtt
//       char t[4];
//       dtostrf(temp, 2, 0, t);
//       client.publish("ceveceria/mediciones/temperatura/maceracion",t);
//       //Impresión por pantalla
//       oled.clrScr();
//       oled.print("MACERACIÓN EXTENDIDA",CENTER,0);
//       m="Temp: "+String(temp)+" C";
//       oled.print(m,LEFT,16);
//        oled.update();
      
//       if (maceracionExtendida)
//       {
//         maceracionExtendida=false;
//         client.publish("ceveceria/mediciones/Maceracion/t10","true");
//       }
      
//        if (temp>=69 )
//     {
//               //Si la temperatura es mayor a la de inicio de maceración solo restará esperar a que ésta baje a la t° necesaria con la resistencia apagada
//              digitalWrite(macerador,LOW);
//               oled.print("Iniciando",CENTER,32);
//               oled.print("Resistencia: OFF",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","OFF");
//                oled.update();
//     }     else if(temp<=67){
//               // Si la temperatra es menor, comenzará a cronometrarse la maceración, y se prenderá la resistencia.
//               digitalWrite(macerador,HIGH);
//               oled.print("Resistencia: ON",CENTER,48);
//               client.publish("ceveceria/mediciones/estado/resistencia","ON");
//                oled.update();
//     }
//              String impresion= "T: "+ cronometro(timerMaceracion)+"|"+ cronometro(tiempoMaceracionTotal);
//               oled.print(impresion,LEFT,32);
//                oled.update();

//         if (instruccion=="maceracionExtendidaOff")
//         {
           
//                client.publish("ceveceria/mediciones/Maceracion/t11","true");
//                instruccion="CANCELAR";
//         }
        
//                  }
                 
              

 
 






//  else if (operacion=="cerveceria/operaciones/recircular")
//  {
   
//    digitalWrite(bombaGeneral,HIGH);
//    oled.clrScr();
//    oled.print("CERVECERIA ARCANUM", CENTER,0);
//    oled.print("RECIRCULANDO",CENTER,16);
//     oled.update();
        
// }
 
//  else if (operacion=="cerveceria/operaciones/lavar")
//  {
//      digitalWrite(bombaLavado,LOW);
//      oled.clrScr();
//      oled.print("CERVECERIA ARCANUM", CENTER,0);
//      oled.print("LAVANDO",CENTER,32);
//       oled.update();
    
//  }
//  else if (operacion=="cerveceria/operaciones/trasiegoOlla")
//  {
//      digitalWrite(bombaGeneral,LOW);
//      oled.clrScr();
//      oled.print("CERVECERIA ARCANUM", CENTER,0);
//      oled.print("TRASIEGO A OLLA",CENTER,32);
//       oled.update();
     
//  }
// else if (operacion=="cerveceria/operaciones/trasiegoMacerador")
//  {
//      digitalWrite(bombaGeneral,LOW);
//      oled.clrScr();
//      oled.print("CERVECERIA ARCANUM", CENTER,0);
//      oled.print("TRASIEGO A MACERADOR",CENTER,32);
//       oled.update();
//  }



//  else if (operacion=="cerveceria/operaciones/coccion")
//  {
//    double temp=getTemp(0);
//     oled.clrScr();
//    m="Temp: "+(String)temp +" C";
//     oled.print("COCCIÓN",CENTER,0);
//     oled.print(m,CENTER,16);
//     char t[4];
//     dtostrf(temp, 2, 0, t);
//     client.publish("ceveceria/mediciones/temperatura/coccion",t);
//     oled.update();

//     if ((temp>98 && primerLupulo)){
        
//     String mensaje="Primer Lúpulo";
//     char m[100];
//     mensaje.toCharArray(m,100);
//     client.publish("ceveceria/mediciones/coccion/mensajeAgregadoLupulo1",m);
//     oled.print("HOP 1-> 0",CENTER,32);
//     primerLupulo=false; 
//     inicioCoccion=true;
//     oled.update();
    
//   }
//    if (inicioCoccion)
//   {
//     String t= cronometro(tiempoCoccion);
//     oled.print(t,CENTER,32);
//     oled.update();
  
//   }

//   if (tiempoCoccion>1800 && segundoLupulo)
//   {
//   segundoLupulo=false;
//      client.publish("ceveceria/mediciones/coccion/mensajeAgregadoLupulo2","true");
//      oled.print("HOP 1-> 0|HOP2->30",CENTER,32);
//      segundoLupulo=false;
//      oled.update();

//   }
//   if (tiempoCoccion>3300 && tercerLupulo)
//   {
  
//      client.publish("ceveceria/mediciones/coccion/mensajeAgregadoLupulo3","true");
//      oled.print("HOP 1-> 0|HOP2->30|HOP3->55",CENTER,32);
//      tercerLupulo=false;
//      oled.update();

//   }

//    if (tiempoCoccion>3600)
//   {
//     oled.clrScr();
//      client.publish("ceveceria/mediciones/coccion/mensajeFinCoccion","true");
//      instruccion="CANCELAR";
//      oled.print("FIN DE COCCIÓN",CENTER,32);
//      oled.update();
//      delay(10000);

//   }
  
 
  
//  }

 
 
// else{
//        parada();
      
 }

  client.loop();
   delay(1000);
   
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "ARCANUM";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  operacion=topic;
  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);
  instruccion=incoming;





}


//////////////////////////////////////////////////////////////////////Definición de funciones//////////////////////////////////////////////////////////////////////////////////////////////////
void parada(){
  digitalWrite(macerador,LOW);
  digitalWrite(bombaGeneral,HIGH);
  digitalWrite(bombaLavado,HIGH);
}


double getTemp(int sensor){
 sensorT.requestTemperatures();
  if (sensor==0)
  {
    Serial.print("Temp Olla ->  ");
    Serial.println(sensorT.getTempCByIndex(0));

  }

  if (sensor==1)
  {

  Serial.print("Temp Macerador-> ");
  Serial.println(sensorT.getTempCByIndex(1));

  }
  Serial.println("------------------------------------------------------------------------------------------------------------");

  return sensorT.getTempCByIndex(sensor);

}


String cronometro(long contador){
 
contador++;
 int horas = (contador / 3600);
 int minutos = ((contador-horas*3600)/60);
 int segundos = contador-(horas*3600+minutos*60);
 String tiempo= (String)horas+":"+(String)minutos+":"+(String)segundos;
    return tiempo;
}

