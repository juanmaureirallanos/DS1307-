# include "16F877a.h"
# use delay(Clock=20Mhz)

# FUSES NOWDT                 	//No Watch Dog Timer
# FUSES HS                 		//High speed oscillator
# FUSES NOBROWNOUT            	//No brownout reset
# FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O


#define	LED			PIN_A2		//Led que indica estado de la conexion

int Indicator=0;

#include"DS1307.c"
#include"ConfigPc.c"
#include"Indicador.c"
#include"Print.c"


void InitializeComponents(void){
	delay_ms(5);
	//Indicator=input_d();
//	delay_ms(5);
	DS1307_init();				//Prepara bus i2c de micro
	delay_ms(10);
	InitIndicador();
	delay_ms(5);
	InitPc();
	delay_ms(10);
	enable_interrupts(global);	//Habilita las interrupciones
}

void main(void){
InitializeComponents();
	while(true){
		if(NewDateTime){
			DS1307_SetDatetime(dia,meses,ano,hora,minutos,segundos);
			NewDateTime=false;
		}
		if(NewDataWeigth){
			SeparaDatos();
			ClearBufferXk();
			NewDataWeigth=false;
			ImprimirEtiqueta();
		}
//		fprintf(PC,"hola mundo\r\n");
//		delay_ms(250);
//		output_toggle(PIN_D2);
	}
}

