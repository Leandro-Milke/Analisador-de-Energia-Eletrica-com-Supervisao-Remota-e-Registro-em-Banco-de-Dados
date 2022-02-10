//Analisador de energia TCC
// Final vs 6 com determinação do fator de potencia pela potencia instantanea e correção do regstro de eventos e config rede wifi por C#

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <WiFi.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "String.h"
#include "driver/uart.h"
#include "SPIFFS.h"


//funções
void CarregaParametros(); // dentro do setup
void aquisicao_dados();
void CalculoRMS();
void AtualizaLeitura();
void Regitra_Eventos();
void Registra_MaxMin();
void ConfigRede();      // funçao para gravar no arquio os dados para conexão na rede
void carregaConfigRede();  // função chamada no inicio para caregar as configuraçoes para conexao na rede.

// variaveis para conexão wiifi
char ssid[15]= ""; 
char password[15]="";
char host[15] = "";
int ip1,ip2,ip3,ip4;
int ipS1,ipS2,ipS3,ipS4;
IPAddress ip(ip1,ip2,ip3,ip4);
IPAddress gat(192,168,1,1);
IPAddress sub(255,255,0,0);
IPAddress priDNS(8,8,8,8);
IPAddress secDNS(8,8,4,4);
File file;
WiFiServer server(80);
WiFiClient client;

// entradas analogicas
const int tensao = 34;   // para aquisição da amostra de tensão
const int corrente = 35; // para aquisição da amostra de corrente

const int amostra = 360; // quantidade de amostras

float media_tensao [60], media_corrente [60],  media_FP[60];  // para calcular a média das grandezas, para descarte de valores incoerentes
float media_tensaoRMS, media_correnteRMS;                     // valoes que serão amostrados e utilizados nos calculos das outras grandezas
float ProtecaoTensaoAlta, ProtecaoTensaoBaixa, ProtecaoCorrente, ProtecaoVAAlta, ProtecaoWAlta, Tensao_Nominal; // variaveis contendo o valor configurado para sinalizar a anormalidade
int StatusTensao = 0; // 1 para nível adequado, 2 crítico e 3 precário 
int ciclo = 0; // para controlar 60 ciclos para fechar 1 sesgundo a atualização das grandezas
long hora = 0; // para controlar 1 hora para enviar os dados para regstro no banco de dados.

// variaveis utilizadas para ver se a tensão ou corrente é zero.
int contZerosVolt = 0;
int contZerosCor = 0;
float anteriorTensao = 1.0;
float anteriorCorrente = 1.0;

// varaveis utilizadas para achar o valor médio, para excluir a tensão DC do circuto
double accTensao, accCorrente ;
float medTensao, medCorrente;

int amostras_tensao[amostra];     // Variável criada para armazenar as amostras lidas
int amostras_corrente[amostra];
float PotInst[amostra];
float PotInstAcc;
float Media_PotAtiva[60];
float Fc_W = 2.265 * 0.86;               // fator de correção de fase para calcular a potencia instantânea

float tensaoPROTO[amostra];       // Utilizada para armazenar os valores correspondentes a tensão a tensão real
float correntePROTO[amostra];
float tensaoREAL[amostra];        // Valores reais de tensão
float correnteREAL[amostra];
float soma_tensao = 0;
float soma_corrente = 0;
float tensaoRMS;
float correnteRMS;

float Potencia_Ativa, Potencia_Aparente, Potencia_Reativa, FP;
float Par_sobretensao, Par_subtensao, Par_sobrecorrente, Par_W_alta;  // parâmetros que irá disparar a proteção
float tensao_max, tensao_minima, corrente_maxima, VA_maximo, VA_minimo, W_maximo, W_minimo, Kwh; // variaveis para guardar os máximos e mínimos

int SincHora = 0;
long millisAnterior;
long tempoKwh;

int i; // utilizados nos for

int flag_evento = 0; // usado no eno dos eventos de anormalidades nas função de regstro de eventos e na atualização dos medições.

