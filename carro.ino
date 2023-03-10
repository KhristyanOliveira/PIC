#include <Ultrasonic.h>

/* Definições das portas para leitura do sensor ultrasonico */
#define trigger     13
#define echo       12

/* Definições de operação do sensor ultrasônico */
#define dist_minima                30.0 //cm
#define tempo_de_leitura 250  //ms

/* Definições dos pinos para controle dos dois motores */
#define IN_1      4
#define IN_2      5
#define IN_3      6
#define IN_4      7

/* Definições dos motores a serem controlados */
#define MOTOR_A  0x00
#define MOTOR_B  0x01

/* Definições das ações dos motores */
#define ACAO_FREIO  0x00
#define ACAO_MOVIMENTO_ANTI_HORARIO  0x01
#define ACAO_MOVIMENTO_HORARIO  0x02
#define ACAO_PONTO_MORTO  0x03

/* Definições de sentido de giro (em caso de obstáculo) */
#define SENTIDO_GIRO_ANTI_HORARIO  0x00
#define SENTIDO_GIRO_HORARIO  0x01

/* Definições do desvio de objetos */
#define ESTADO_AGUARDA_OBSTACULO  0x00
#define ESTADO_GIRANDO  0x01

/* Variáveis e objetos globais */

char ultimo_lado_que_girou = (SENTIDO_GIRO_ANTI_HORARIO);
char estado_desvio_obstaculos = (ESTADO_AGUARDA_OBSTACULO);

/* Protótipos */
void configura_controle_motor(void);
void controla_motor(char motor, char acao);
void desvio_obstaculos(float distancia_obstaculo);

void configura_controle_motor(void)
{
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);

  pinMode(echo, INPUT);
  pinMode(trigger,OUTPUT);
}

// configura o funcionamento dos motores 
void controla_motor(char motor, char acao)
{
  int 1_motor = 0;
  int 2_motor = 0;


  switch (motor)
  {
    case (MOTOR_A):
      1_motor = IN_1;
      2_motor = IN_2;
      break;

    case (MOTOR_B):
      1_motor = IN_3;
      2_motor = IN_4;
      break;

    default:
      /* Motor inválido. Nada mais deve ser feito nesta função */
      return;
  }

  /* Controla o motor conforme ação desejada */
  switch (acao)
  {
    case (ACAO_FREIO):
      digitalWrite(1_motor, HIGH);
      digitalWrite(2_motor, HIGH);
      break;

    case (ACAO_MOVIMENTO_ANTI_HORARIO):
      digitalWrite(1_motor, LOW);
      digitalWrite(2_motor, HIGH);
      break;

    case (ACAO_MOVIMENTO_HORARIO):
      digitalWrite(1_motor, HIGH);
      digitalWrite(2_motor, LOW);
      break;

    case (ACAO_PONTO_MORTO):
      digitalWrite(1_motor, LOW);
      digitalWrite(2_motor, LOW);
      break;

    default:
      /* Ação inválida. Nada mais deve ser feito nesta função */
      return;
  }
}

// implementação do sensor de obstáculos
void desvio_obstaculos(float distancia_obstaculo){
  
  switch (estado_desvio_obstaculos)
  {
    case (ESTADO_AGUARDA_OBSTACULO):
      if (distancia_obstaculo <= dist_minima)
      {
        /* Obstáculo encontrado. O robô deve girar para
           desviar dele */

        /* Alterna sentido de giro para se livrar de obstáculos
           (para otimizar o desvio de obstáculos) */
        if (ultimo_lado_que_girou == SENTIDO_GIRO_ANTI_HORARIO) {
          ultimo_lado_que_girou = (SENTIDO_GIRO_HORARIO);
        }
        else {
          ultimo_lado_que_girou = (SENTIDO_GIRO_ANTI_HORARIO);

          estado_desvio_obstaculos = ( ESTADO_GIRANDO);
        }
      }
      else
      {
        /* Se não há obstáculos, continua em frente */
        controla_motor((MOTOR_A), (ACAO_MOVIMENTO_HORARIO));
        controla_motor((MOTOR_B), (ACAO_MOVIMENTO_HORARIO));
      }

      break;

    case (ESTADO_GIRANDO):
      if (distancia_obstaculo > dist_minima)
      {
        /* Não há mais obstáculo a frente do robô */
        estado_desvio_obstaculos = (ESTADO_AGUARDA_OBSTACULO);

      }
      else
      {
        if (ultimo_lado_que_girou == SENTIDO_GIRO_ANTI_HORARIO)
        {
          controla_motor((MOTOR_A), (ACAO_MOVIMENTO_ANTI_HORARIO));
          controla_motor((MOTOR_B), (ACAO_MOVIMENTO_HORARIO));

        }
        else
        {
          controla_motor((MOTOR_A), (ACAO_MOVIMENTO_HORARIO));
          controla_motor((MOTOR_B), (ACAO_MOVIMENTO_ANTI_HORARIO));

        }
      }

      break;
  }
}

void setup()
  {

   configura_controle_motor();
   controla_motor((MOTOR_A), (ACAO_FREIO));
   controla_motor((MOTOR_B), (ACAO_FREIO));
  }


void loop()
{
  float distancia_a_frente = 0.0;
  float duracao;
// leitura do sensor ultrassonico
  digitalWrite(trigger,HIGH);  
  delayMicroseconds(10);      
  digitalWrite(trigger,LOW);
  duracao=pulseIn(echo,HIGH);      
  
   distancia_a_frente = duracao *0.017;

  /* Verifica se há obstáculo a frente */
  desvio_obstaculos(distancia_a_frente);

  delay(tempo_de_leitura);
}
