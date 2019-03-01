/*******************************************************************************
*        PROJETO AUTOMA��O PARA PARTIDA AUTOM�TICA DE GERADOR EL�TRICO         *
*                CCB(CONGREGA��O CRIST� NO BRASIL)                             *
*                        JARDIM ANG�LICA                                       *
*                    MODELO PIC 16F628a                                        *
*               IDE : MIKROC PRO FOR PIC V6.6.3                                *
*                 CRIADO  POR: GUSTAVO RODRIGUES                               *
*******************************************************************************/

//**********************DIRETIVAS E DIFINES*************************************

#define fase RB4_bit              //Conector de entrada inferior(concession�ria).
#define motor RB5_bit             //Conector de entrada superior (gerador).
#define liga RA0_bit              //Rel� da partida.
#define toogle RA1_bit            //1� saida.
#define ts RA2_bit                //2� saida.
#define erro RA3_bit              //3� saida.                         
                                            




/*---------------------VARI�VEIS GLOBAIS--------------------------------------*/

short tentativas = 4;             //numero de tentativas de partida do motor.
short tempoLiga = 2;              //dura��o do tempo de cada tentativa(Segundos).
short tempoDesliga = 2;           //dura��o do tempo entre as tentativa de partida(Segundos).
int transferencia = 2;            //tempo em segundos para transferencia de energia
short cont;
bit automatico;                     
int x = 1;


/*---------------------PROT�TIPOS DE FUN��ES----------------------------------*/
void atraso(int tempo);
void partida();
void transswitch();

/*---------------------CONFIGURA��O INICIAL-----------------------------------*/
void main()
{
 CMCON = 0x07;                      //desabilita os comparadores internos
 TRISA= 0x00;                       //configura porte A como saida
 TRISB= 0xff;                       //configura porte B como entrada
 PORTA = 0x00;                      //desliga todo porte A
 OPTION_REG = 0x8E;                 //configura o Watchdog timer para 1,15 seg.
 asm clrwdt;                        //limpa o wdt
 automatico = 1;
 
/*--------------------------LOOP INFINITO-------------------------------------*/                       

   while(1)
   {
     asm clrwdt;                               //limpa o wdt e espera.                                                               
     Delay_ms(10);          
     while(automatico)
     {                         //enquanto houver tentativas
      
        asm clrwdt;                            //limpa o wdt e espera. 
        Delay_ms(100);
        toogle = 1;                             //luz de automatico pisca
        Delay_ms(300);
        toogle = 0;
        Delay_ms(300);
        asm clrwdt;
        if(!fase && !motor  )partida();         //se motor e fase n�o estiverem ligados, vai para fun��o de ligar.
        if(motor && x )transswitch();           //se o motor ligou faz a transferencia.
      }
   }  
}
   

/*-------------------------- FUN��ES -----------------------------------------*/


void partida()
{    

     while(!motor && tentativas>0)       //enquanto houver tentativas e motos desligado                                              
      {  
         asm clrwdt;
         liga=1;                        //liga motor
         atraso(tempoLiga);             //continua ligando pelo tempo programado
         liga=0;                        //para de ligar
         if(!motor)
           {                             //se o motor ainda n�o ligou espera para nova tentativa
           atraso(tempoDesliga);
           tentativas--;                //decrementa numero de tentativas
           }
      }   
      if (!tentativas)
      {
      erro  = 1;        //se acabaram as tentativas liga a luz de erro
      automatico = 0;
      }
       
}

 void atraso(int tempo)                 //rotina que fraciona um delay de segundos para detec��o de ligamento do motor
{ 
   cont=(tempo*1000)/100;
    while(cont>0 && !motor)
    {
      asm clrwdt;
      cont--;
      Delay_ms(100);
    }
}


void transswitch()                       // rotina que transfere a energia para o circuito local.
{    
     short i = transferencia * 2;        // aguarda "X" segunos apos ligamento (previamente configurado)
     for(i; i>0 ; i--)
     {
        Delay_ms(500);
        asm clrwdt;
     }
     
     ts = 1;                             // da um pulso no pino de ts(transferencia)
     Delay_ms(200);
     ts = 0;
     x = 0;                              //desabilita a fun��o de transferencia para evitar novas transferencias.
}