String teste = "";

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector queda de tensão
  Serial.begin(115200);
  delay(10000);

  int cont;
  teste ="";

  if (!SPIFFS.begin(true)) {
    Serial.println("Erro abertura arquivo SPIFFS");
    return;
  }
  delay(5000);
  carregaConfigRede();
  
  IPAddress ip(ip1,ip2,ip3,ip4);
  WiFi.config(ip,gat,sub,priDNS,secDNS);
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s e %s", ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    while(Serial.available()>0)                //se algum dado disponível
     {
        if(Serial.read()=='C')ConfigRede();   //le o byte disponivel e se for a letra C do software de configuração, chama a rotina para configurar os parametros da rede       
     }   
  } 
  
  Serial.println(" connected");
  Serial.print(WiFi.localIP());

  server.begin();
  Serial.println("Servidor Iniciado");

  tensao_max = 0.0;
  corrente_maxima = 0.0;
  VA_maximo = 0.0;
  W_maximo = 0.0;
  tensao_minima = 999.99;
  VA_minimo = 999.99;
  W_minimo = 999.99;
  Kwh = 0.0;

  CarregaParametros();
  millisAnterior = millis();  // para contar a hora aproximado
  tempoKwh = millisAnterior;  // para saber o tempo exato no incremento do kwh

}

void loop() {

  Potencia_Ativa = 0;
  Potencia_Aparente = 0;
  soma_tensao = 0;
  soma_corrente = 0;
  accTensao = 0;
  accCorrente = 0;
  contZerosVolt = 0;
  contZerosCor = 0;

  aquisicao_dados();
  CalculoRMS();
  
  if((millis()- millisAnterior) >= 60000){   //se for percorrido 1 min
    SincHora ++;
    millisAnterior = millis();
    Serial.println(SincHora);
  }
  if(SincHora >= 60){   // se fechou 1 hora regstra no BC os máximos e mínimos
     Registra_MaxMin();
     SincHora = 0;
  }
  
  ciclo++;
  if (ciclo >= 60)AtualizaLeitura();
  
  while(Serial.available()>0)                //se algum dado disponível
     {
        if(Serial.read()=='C')ConfigRede();   //le o byte disponivel e se for a letra C do software de configuração, chama a rotina para configurar os parametros da rede       
     }  
}


void CarregaParametros() { // carregar os parametros de proteção e a hora
 // Serial.println("carregando parametros");
  // conexão com o servidor para carregar os parâmetros de proteção
  if (client.connect(host, 8080))
  {
    Serial.println("connected");
    Serial.println("[Sending a request] carregar os parametros");
    client.print(String("GET /TCC/CarregaParametrosESP32.php") + " HTTP/1.1\r\n" +    // conexão com a pagina no servidor
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        teste = client.readStringUntil('\n');
        Serial.println(teste);
      }
    }
    String aux = "";
    if (teste.charAt(0) == 'a') {
      for (i = 1; teste.charAt(i) != 'b'; i++)aux += teste.charAt(i);
      ProtecaoTensaoAlta = aux.toFloat();
      i = teste.indexOf('b');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'c'; i++)aux += teste.charAt(i);
      ProtecaoTensaoBaixa = aux.toFloat();
      i = teste.indexOf('c');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'd'; i++)aux += teste.charAt(i);
      ProtecaoCorrente = aux.toFloat();
      i = teste.indexOf('d');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'e'; i++)aux += teste.charAt(i);
      ProtecaoVAAlta = aux.toFloat();
      i = teste.indexOf('e');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'f'; i++)aux += teste.charAt(i);
      ProtecaoWAlta = aux.toFloat();
      i = teste.indexOf('f');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'g'; i++)aux += teste.charAt(i);
      SincHora = aux.toInt();
      if(SincHora >= 59)SincHora = 0;
      else SincHora + 1;
      i = teste.indexOf('g');
      aux = "";
      for (i = i + 1; teste.charAt(i) != 'h'; i++)aux += teste.charAt(i);
      Tensao_Nominal = aux.toInt();
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else
  {
    Serial.println("connection failed!");
    client.stop();
  }
     millisAnterior = millis();
  return;
}

