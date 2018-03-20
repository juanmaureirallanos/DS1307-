/*
							DS1307.c
					Driver para Real Time Clock

AUTHOR: Juan Maureira
FECHA: 01/02/2012

    juanmaureira3@gmail.com
    
    I2C Interface with DS1307

	Datasheet DS1307
	
	ADDRESS MAP
	
	  SECONDS|MINUTES|HOURS|DAY|DATE|MONTH|YEAR|CONTROL|RAM 56 x 8
         00h    01h    02h  03h  04h  05h   06h   07h     08~3Fh

	CONTROL REGISTER

		BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0
		 OUT  |	  0   |   0   | SQWE  |   0   |	  0   |	 RS1  |  RS0

OUT (Output control): This bit controls the output level of the SQW/OUT pin when the square wave
output is disabled. If SQWE = 0, the logic level on the SQW/OUT pin is 1 if OUT = 1 and is 0 if
OUT = 0.

SQWE (Square Wave Enable): This bit, when set to a logic 1, will enable the oscillator output. The
frequency of the square wave output depends upon the value of the RS0 and RS1 bits. With the square
wave output set to 1Hz, the clock registers update on the falling edge of the square wave.

RS (Rate Select): These bits control the frequency of the square wave output when the square wave
output has been enabled. Table 1 lists the square wave frequencies that can be selected with the RS bits.
 
	RS1 RS0 	SQW OUTPUT FREQUENCY
	 0   0 				1Hz
	 0   1 				4.096kHz
	 1   0 				8.192kHz
	 1   1 				32.768kHz
 
  
*/
#ifndef	DS1307_SDA
#define	DS1307_SDA	PIN_C4
#define DS1307_SCL	PIN_C3
#endif

#define	REG_SECONDS		0x00
#define	REG_MINUTES		0x01
#define REG_HORAS		0x02
#define REG_DAY			0x03
#define REG_DATE		0x04
#define REG_MONTH		0x05
#define REG_YEAR		0x06	
#define REG_CONTROL		0x07

#use i2c(master, sda=DS1307_SDA, scl=DS1307_SCL)  

void DS1307_init(void);
int DS1307_read(int address);
void DS1307_write(int address,int data);
void DS1307_SQWOUT(void);

union _SQWOUT{
	int sqwout;
	struct{	
		boolean	RS0;
		boolean	RS1;
		boolean	unused:2;
		boolean	SQWE;
		boolean	nused:2;
		boolean	OUT;
	}bits;
}SQWOUT;

#define	SQW_1HZ(void){\
	SQWOUT.bits.RS0=0;\
	SQWOUT.bits.RS1=0;\
}\

#define	SQW_4096KHz(void){\
	SQWOUT.bits.RS0=1;\
	SQWOUT.bits.RS1=0;\
}\

#define	SQW_8192KHz(void){\
	SQWOUT.bits.RS0=0;\
	SQWOUT.bits.RS1=1;\
}\

#define	SQW_32768KHz(void){\
	SQWOUT.bits.RS0=1;\
	SQWOUT.bits.RS1=1;\
}\

#define	Output_Control_Disable(void){\
	SQWOUT.bits.OUT=0;\
}\

#define	Output_Control_Enable(void){\
	SQWOUT.bits.OUT=1;\
}\

#define	Square_Wave_Enable(void){\
	SQWOUT.bits.SQWE=1;\
}\

#define	Square_Wave_Disable(void){\
	SQWOUT.bits.SQWE=0;\
}\

/**************************************************************************
 FUNCION: DS1307_init
 INPUT:  -
 OUTPUT: -
 NOTA: Inicializa lineas para I2C
**************************************************************************/
void DS1307_init(void){
	output_float(DS1307_SDA);
	output_float(DS1307_SCL);
	delay_ms(5);
	DS1307_SQWOUT();
	delay_ms(5);
}
/**************************************************************************
 FUNCION: DS1307_read
 INPUT:  ADDRESS
 OUTPUT: DATA
 NOTA: Lee registros de la direccion especificada
**************************************************************************/
int DS1307_read(int address){
int data;
	i2c_start();
	i2c_write(0xD0);
	i2c_write(address);
	i2c_start();
	i2c_write(0xD1);
	data=i2c_read(0);
	i2c_stop();
return data;
}
/**************************************************************************
 FUNCION: DS1307_write
 INPUT:  DATA
 OUTPUT: -
 NOTA: Escribe registros de la direccion especificada
**************************************************************************/
void DS1307_write(int address,int data){
	i2c_start();
	i2c_write(0xD0);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
}
/**************************************************************************
 FUNCION: DS1307_SQWOUT
 INPUT:  DATA
 OUTPUT: -
 NOTA: Escribe registros de la direccion especificada
**************************************************************************/
void DS1307_SQWOUT(void){
	SQW_1HZ();
	Output_Control_Disable();
	Square_Wave_Enable();
	DS1307_write(REG_CONTROL,SQWOUT.sqwout);
}
/**************************************************************************
 FUNCION: get_bcd
 INPUT:  data
 OUTPUT: bcd
 NOTA: Convierte entero a bcd // Sacada de DS1302.c
**************************************************************************/
byte get_bcd(BYTE data){
byte nibh,nibl;
   nibh=data/10;
   nibl=data-(nibh*10);
return((nibh<<4)|nibl);
}
/**************************************************************************
 FUNCION: rm_bcd
 INPUT:  data
 OUTPUT: data (en BCD)
 NOTA: convierte data a BCD // Sacada de DS1302.c
**************************************************************************/
byte rm_bcd(BYTE data){
byte i;
   i=data;
   data=(i>>4)*10;
   data=data+(i<<4>>4);
return data;
}
/**************************************************************************
 FUNCION: DS1307_SetDatetime
 INPUT:  day,mth,year,dow,hr,min
 OUTPUT: -
 NOTA: Setea los parametros del reloj
**************************************************************************/
void DS1307_SetDatetime(byte date,byte mth,byte year,byte hr,byte min,byte day){
	DS1307_write(REG_SECONDS,get_bcd(0));
	delay_ms(5);
	DS1307_write(REG_MINUTES,get_bcd(min));
	delay_ms(5);
	DS1307_write(REG_HORAS,get_bcd(hr));
	delay_ms(5);
	DS1307_write(REG_DAY,get_bcd(day));
	delay_ms(5);
	DS1307_write(REG_DATE,get_bcd(date));
	delay_ms(5);
	DS1307_write(REG_MONTH,get_bcd(mth));
	delay_ms(5);
	DS1307_write(REG_YEAR,get_bcd(year));
	delay_ms(5);
}
/**************************************************************************
 FUNCION: DS1307_GetDatetime
 INPUT:  -
 OUTPUT: day,date,hr,mth,year,min,sec,year
 NOTA: Lee fecha y hora de rtc
**************************************************************************/
void DS1307_GetDatetime(byte &date,byte &mth,byte &year,byte &hr,byte &min,byte &day,byte &sec){
	sec=rm_bcd(DS1307_read(REG_SECONDS));
	delay_ms(5);
	min=rm_bcd(DS1307_read(REG_MINUTES));
	delay_ms(5);
	hr=rm_bcd(DS1307_read(REG_HORAS));
	delay_ms(5);
	day=rm_bcd(DS1307_read(REG_DAY));
	delay_ms(5);
	date=rm_bcd(DS1307_read(REG_DATE));
	delay_ms(5);
	mth=rm_bcd(DS1307_read(REG_MONTH));
	delay_ms(5);
	year=rm_bcd(DS1307_read(REG_YEAR));	
	delay_ms(5);
}