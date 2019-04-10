#include "../headers/MPU.h"

///////////////////////////////////////////////////////////////////////////
////////////////////// 24LC512 ou 24LC1025 ////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// (100) Esperar Flash terminar gravação
// Ficar acessando com WR até ela responder
void m24lc_espera(void){
  twi_start(100);                //START
  //Esperar Flash enviar ack
  while ( twi_er_check(M24LC_EWR, 101) == FALSE);
  twi_stop();                   //Gerar STOP para finalizar
}

// (90) Escrever um bloco com FF
// qtd --> precisa estar dentro de página de 128 bytes
void m24lc_apg_blk(word adr, byte qtd){
  byte ct;
  twi_start(90);                //START
  twi_er(M24LC_EWR, 91);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 92);      //MSB do endereço
  twi_dado_er(adr&0xFF, 93);    //LSB do endereço
  for (ct=0; ct<qtd; ct++)
    twi_dado_er(0xFF, 94);      //0xFF a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
}

// (80) Ler um bloco a partir de um endereço
// qtd --> não há limitação dentro dos 16 bits
void m24lc_rd_blk(word adr,  byte *vet, word qtd){
  word ct;
  twi_start(80);                //START
  twi_er(M24LC_EWR, 81);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 82);      //MSB do endereço
  twi_dado_er(adr&0xFF, 83);    //LSB do endereço
  twi_start_rep(84);            //START Repetido
  twi_et(M24LC_ERD, 85);        //Endereçar 24LCxxx para leitura
  for (ct=0; ct<qtd-1; ct++)
    vet[ct]=twi_dado_et_ack(86);//Ler um dado com ACK
  vet[ct]=twi_dado_et_nack(87); //Ler último dado com NACK
  twi_stop();                   //Gerar STOP para finalizar
}

// (70) Escrever um bloco a partir de um endereço
// qtd --> precisa estar dentro de página de 128 bytes
void m24lc_wr_blk(word adr, byte *vet, byte qtd){
  byte ct;
  twi_start(70);                //START
  twi_er(M24LC_EWR, 71);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 72);      //MSB do endereço
  twi_dado_er(adr&0xFF, 73);    //LSB do endereço
  for (ct=0; ct<qtd; ct++)
    twi_dado_er(vet[ct], 74);   //Dado  a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
}

// (50) Ler um endereço da memória
byte m24lc_rd(word adr) {
  byte dado;
  twi_start(50);                //START
  twi_er(M24LC_EWR, 51);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 52);      //MSB do endereço
  twi_dado_er(adr&0xFF, 53);    //LSB do endereço
  twi_start_rep(54);            //START Repetido
  twi_et(M24LC_ERD, 55);        //Endereçar 24LCxxx para leitura
  dado = twi_dado_et_nack(56);  //Receber dado da 24LCxxx com NACK
  twi_stop();                   //Gerar STOP para finalizar
  return dado;
}

