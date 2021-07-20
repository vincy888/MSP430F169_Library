/*******************************************************************************
@�ļ�����i2c.c
@˵��   �������������I2C�ĳ�ʼ�����������ݡ��������ݣ�
@����   ��2021��7��12��
@����   ��鱿�
*******************************************************************************/
#include"i2c.h"
#include<msp430f169.h>
unsigned char I2CBuffer[3];
int tx_count;
int rx_count;

/*****************************************************************************
@��������I2CMasterInit
@���ܣ�I2C����ģʽ��ʼ��
@���룺SlaveAddr-�ӻ���ַ��baud-�����ʣ�fen-��Ƶ��HCLK-�ߵ�ƽʱ�䣬LCLK-�͵�ƽʱ��
@�������
@ʹ��ʾ����I2CMasterInit(0x048,38400,0x01,0x18,0x18);//�ӻ���ַΪ0x48��I2Cʱ�� = SMCLK/2��
SCL�ߵ�ƽ���� = 20*I2Cʱ�ӣ�SCL�͵�ƽ����Ϊ20*I2Cʱ��
******************************************************************************/
void I2CMasterInit(unsigned char SlaveAddr,long baud,unsigned char fen,unsigned char HCLK,unsigned char LCLK)
{
    long int brclk; //�����ʷ�����ʱ��Ƶ��
    P3SEL |= 0x0a;//����I2C����p3.1-SDA,p3.3-SCL
    P3DIR &= ~0x0A;
    U0CTL |= I2C + SYNC + MST;//I2C����ģʽ
    U0CTL &= ~I2CEN;//���I2Cʹ��
    I2CSA = SlaveAddr;//�ӻ���ַ
    //ʱ��Դ����
    U0TCTL &= ~(SSEL0 + SSEL1);       //���֮ǰ��ʱ������
    if (baud <= 16364)                 //
    {
        U0TCTL |= SSEL0;              //ACLK�����͹���
        brclk = 32768;                //�����ʷ�����ʱ��Ƶ��=ACLK(32768)
    }
    else
    {
        U0TCTL |= SSEL1;              //SMCLK����֤�ٶ�
        brclk = 1000000;              //�����ʷ�����ʱ��Ƶ��=SMCLK(1MHz)
    }

    //------------------------���ò�����-------------------------
    if (baud < 300 || baud > 115200)   //�����ʳ�����Χ
    {
        return 0;
    }
    I2CPSC = fen;//I2Cʱ��
    I2CSCLH = HCLK;//SCL�ߵ�ƽ����
    I2CSCLL = LCLK;//SCL�͵�ƽ����
     //I2CTCTL |= I2CSSEL1;//ʱ��ԴΪSMCLK
    U0CTL |= I2CEN;//I2Cʹ��
    tx_count = 0;
    rx_count = 0;
}


/*****************************************************************************
@��������I2CWriteByte
@���ܣ���������һ�ֽ��������ӻ�
@���룺Addr-�ӻ���ַ��Val-����
@�������
@ʹ��ʾ����I2CWriteByte(0x48,0x12);
******************************************************************************/
void I2CWriteByte(unsigned char Addr,unsigned char Val)
{
    U0CTL |= MST;//��ģʽ
    I2CTCTL |= I2CTRX ;//����ģʽ
    I2CIFG &= ~TXRDYIFG;// ����жϱ�־
    I2CIE = TXRDYIE;// �����ж�ʹ��
    while(I2CDCTL & I2CBUSY);//�ȴ�I2C����
    I2CBuffer[1] = Addr;
    I2CBuffer[0] = Val;
    tx_count = 1;    // ���û�����ָ��
    I2CNDAT = 0X02;
    I2CTCTL |= I2CSTT + I2CTRX;//��ʼ��ֹͣ��������
}

/*****************************************************************************
@��������I2CReadByte
@���ܣ���ȡ���������
@���룺Addr-�ӻ���ַ
@���������
@ʹ��ʾ����I2CreadByte(0x48);
******************************************************************************/
unsigned char I2CReadByte(unsigned char Addr)
{
     I2CTCTL &= ~I2CTRX;    // ����ģʽ�� R/W Ϊ�� 1
     I2CIE = RXRDYIE;// �����ж�ʹ��
     while(I2CDCTL & I2CBUSY);//�ȴ�I2C����
     U0CTL |= MST;//����ģʽ
     I2CNDAT = 0X02;//���������ֽڵ�����
     I2CIFG &=~ARDYIFG;//����жϱ�־
     I2CTCTL |= I2CSTT + I2CSTP;//��ʼ����
     tx_count = 0;    // ���û�����ָ��
     while((~I2CIFG)&ARDYIFG);//�ȴ��������
     return I2CBuffer[0];//��������

}


/*****************************************************************************
@��������I2CWriteStr
@���ܣ�������ӻ�������ֽ�����
@���룺Addr-�ӻ���ַ��*Val-���ݣ�Length-���ݳ���
@�������
@ʹ��ʾ����char *Val[10] = "hello";
           I2CWriteStr(0x48,*Val,strlen("hello));
******************************************************************************/
unsigned char I2CWriteStr(unsigned char Addr,unsigned char *Val,int Length)
{
    int num = 0;
    int Len = Length - 1;
    for(num = 0;num<Len;num++)
    {
        I2CWriteByte(Addr,*Val);
        while((I2CIFG & 0X02) == NACKIFG)
        {
            I2CTCTL |= I2CSTP;//����Ӧ����������
            return 0;
        }
        Val++;
    }
    I2CWriteByte(Addr,*Val);
    while((I2CIFG & 0x02) == NACKIFG)// �ȴ����ݵ�Ӧ��
    {
        I2CTCTL |= I2CSTP; // ��Ӧ�𣬽�������
        return(0);
    }
    return 1;
}

#pragma vector=USART0TX_VECTOR
__interrupt void ISR_I2C(void)
{
    switch (I2CIV)
    {
        case I2CIV_AL:
        {
            // �ٲ��ж�
            break;
        }
        case I2CIV_NACK:
        {
            //NACK �ж�
            break;
        }
        case I2CIV_OA:
        {
            // �Լ���ַ�ж�
            break;
        }
        case I2CIV_ARDY:
        {
            // ����׼�����ж�
            break;
        }
        case I2CIV_RXRDY:
        {
            // ����׼�����ж�
            I2CBuffer[0]=I2CDRB;
            break;
        }
        case I2CIV_TXRDY:
        {
            // ����׼�����ж�
            I2CDRB = I2CBuffer[tx_count];
            tx_count = tx_count - 1;
            if (tx_count < 0)
            {
                // ��ֹ�����ж�
                I2CIE &= ~TXRDYIE;
            }
            break;
        }
        case I2CIV_GC:
        {
            // һ������ж�
            break;
        }
        case I2CIV_STT:
        {
            // ��ʼ�����ж�
            break;
        }
    }
}

