#include "sys.h"
#include "ov2640.h"
#include "ov2640cfg.h"
//#include "timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	
#include "stdio.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//OV2640 Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2014/5/14
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
  
//³õÊ¼»¯OV2640 
//ÅäÖÃÍêÒÔºó,Ä¬ÈÏÊä³öÊÇ1600*1200³ß´çµÄÍ¼Æ¬!! 
//·µ»ØÖµ:0,³É¹¦
//    ÆäËû,´íÎó´úÂë
u8 OV2640_Init(void)
{ 
	u16 i=0;
	u16 reg;
	//ÉèÖÃIO     	   
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOB_CLK_ENABLE();           //Ê¹ÄÜGPIOBÊ±ÖÓ
  //GPIOF9,F10³õÊ¼»¯ÉèÖÃ
  GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_15;//PD6,7 ÍÆÍìÊä³ö
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;  //PD6,7 ÍÆÍìÊä³ö
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;//100MHz
  GPIO_InitStructure.Pull = GPIO_PULLUP;//ÉÏÀ­
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);//³õÊ¼»
	
 	OV2640_PWDN=0;	//POWER ON
	delay_ms(10);
	OV2640_RST=0;	//¸´Î»OV2640
	delay_ms(10);
	OV2640_RST=1;	//½áÊø¸´Î» 
  SCCB_Init();        		//³õÊ¼»¯SCCB µÄIO¿Ú	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//²Ù×÷sensor¼Ä´æÆ÷
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//Èí¸´Î»OV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//¶ÁÈ¡³§¼ÒID ¸ß°ËÎ»
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//¶ÁÈ¡³§¼ÒID µÍ°ËÎ»
	if(reg!=OV2640_MID)
	{
		printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//¶ÁÈ¡³§¼ÒID ¸ß°ËÎ»
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//¶ÁÈ¡³§¼ÒID µÍ°ËÎ»
	if(reg!=OV2640_PID)
	{
		printf("HID:%d\r\n",reg);
		return 2;
	}   
 	//³õÊ¼»¯ OV2640,²ÉÓÃSXGA·Ö±æÂÊ(1600*1200)  
	for(i=0;i<sizeof(ov2640_sxga_init_reg_tbl)/2;i++)
	{
	   	SCCB_WR_Reg(ov2640_sxga_init_reg_tbl[i][0],ov2640_sxga_init_reg_tbl[i][1]);
 	} 
  	return 0x00; 	//ok
} 
//OV2640ÇĞ»»ÎªJPEGÄ£Ê½
void OV2640_JPEG_Mode(void) 
{
	u16 i=0;
	//ÉèÖÃ:YUV422¸ñÊ½
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	
	//ÉèÖÃ:Êä³öJPEGÊı¾İ
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
}
//OV2640ÇĞ»»ÎªRGB565Ä£Ê½
void OV2640_RGB565_Mode(void) 
{
	u16 i=0;
	//ÉèÖÃ:RGB565Êä³ö
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
} 
//×Ô¶¯ÆØ¹âÉèÖÃ²ÎÊı±í,Ö§³Ö5¸öµÈ¼¶
const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,	
		0x24,0x3e,	
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,	
		0x24,0x58,	
		0x25,0x50,	
		0x26,0x92,	
	},
}; 
//OV2640×Ô¶¯ÆØ¹âµÈ¼¶ÉèÖÃ
//level:0~4
void OV2640_Auto_Exposure(u8 level)
{  
	u8 i;
	u8 *p=(u8*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		SCCB_WR_Reg(p[i*2],p[i*2+1]); 
	} 
}  
//°×Æ½ºâÉèÖÃ
//0:×Ô¶¯
//1:Ì«Ñôsunny
//2,ÒõÌìcloudy
//3,°ì¹«ÊÒoffice
//4,¼ÒÀïhome
void OV2640_Light_Mode(u8 mode)
{
	u8 regccval=0X5E;//Sunny 
	u8 regcdval=0X41;
	u8 regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			SCCB_WR_Reg(0XFF,0X00);	 
			SCCB_WR_Reg(0XC7,0X10);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	SCCB_WR_Reg(0XFF,0X00);	 
	SCCB_WR_Reg(0XC7,0X40);	//AWB OFF 
	SCCB_WR_Reg(0XCC,regccval); 
	SCCB_WR_Reg(0XCD,regcdval); 
	SCCB_WR_Reg(0XCE,regceval);  
}
//É«¶ÈÉèÖÃ
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(u8 sat)
{ 
	u8 reg7dval=((sat+2)<<4)|0X08;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0X7C,0X00);		
	SCCB_WR_Reg(0X7D,0X02);				
	SCCB_WR_Reg(0X7C,0X03);			
	SCCB_WR_Reg(0X7D,reg7dval);			
	SCCB_WR_Reg(0X7D,reg7dval); 		
}
//ÁÁ¶ÈÉèÖÃ
//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(u8 bright)
{
  SCCB_WR_Reg(0xff, 0x00);
  SCCB_WR_Reg(0x7c, 0x00);
  SCCB_WR_Reg(0x7d, 0x04);
  SCCB_WR_Reg(0x7c, 0x09);
  SCCB_WR_Reg(0x7d, bright<<4); 
  SCCB_WR_Reg(0x7d, 0x00); 
}
//¶Ô±È¶ÈÉèÖÃ
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(u8 contrast)
{
	u8 reg7d0val=0X20;//Ä¬ÈÏÎªÆÕÍ¨Ä£Ê½
	u8 reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,0x04);
	SCCB_WR_Reg(0x7c,0x07);
	SCCB_WR_Reg(0x7d,0x20);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,0x06);
}
//ÌØĞ§ÉèÖÃ
//0:ÆÕÍ¨Ä£Ê½    
//1,¸ºÆ¬
//2,ºÚ°×   
//3,Æ«ºìÉ«
//4,Æ«ÂÌÉ«
//5,Æ«À¶É«
//6,¸´¹Å	    
void OV2640_Special_Effects(u8 eft)
{
	u8 reg7d0val=0X00;//Ä¬ÈÏÎªÆÕÍ¨Ä£Ê½
	u8 reg7d1val=0X80;
	u8 reg7d2val=0X80; 
	switch(eft)
	{
		case 1://¸ºÆ¬
			reg7d0val=0X40; 
			break;	
		case 2://ºÚ°×
			reg7d0val=0X18; 
			break;	 
		case 3://Æ«ºìÉ«
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://Æ«ÂÌÉ«
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://Æ«À¶É«
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://¸´¹Å
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7c,0x05);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,reg7d2val); 
}
//²ÊÌõ²âÊÔ
//sw:0,¹Ø±Õ²ÊÌõ
//   1,¿ªÆô²ÊÌõ(×¢ÒâOV2640µÄ²ÊÌõÊÇµş¼ÓÔÚÍ¼ÏñÉÏÃæµÄ)
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//ÉèÖÃÍ¼ÏñÊä³ö´°¿Ú 
//sx,sy,ÆğÊ¼µØÖ·
//width,height:¿í¶È(¶ÔÓ¦:horizontal)ºÍ¸ß¶È(¶ÔÓ¦:vertical)
void OV2640_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//¶ÁÈ¡VrefÖ®Ç°µÄÖµ
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//ÉèÖÃVrefµÄstartºÍendµÄ×îµÍ2Î»
	SCCB_WR_Reg(0X19,sy>>2);			//ÉèÖÃVrefµÄstart¸ß8Î»
	SCCB_WR_Reg(0X1A,endy>>2);			//ÉèÖÃVrefµÄendµÄ¸ß8Î»
	
	temp=SCCB_RD_Reg(0X32);				//¶ÁÈ¡HrefÖ®Ç°µÄÖµ
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//ÉèÖÃHrefµÄstartºÍendµÄ×îµÍ3Î»
	SCCB_WR_Reg(0X17,sx>>3);			//ÉèÖÃHrefµÄstart¸ß8Î»
	SCCB_WR_Reg(0X18,endx>>3);			//ÉèÖÃHrefµÄendµÄ¸ß8Î»
}
//ÉèÖÃÍ¼ÏñÊä³ö´óĞ¡
//OV2640Êä³öÍ¼ÏñµÄ´óĞ¡(·Ö±æÂÊ),ÍêÈ«ÓÉ¸Äº¯ÊıÈ·¶¨
//width,height:¿í¶È(¶ÔÓ¦:horizontal)ºÍ¸ß¶È(¶ÔÓ¦:vertical),widthºÍheight±ØĞëÊÇ4µÄ±¶Êı
//·µ»ØÖµ:0,ÉèÖÃ³É¹¦
//    ÆäËû,ÉèÖÃÊ§°Ü
u8 OV2640_OutSize_Set(u16 width,u16 height)
{
	u16 outh;
	u16 outw;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0X5A,outw&0XFF);		//ÉèÖÃOUTWµÄµÍ°ËÎ»
	SCCB_WR_Reg(0X5B,outh&0XFF);		//ÉèÖÃOUTHµÄµÍ°ËÎ»
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//ÉèÖÃOUTH/OUTWµÄ¸ßÎ» 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//ÉèÖÃÍ¼Ïñ¿ª´°´óĞ¡
//ÓÉ:OV2640_ImageSize_SetÈ·¶¨´«¸ĞÆ÷Êä³ö·Ö±æÂÊ´Ó´óĞ¡.
//¸Ãº¯ÊıÔòÔÚÕâ¸ö·¶Î§ÉÏÃæ½øĞĞ¿ª´°,ÓÃÓÚOV2640_OutSize_SetµÄÊä³ö
//×¢Òâ:±¾º¯ÊıµÄ¿í¶ÈºÍ¸ß¶È,±ØĞë´óÓÚµÈÓÚOV2640_OutSize_Setº¯ÊıµÄ¿í¶ÈºÍ¸ß¶È
//     OV2640_OutSize_SetÉèÖÃµÄ¿í¶ÈºÍ¸ß¶È,¸ù¾İ±¾º¯ÊıÉèÖÃµÄ¿í¶ÈºÍ¸ß¶È,ÓÉDSP
//     ×Ô¶¯¼ÆËãËõ·Å±ÈÀı,Êä³ö¸øÍâ²¿Éè±¸.
//width,height:¿í¶È(¶ÔÓ¦:horizontal)ºÍ¸ß¶È(¶ÔÓ¦:vertical),widthºÍheight±ØĞëÊÇ4µÄ±¶Êı
//·µ»ØÖµ:0,ÉèÖÃ³É¹¦
//    ÆäËû,ÉèÖÃÊ§°Ü
u8 OV2640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height)
{
	u16 hsize;
	u16 vsize;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);					
	SCCB_WR_Reg(0X51,hsize&0XFF);		//ÉèÖÃH_SIZEµÄµÍ°ËÎ»
	SCCB_WR_Reg(0X52,vsize&0XFF);		//ÉèÖÃV_SIZEµÄµÍ°ËÎ»
	SCCB_WR_Reg(0X53,offx&0XFF);		//ÉèÖÃoffxµÄµÍ°ËÎ»
	SCCB_WR_Reg(0X54,offy&0XFF);		//ÉèÖÃoffyµÄµÍ°ËÎ»
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//ÉèÖÃH_SIZE/V_SIZE/OFFX,OFFYµÄ¸ßÎ»
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//ÉèÖÃH_SIZE/V_SIZE/OFFX,OFFYµÄ¸ßÎ»
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//¸Ãº¯ÊıÉèÖÃÍ¼Ïñ³ß´ç´óĞ¡,Ò²¾ÍÊÇËùÑ¡¸ñÊ½µÄÊä³ö·Ö±æÂÊ
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:Í¼Ïñ¿í¶ÈºÍÍ¼Ïñ¸ß¶È
//·µ»ØÖµ:0,ÉèÖÃ³É¹¦
//    ÆäËû,ÉèÖÃÊ§°Ü
u8 OV2640_ImageSize_Set(u16 width,u16 height)
{ 
	u8 temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//ÉèÖÃHSIZEµÄ10:3Î»
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//ÉèÖÃVSIZEµÄ10:3Î»
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

















