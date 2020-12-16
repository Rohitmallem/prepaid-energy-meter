#include<reg52.h>
//#include<intrins.h>
#include "lcd.h"
#include "serial.h" 
#include "varibles_gps_gsm.h"
#include "gsm1_init.h"
#define MOBILE_NO1 "AT+CMGS=\"8985528023\""
sbit meter_sensor=P3^7;
sbit relay=P0^0;
 unsigned char mobile_no[15];
unsigned recharge_amount=0; 
sbit relay1=P0^0;
sbit relay2=P0^1;
sbit relay3=P0^2;
sbit relay4=P0^3;	
sbit buzzer=P3^7; 
sbit IR=P1^7; 
sbit GAS=P1^6; 
void uart_int() interrupt 4
{
if(RI)

{
		gps_gsm[gps_gsm_i]=SBUF;
		if(gps_gsm_i==MAX_GPS_GSM)
		{
			gps_flag=1;
		}
		if(gps_gsm[gps_gsm_i]=='*')
		{
			IE=0X00;
			gsm_loc=gps_gsm_i;
			gsm_flag=1;
		}	
                         gps_gsm_i++;
		RI=0;
	}
} 
void number_track()
{
   	int i,j;
	IE=0X00;
	for(i=0;i<MAX_GPS_GSM;i++)

 { if((gps_gsm[i]=='+')&&(gps_gsm[i+1]=='C')&&(gps_gsm[i+2]=='M')&&(gps_gsm[i+3]=='T')&&(gps_gsm[i+4]==':'))
		{
                                   i=i+8;
			j=0;
			while(gps_gsm[i]!='"')
			{
				mobile_no[j++]=gps_gsm[i++];
			}		
			mobile_no[i]='\0';	
		} 
	} 

}
void gsm_msg(unsigned char *no,unsigned char *msg)
{
	IE=0X00;
	delay(1000);
	lcd_cmd(0x01);
	lcd_string_loc(0x80,"Msg sending.....");
	delay(46);
	uart_string("AT");
	uart_data(0x0d);
	delay(100);
	uart_string("AT+CMGF=1");
	uart_data(0x0d);
	delay(100);
	uart_string("AT+CMGS=");
	uart_data(0x22);    // " SYMBOL
	uart_string(no);
	uart_data(0x22);	   // " SYMBOL
	uart_data(13);
	uart_string(msg);
	uart_data(26);
	lcd_cmd(0x01);
	lcd_string_loc(0x80,"Msg sended.......");
	delay(46);
}
void pulse_chek()
{
	if(recharge_amount>0)
		relay=1;
	else
		relay=0;
	if((meter_sensor==0)&&(recharge_amount>0))
	{
		while(meter_sensor==0);

		recharge_amount--;
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_string("RECHARGE_AMOUNT");
		lcd_cmd(0xc6);
		integer_lcd(recharge_amount);
		if(recharge_amount==10)
		{

			//	gsm_msg(MOBILE_NO1,"ALERT :YOU HAVE ONLY 10 UNITS REMAINING*****");

				delay(100);
		}
	}


} 
void main()
{
    GAS=1;
    IR=1;
	relay=0;
    buzzer=0;
    uart_init();
    lcd_init();
    IE=0X09;
	gsm_setup();
    delay(10);
	gsm_clear();
	lcd_string_loc(0x80,"WELCOME TO ");
	lcd_cmd(0x01);
	lcd_string_loc(0x80,"*PREPAID ENERGY*");
	lcd_string_loc(0xc0,"*****METER******");

	while(1)
	{
	  IE=0X90;
            pulse_chek();
	  if(gsm_flag==1)
	  {
		//	number_track();
		//	uart_string(mobile_no);
		//	lcd_cmd(0x01);
		//	lcd_string_loc(0x80,mobile_no);
			if(gps_gsm[gsm_loc-3]=='R' && gps_gsm[gsm_loc-2]=='5' && gps_gsm[gsm_loc-1]=='0')
			{
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_string("COUPEN_AMOUNT:");
				lcd_string("50");
				recharge_amount=recharge_amount+50;
}
		 	if(gps_gsm[gsm_loc-3]=='R' && gps_gsm[gsm_loc-2]=='3' && gps_gsm[gsm_loc-1]=='0')
			{
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_string("COUPEN_AMOUNT:");
				lcd_string("30");
				recharge_amount=recharge_amount+30;
			}
			if(gps_gsm[gsm_loc-3]=='R' && gps_gsm[gsm_loc-2]=='1' && gps_gsm[gsm_loc-1]=='0')
			{

				lcd_cmd(0x01);
				lcd_cmd(0x80);
				lcd_string("COUPEN_AMOUNT:");
				lcd_string("10");
			   	recharge_amount=recharge_amount+10;
			}

		   gsm_clear();
		   gsm_flag=0;
		   gps_flag=0;
	  }
	  if(gps_flag==1)
	  {
		gsm_clear();
		gps_flag=0;
		IE=0X90;
	  }
	}
}