void aquisicao_dados() {
  for (i = 0; i < amostra; i++)
  {
    amostras_tensao[i] = analogRead(tensao);
    amostras_corrente[i] = analogRead(corrente);
    accTensao += amostras_tensao[i];            // sera usado para zerar a componente DC
    accCorrente += amostras_corrente[i];
    delayMicroseconds(26.00);                   // tempo para fechar o periodo do ciclo.
  }

  medTensao = (float)(accTensao / amostra);      // para retrar a componente DC do circuito
  medCorrente = (float)(accCorrente / amostra);
  return;
}

void CalculoRMS() {
  anteriorTensao = 1.0;
  anteriorCorrente = 1.0;

  for (i = 0; i < amostra; i++)
  {
    tensaoPROTO[i] = ((float)amostras_tensao[i] - medTensao);
    correntePROTO[i] = ((float)amostras_corrente[i] - medCorrente);

    if ( i < 100) {
      if ((tensaoPROTO[i] * anteriorTensao) < 0.0) contZerosVolt++;
      if ((correntePROTO[i] * anteriorCorrente) < 0.0) contZerosCor++;
    };

    anteriorTensao = tensaoPROTO[i];
    anteriorCorrente = correntePROTO[i];

    tensaoREAL[i] = tensaoPROTO[i] * 0.654;
    correnteREAL[i] = correntePROTO[i] * 0.238;

    PotInst[i] = tensaoREAL[i] * correnteREAL[i];

    tensaoREAL[i] = tensaoREAL[i] * tensaoREAL[i];
    correnteREAL[i] = correnteREAL[i] * correnteREAL[i];

    soma_tensao += tensaoREAL[i];
    soma_corrente += correnteREAL[i];
    PotInstAcc += PotInst[i];
    
  }
  if ( contZerosVolt > 25) tensaoRMS = 0.0;
  else {
    tensaoRMS = soma_tensao / amostra;
    tensaoRMS = (sqrt(tensaoRMS));
  }
  if ( contZerosCor > 50) {
    correnteRMS = 0.0;
  }
  else {
    correnteRMS = soma_corrente / amostra;
    correnteRMS = (sqrt(correnteRMS));
  }
  
  Media_PotAtiva[ciclo] = PotInstAcc / amostra;
  PotInstAcc = 0.0;
  media_tensao[ciclo] = tensaoRMS;
  media_corrente[ciclo] = correnteRMS;
  
  return;
}


