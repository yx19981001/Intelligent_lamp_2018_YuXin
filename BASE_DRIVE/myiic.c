#include "myiic.h"
#include "delay.h"
//����ָ��IIC������Ҫ�����Ⱥ궨���������� ��������GPIO��
//���ݺ궨���ʼ��
//���ݺ궨�����������л��������������
//���ݺ궨���������������

//#define IIC1_SCL_Pin  GPIO_Pin_6	//SCL
//#define IIC1_SDA_Pin  GPIO_Pin_7	//SDA
//#define GPIO_IIC1     GPIOB



void IIC_Init(void)	//����ʱ�ӳ�ʼ���������滻
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SCL_Pin|IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SCL_Pin|IIC1_SDA_Pin); 	//PB6,PB7 �����
}


void IIC1_SDA_IN(void)//IO��������
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SDA_Pin); 	//PB6,PB7 �����
}


void IIC1_SDA_OUT(void)//IO��������
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   
	GPIO_InitStructure.GPIO_Pin = IIC1_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_IIC1, &GPIO_InitStructure);
	GPIO_SetBits(GPIO_IIC1,IIC1_SDA_Pin); 	//PB6,PB7 �����
}


//����IIC��ʼ�ź�
void IIC_Start(void)
{
	IIC1_SDA_OUT();     //sda�����
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
 	IIC1_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC1_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	IIC1_SDA_OUT();//sda�����
	IIC1_SCL=0;
	IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC1_SDA_IN();      //SDA����Ϊ����  
	IIC1_SDA=1;delay_us(1);	   
	IIC1_SCL=1;delay_us(1);	 
	while(READ_IIC1_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC1_SCL=0;//ʱ�����0 	   
	return 0;  
} 

//����ACKӦ��
void IIC_Ack(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}

//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}	

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC1_SDA_OUT(); 	    
    IIC1_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC1_SDA=1;
		else
			IIC1_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC1_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC1_SCL=0; 
        delay_us(2);
		IIC1_SCL=1;
        receive<<=1;
        if(READ_IIC1_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}