// (60) Escrever num endereço da memória
void m24lc_wr(word adr, byte dado){
  twi_start(60);                //START
  twi_er(M24LC_EWR, 61);        //Endereçar 24LCxxx para escrita
  twi_dado_er(adr>>8, 62);      //MSB do endereço
  twi_dado_er(adr&0xFF, 63);    //LSB do endereço
  twi_dado_er(dado, 64);        //Dado  a ser escrito
  twi_stop();                   //Gerar STOP para finalizar
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////// MPU ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


// Média da Aceleração, temperatura e giroscópio
void mpu_rd_med_ac_tp_gi(word rpt){
  word i;
  byte vet[14];
  long lax,lay,laz,ltp,lgx,lgy,lgz;
  lax=lay=laz=ltp=lgx=lgy=lgz=0;
  for (i=0; i<rpt; i++){
    mpu_rd_blk(ACCEL_XOUT_H, vet, 14);
    lax += (int)((vet[ 0] << 8) | vet[ 1]);    //Montar Acel X
    lay += (int)((vet[ 2] << 8) | vet[ 3]);    //Montar Acel Y
    laz += (int)((vet[ 4] << 8) | vet[ 5]);    //Montar Acel Z
    ltp += (int)((vet[ 6] << 8) | vet[ 7]);    //Montar Temp
    lgx += (int)((vet[ 8] << 8) | vet[ 9]);    //Montar Giro X
    lgy += (int)((vet[10] << 8) | vet[11]);    //Montar Giro Y
    lgz += (int)((vet[12] << 8) | vet[13]);    //Montar Giro Z
  }
  axi=lax/rpt;
  ayi=lay/rpt;
  azi=laz/rpt;
  tpi=ltp/rpt;
  gxi=lgx/rpt;
  gyi=lgy/rpt;
  gzi=lgz/rpt;
}

// Ler Aceleração, temperatura e giroscópio
void mpu_rd_ac_tp_gi(void){
  byte vet[14];
  mpu_rd_blk(ACCEL_XOUT_H, vet, 14);
  axi = (int)((vet[ 0] << 8) | vet[ 1]);    //Montar Acel X
  ayi = (int)((vet[ 2] << 8) | vet[ 3]);    //Montar Acel Y
  azi = (int)((vet[ 4] << 8) | vet[ 5]);    //Montar Acel Z
  tpi = (int)((vet[ 6] << 8) | vet[ 7]);    //Montar Temp
  gxi = (int)((vet[ 8] << 8) | vet[ 9]);    //Montar Giro X
  gyi = (int)((vet[10] << 8) | vet[11]);    //Montar Giro Y
  gzi = (int)((vet[12] << 8) | vet[13]);    //Montar Giro Z
}

// Média da Aceleração e temperatura
void mpu_rd_med_ac_tp(word rpt){
  byte vet[8];
  word i;
  long lax,lay,laz,ltp;
  lax=lay=laz=ltp=0;
  for (i=0; i<rpt; i++){
    mpu_rd_blk(ACCEL_XOUT_H, vet, 8);
    lax += (int)((vet[0] << 8) | vet[1]);    //Montar Acel X
    lay += (int)((vet[2] << 8) | vet[3]);    //Montar Acel Y
    laz += (int)((vet[4] << 8) | vet[5]);    //Montar Acel Z
    ltp += (int)((vet[6] << 8) | vet[7]);    //Montar Temp
  }
  axi=lax/rpt;
  ayi=lay/rpt;
  azi=laz/rpt;
  tpi=ltp/rpt;
}

// Ler Aceleração e temperatura
void mpu_rd_ac_tp(void){
  byte vet[8];
  mpu_rd_blk(ACCEL_XOUT_H, vet, 8);
  axi = (int)((vet[0] << 8) | vet[1]);    //Montar Acel X
  ayi = (int)((vet[2] << 8) | vet[3]);    //Montar Acel Y
  azi = (int)((vet[4] << 8) | vet[5]);    //Montar Acel Z
  tpi = (int)((vet[6] << 8) | vet[7]);    //Montar Temp
  //serial_str("MPU = ");
  //serial_xyzt(ax,ay,az,tp);
  //serial_crlf();
}

// Acordar o MPU e programar para usar relógio Giro X
void mpu_acorda(void) {
  mpu_wr(PWR_MGMT_1, 1);
}

// Dormir o MPU e programar para usar relógio Giro X
void mpu_dorme(void) {
  mpu_wr(PWR_MGMT_1, 0x21);  //SLEEP=1 e PLL com Giro X
}

// Ler o registrador WHO_AM_I
byte mpu_whoami(void) {
  return mpu_rd(WHO_AM_I);
}

// Colocar o MPU num estado conhecido
// Algumas operações são redundantes dependendo de
// quando esta função é chamada
void mpu_inicializa(void) {

  // Despertar MPU, Relógio = PLL do Giro-x
  mpu_wr(PWR_MGMT_1, 0x01);
  delay(200);       //200ms - Esperar PLL estabilizar

  // 6 ==> Taxa = 1 kHz, Banda: Acel=5 Hz e Giro=5 Hz e delay=19 mseg
  mpu_wr(CONFIG, 6);

  // Taxa de amostragem =  taxa/(1+SMPLRT_DIV) = 1k/10 = 100Hz
  mpu_wr(SMPLRT_DIV, 9);  //Taxa = 100 Hz
  //mpu_wr(SMPLRT_DIV, 4);  //Taxa = 1k/5 = 500 Hz
}

// Selecionar Fundo de Escalas para o MPU
// Acel: 0=+/-2g, 1=+/-4g, 2=+/-8g, 3=+/-16g
// Gyro: 0=+/-250gr/s, 1=+/-500gr/s, 2=+/-1000gr/s, 3=+/-2000gr/s,
void mpu_escalas(byte gfs, byte afs) {
  mpu_wr(GYRO_CONFIG, gfs << 3); //FS do Giro
  mpu_wr(ACCEL_CONFIG, afs << 3); //FS do Acel
}

///////////////// Rotinas Básicas para MPU

// (10) Escrever num registrador do MPU
void mpu_wr(byte reg, byte dado) {
  twi_start(10);          //START
  twi_er(MPU_EWR, 11);    //Endereçar MPU para escrita
  twi_dado_er(reg, 12);   //Informar acesso ao PWR_MGMT_1 (0x6B)
  twi_dado_er(dado, 13);  //Selecionar PLL eixo X como referência
  twi_stop();             //Gerar STOP para finalizar
}

// (20) Ler um registrador do MPU
byte mpu_rd(byte reg) {
  uint8_t dado;
  twi_start(20);                //START
  twi_er(MPU_EWR, 21);           //Endereçar MPU para escrita
  twi_dado_er(reg, 22);         //Informar registrador
  twi_start_rep(23);            //START Repetido
  twi_et(MPU_ERD, 24);           //Endereçar MPU para leitura
  dado = twi_dado_et_nack(25);  //Receber dado do MPU com NACK
  twi_stop();                    //Gerar STOP para finalizar
  return dado;
}

// (30) Escrever um bloco de dados no MPU a partir de um registrador
void mpu_wr_blk(byte reg, byte *dado, byte qtd) {
  uint8_t i;
  twi_start(30);                //START
  twi_er(MPU_EWR, 31);          //Endereçar MPU para escrita
  twi_dado_er(reg, 32);         //Informar acesso ao PWR_MGMT_1 (0x6B)
  for (i = 0; i < qtd; i++)
    twi_dado_er(dado[i], 33);   //Selecionar PLL eixo X como referência
  twi_stop();                   //Gerar STOP para finalizar
}

// (40) Ler um bloco do MPU a partir de um registrador
void mpu_rd_blk(byte reg, byte *dado, byte qtd) {
  byte i;
  twi_start(40);                //START
  twi_er(MPU_EWR, 41);          //Endereçar MPU para escrita
  twi_dado_er(reg, 42);         //Informar registrador
  twi_start_rep(43);            //START Repetido
  twi_et(MPU_ERD, 44);          //Endereçar MPU para leitura
  for (i=0; i<qtd-1; i++)
    dado[i] = twi_dado_et_ack(45);  //Receber dados e gerar ACK
  dado = twi_dado_et_nack(46);  //Receber último dado e gerar NACK
  twi_stop();                   //Gerar STOP para finalizar
}