void AtualizaLeitura() {
  //Serial.println("atualizando dados");
  static int contZerosTensao = 0;
  static int contZerosCorrente = 0;
  static float media = 0;

  FP = 0 ;
  for (i = 0; i < 60; i++) {
   
    if (media_tensao[i] == 0.000) contZerosTensao++;
    if (media_corrente[i] == 0.000) contZerosCorrente++;
    media_tensaoRMS +=  media_tensao[i];
    media_correnteRMS +=  media_corrente[i];
    Potencia_Ativa += Media_PotAtiva[i];

  }
  
  
  if (contZerosTensao >= 5)  media_tensaoRMS = 0;
  else {
    media = media_tensaoRMS / ciclo;
    for (i = 0; i < 60; i++) {
      if (media_tensao[i] < (media - 5.0) || media_tensao[i] > (media + 5.0))media_tensao[i] = media;
      media_tensaoRMS +=  media_tensao[i];
    }
    media_tensaoRMS =  media_tensaoRMS / ciclo;
  }

   //_______________________ verifica se está em nível adequado, crítico e precário ___________
    if (media_tensaoRMS >= (Tensao_Nominal * 0.92) && media_tensaoRMS <= (Tensao_Nominal * 1.05))StatusTensao = 1; // adequado
    else if (media_tensaoRMS<(Tensao_Nominal * 0.87)|| media_tensaoRMS > (Tensao_Nominal * 1.06))StatusTensao = 3;
    else StatusTensao = 2; // critico
    Serial.println(StatusTensao);
    float test = Tensao_Nominal * 0.92;
    Serial.println(test);

  if (contZerosCorrente >= 5) media_correnteRMS = 0;
  else media_correnteRMS = media_correnteRMS / ciclo;
  if (media_correnteRMS < 4 ) media_correnteRMS = 0; 

  if (media_tensaoRMS == 0.00 || media_correnteRMS == 0.00) {
    Potencia_Aparente = 0.00;
    Potencia_Ativa = 0.00;
    FP = 0.00;
  }
  else {
    media_tensaoRMS = media_tensaoRMS * 0.98;
    if(media_tensaoRMS < 122.5) media_tensaoRMS = media_tensaoRMS + 1.0;
    media_correnteRMS = (media_correnteRMS * 1.013)*1.061;
    if(media_correnteRMS > 21.00) media_correnteRMS - 1.4;
    if(media_correnteRMS < 18.0) media_correnteRMS = (media_correnteRMS * 0.982);
    if(media_correnteRMS > 6.0 && media_correnteRMS < 7.0) media_correnteRMS = media_correnteRMS - 0.20;
    else if(media_correnteRMS >7.0 && media_correnteRMS < 8.0) media_correnteRMS = media_correnteRMS - 0.50;
   // else if(media_correnteRMS < 6.0) media_correnteRMS = 0.15;
    //media_correnteRMS = media_correnteRMS / 2;
    Potencia_Aparente = media_tensaoRMS * media_correnteRMS/1000;
    Potencia_Ativa = Potencia_Ativa / ciclo;
    Potencia_Ativa = Potencia_Ativa * Fc_W/1000;
    Potencia_Ativa = Potencia_Ativa * 1.034 * 1.058;//1.035;
    //Potencia_Ativa = Potencia_Ativa 2.0;
    FP = Potencia_Ativa / Potencia_Aparente;
    /* teste com corrente mais aixa
    if(media_correnteRMS < 10.0){
    FP = FP - 0.02;
    Potencia_Ativa = Potencia_Aparente * FP;
    if(Potencia_Ativa > Potencia_Aparente) Potencia_Ativa = Potencia_Aparente;
   // if(FP > 1.0){
   //   FP = 1.0 - (FP - 1.0);
    //  Potencia_Ativa = Potencia_Aparente * FP;
    }*/
    if(Potencia_Ativa > Potencia_Aparente) Potencia_Ativa = Potencia_Aparente;
  }
  
  static long tempAux;
  tempAux = millis();
  Kwh += ((Potencia_Ativa / 3600000)*(tempAux- tempoKwh));
  tempoKwh = tempAux;
   
// envia as medições para o seridor
  client = server.available();
  if (client) {                  // se tiver uma solicitação do servidor, entra na condição
    while (!client.available()) {
      delay(1);
    }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    client.print(String(media_tensaoRMS) + ":" + String(media_correnteRMS) + ":" + String(Potencia_Ativa) + ":" + String(Potencia_Aparente) + ":" + String(FP) + ":" 
    + String(Kwh) + ":" + String(StatusTensao));

    delay(1);
    Serial.println("Embarcado saiu (: ");
    Serial.println("");

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    static String aux = "";   // para ver se há comando para atualizar parâmetros
      i = request.indexOf('/')+1;
      if(request.charAt(i) == 'a'){ 
        for (i = i+1; request.charAt(i) != 'b' ; i++){
           aux += request.charAt(i);
           if (i >=20) break;
      }
      if(aux == "Atualiza_Par")CarregaParametros(); 
      aux = "";    
  }
 }

  // para o registro dos máximo e mínimos
  if (media_tensaoRMS > tensao_max) tensao_max = media_tensaoRMS;
  else if (media_tensaoRMS < tensao_minima) tensao_minima = media_tensaoRMS;

  if (media_correnteRMS > corrente_maxima) corrente_maxima = media_correnteRMS;

  if (Potencia_Aparente >  VA_maximo)  VA_maximo = Potencia_Aparente;
  else if (Potencia_Aparente <  VA_minimo) VA_minimo = Potencia_Aparente;

  if (Potencia_Ativa > W_maximo) W_maximo = Potencia_Ativa;
  else if (Potencia_Ativa <  W_minimo)  W_minimo = Potencia_Ativa;

//##################################################################################
  // verifica se atuou alguma proteção  ProtecaoTensaoAlta, ProtecaoTensaoBaixa, ProtecaoCorrente, ProtecaoVAAlta, ProtecaoWAlta;
  static bool FlagTensaoAlta = false, FlagTensaoBaixa = false, FlagTensaoNormal = false, FlagCritico = false, FlagPrecario = false,  FlagNivelNormal = false,FlagSobrecorrente = false,FlagSobrecarga = false;
  if (media_tensaoRMS >= ProtecaoTensaoAlta){
      if(FlagTensaoAlta == false) Regitra_Eventos("Tensao_Alta");
      FlagTensaoAlta = true;  FlagTensaoNormal = false;
  }
  else if (media_tensaoRMS <= ProtecaoTensaoBaixa){
      if(FlagTensaoBaixa == false) Regitra_Eventos("Tensao_Baixa");
      FlagTensaoBaixa = true;  FlagTensaoNormal = false;
  }
  else {
        FlagTensaoAlta = false; FlagTensaoBaixa = false; 
        if(FlagTensaoNormal == false)Regitra_Eventos("Tensao_Normal");
        FlagTensaoNormal = true;
  }
  
  if(StatusTensao == 2){
     if(FlagCritico == false)Regitra_Eventos("Tensao_Nivel_Critico");
     FlagCritico = true;
     FlagNivelNormal = false;
  }
  else if(StatusTensao == 3){
     if(FlagPrecario == false)Regitra_Eventos("Tensao_Nivel_Precario");
     FlagPrecario = true;
     FlagNivelNormal = false;
  }
  else {
     if(FlagNivelNormal == false)Regitra_Eventos("Tensao_Nivel_Adequado");
     FlagPrecario = false;
     FlagCritico = false;
     FlagNivelNormal = true;   
  }

  if(media_correnteRMS >= ProtecaoCorrente && FlagSobrecorrente == false){
    Regitra_Eventos("Corrente_Alta");
    FlagSobrecorrente = true;
  }
  else if(FlagSobrecorrente == true && media_correnteRMS < ProtecaoCorrente){
    Regitra_Eventos("Corrente_Normal");
    FlagSobrecorrente = false;
  }

  if( Potencia_Ativa >= ProtecaoWAlta && FlagSobrecarga == false) {
    Regitra_Eventos("Carga Alta");
    FlagSobrecarga = true;
  }
  else if(FlagSobrecarga == true && Potencia_Ativa < ProtecaoWAlta){
    Regitra_Eventos("Carga Normal");
    FlagSobrecarga = false;
  }      
//##################################################################################33
  ciclo = 0;
  media_tensaoRMS = 0;
  media_correnteRMS = 0;
  contZerosTensao = 0;
  contZerosCorrente = 0;
  return;
}


void Regitra_Eventos(String evento) {
  Serial.println("regstrando evento");
  
  if (client.connect(host, 8080))
  {
    Serial.println("connected");

    Serial.println("[Sending a request]");
    Serial.println(evento);
    client.print(String("GET /TCC/CarregaEventosESP32.php?Evento=") + evento + " HTTP/1.1\r\n" +     // conexão com a pagina no servidor
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");

  } else
  {
    Serial.println("connection failed!]");
    client.stop();
  }

  return;
}



void Registra_MaxMin() {
 Serial.println("registro max e min");

  if (client.connect(host, 8080))
  {
    Serial.println("connected]");

    Serial.println("[Sending a request]");

    client.print(String("GET /TCC/comWemos.php?tensao_max=") + int(tensao_max) + "&tensao_minima=" +  int(tensao_minima) + "&corrente_maxima=" +  int(corrente_maxima) + "&VA_maximo=" +  int(VA_maximo)
                 + "&VA_minimo=" +  int(VA_minimo) + "&W_maximo=" +  int(W_maximo) + "&W_minimo=" +  int(W_minimo) + "&Kwh=" +  String(Kwh) + " HTTP/1.1\r\n" +    // conexão com a pagina no servidor
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );

    Serial.println("[Response:]");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        teste = client.readStringUntil('\n');
        Serial.println(teste);

      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");

  } else
  {
    Serial.println("connection failed!]");
    client.stop();
  }
    // para sincronsar a hora
      String aux = "";
      if (teste.charAt(0) == 'a') {
      i = teste.indexOf('b');
      for (i = i + 1; teste.charAt(i) != 'c'; i++)aux += teste.charAt(i);
      SincHora= aux.toInt();
      if(SincHora >= 59)SincHora = 0;
      else SincHora + 1;
      Serial.println(SincHora);
      }
  tensao_max = 0.0;
  corrente_maxima = 0.0;
  VA_maximo = 0.0;
  W_maximo = 0.0;
  tensao_minima = 999.99;
  VA_minimo = 999.99;
  W_minimo = 999.99;
  Kwh = 0.0;
  return;
}
void carregaConfigRede(){
  Serial.println("Carregando Rede");
  String linha;
  int cont1, cont2;
  File file2 = SPIFFS.open("/test.txt");
  if (!file2 || file2.isDirectory()) {
    Serial.println("nada salvo");
    //ConfigRede();
  }
  else{
    Serial.println("Existe!!!");
    while(file2.available()){
    //Serial.write(file2.read());
    linha = file2.readStringUntil('\n');
    }
    linha+='\0';
    Serial.println(linha);
    cont1 = 0;
    cont2 = 0;
  }
  String aux=linha;
  String auxIP;
  cont1 = linha.indexOf(':');
  aux=aux.substring(0,cont1);
  for(cont2=0;cont2<=cont1;cont2++)ssid[cont2]=aux[cont2];
  linha.remove(0,cont1+1);
  
  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  for(cont2=0;cont2<=cont1;cont2++)password[cont2]=aux[cont2];
  linha.remove(0,cont1+1);
  
  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ip1=0;
  for(cont2=0;cont2<cont1;cont2++){
    ip1=ip1*10;
    ip1+=((int)aux[cont2]-48);
  }
  linha.remove(0,cont1+1);
  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ip2=0;
  for(cont2=0;cont2<cont1;cont2++){
    ip2=ip2*10;
    ip2+=((int)aux[cont2]-48);
  }
  linha.remove(0,cont1+1);
  
  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ip3=0;
  for(cont2=0;cont2<cont1;cont2++){
    ip3=ip3*10;
    ip3+=((int)aux[cont2]-48);
  }
  linha.remove(0,cont1+1);

  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ip4=0;
  for(cont2=0;cont2<cont1;cont2++){
    ip4=ip4*10;
    ip4+=((int)aux[cont2]-48);
  }
  linha.remove(0,cont1+1);

  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ipS1=0;
  int indice = 0;
  for(cont2=0;cont2<cont1;cont2++){
    ipS1=ipS1*10;
    ipS1+=((int)aux[cont2]-48);
    host[indice] = aux[cont2];
    indice++;
  }
  host[indice] = '.';
  indice++;
  
  linha.remove(0,cont1+1);

  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ipS2=0;
  for(cont2=0;cont2<cont1;cont2++){
    ipS2=ipS2*10;
    ipS2+=((int)aux[cont2]-48);
    host[indice] = aux[cont2];
    indice++;
  }
  host[indice] = '.';
  indice++;
  
  linha.remove(0,cont1+1);

  aux=linha;
  cont1 = aux.indexOf(':');
  aux=aux.substring(0,cont1);
  ipS3=0;
  for(cont2=0;cont2<cont1;cont2++){
    ipS3=ipS3*10;
    ipS3+=((int)aux[cont2]-48);
    host[indice] = aux[cont2];
    indice++;
  }
  host[indice] = '.';
  indice++;
  
  linha.remove(0,cont1+1);

  aux=linha;
  cont1 = aux.indexOf('\0');
  aux=aux.substring(0,cont1);
  ipS4=0;
  for(cont2=0;cont2<cont1;cont2++){
    ipS4=ipS4*10;
    ipS4+=((int)aux[cont2]-48);
    host[indice] = aux[cont2];
    indice++;
  }
  linha.remove(0,cont1+1);
  
  Serial.println(ssid);
  delay(500);
  Serial.println(password);
  delay(500);
  Serial.println("host:");
  delay(500);
  Serial.println(host);
  delay(500);
  file2.close();
  return;
}

void ConfigRede(){
   
  File file1 = SPIFFS.open("/test.txt", FILE_WRITE);
  if (!file1) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  
  char aux1[15] = "";
  char aux2[15] = "";
  int cont=0;
  char aux;
  
    Serial.print('A');
    delay(1000);
    cont=0;
    while(Serial.available()>0)                //se algum dado disponível
     {
        aux1[cont]= Serial.read();   //le o byte disponivel
        cont++;        
     }
     if (file1.print(aux1)) {
     // Serial.println("rede ok");
      } else {
      Serial.println("File write failed");
     }
     Serial.print('B');
     delay(1000);
     cont=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        aux2[cont]= Serial.read();   //le o byte disponivel
        cont++;        
     }
     String Pass = ":"+(String)aux2;
     if (file1.print(Pass)) {
        //Serial.println("password ok");
      } else {
        Serial.println("File write failed");
      }
     delay(1000);
     Serial.print('C');
     delay(1000);
     ip1=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ip1=ip1*10;
        ip1+=((int)Serial.read()-48);           //le o byte disponivel        
     }
     delay(1000);
     Serial.print('D');
     delay(1000);
     ip2=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ip2=ip2*10;
        ip2+=((int)Serial.read()-48);        
     }
     delay(1000);
     Serial.print('E');
     delay(1000);
     ip3=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ip3=ip3*10;
        ip3+=((int)Serial.read()-48);        
     }
     delay(1000);
     Serial.print('F');
     delay(1000);
     ip4=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ip4=ip4*10;
        ip4+=((int)Serial.read()-48);       
     }
     String IP = ":"+(String)ip1+":"+(String)ip2+":"+(String)ip3+":"+(String)ip4;
     if (file1.print(IP)) {
       // Serial.println("IP ok");
      } else {
        Serial.println("File write failed");
      }
     delay(1000);
     Serial.print('G');
     delay(1000);
     ipS1=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ipS1=ipS1*10;
        ipS1+=((int)Serial.read()-48);       
     }
     delay(1000);
     Serial.print('H');
     delay(1000);
     ipS2=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ipS2=ipS2*10;
        ipS2+=((int)Serial.read()-48);       
     }
     delay(1000);
     Serial.print('I');
     delay(1000);
     ipS3=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ipS3=ipS3*10;
        ipS3+=((int)Serial.read()-48);       
     }
     delay(1000);
     Serial.print('J');
     delay(1000);
     ipS4=0;
     while(Serial.available()>0)                //se algum dado disponível
     {
        ipS4=ipS4*10;
        ipS4+=((int)Serial.read()-48);       
     } 
  
  String IPS = ":"+(String)ipS1+":"+(String)ipS2+":"+(String)ipS3+":"+(String)ipS4;
  if (file1.print(IPS)) {
  }
  file1.close();
  ESP.restart();  

}
