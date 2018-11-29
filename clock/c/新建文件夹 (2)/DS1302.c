/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����DS1302.c
* ��  ����ʵʱʱ��оƬDS1302����ģ��
* �汾�ţ�v1.0.0
* ��  ע��
*******************************************************************************
*/

#define  _DS1302_C
#include "config.h"
#include "DS1302.h"

/* ����һ���ֽڵ�DS1302ͨ�������� */
void DS1302ByteWrite(uint8 dat)
{
    uint8 mask;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ�Ƴ�
    {
        if ((mask&dat) != 0) //���������λ����
            DS1302_IO = 1;
        else
            DS1302_IO = 0;
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    DS1302_IO = 1;           //���ȷ���ͷ�IO����
}
/* ��DS1302ͨ�������϶�ȡһ���ֽ� */
uint8 DS1302ByteRead()
{
    uint8 mask;
    uint8 dat = 0;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ��ȡ
    {
        if (DS1302_IO != 0)  //���ȶ�ȡ��ʱ��IO���ţ�������dat�еĶ�Ӧλ
        {
            dat |= mask;
        }
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    return dat;              //��󷵻ض������ֽ�����
}
/* �õ���д������ĳһ�Ĵ���д��һ���ֽڣ�reg-�Ĵ�����ַ��dat-��д���ֽ� */
void DS1302SingleWrite(uint8 reg, uint8 dat)
{
    DS1302_CE = 1;                   //ʹ��Ƭѡ�ź�
    DS1302ByteWrite((reg<<1)|0x80);  //����д�Ĵ���ָ��
    DS1302ByteWrite(dat);            //д���ֽ�����
    DS1302_CE = 0;                   //����Ƭѡ�ź�
}
/* �õ��ζ�������ĳһ�Ĵ�����ȡһ���ֽڣ�reg-�Ĵ�����ַ������ֵ-�������ֽ� */
uint8 DS1302SingleRead(uint8 reg)
{
    uint8 dat;
    
    DS1302_CE = 1;                   //ʹ��Ƭѡ�ź�
    DS1302ByteWrite((reg<<1)|0x81);  //���Ͷ��Ĵ���ָ��
    dat = DS1302ByteRead();          //��ȡ�ֽ�����
    DS1302_CE = 0;                   //����Ƭѡ�ź�
    
    return dat;
}
/* ��ͻ��ģʽ����д��8���Ĵ������ݣ�dat-��д������ָ�� */
void DS1302BurstWrite(uint8 *dat)
{
    uint8 i;
    
    DS1302_CE = 1;
    DS1302ByteWrite(0xBE);  //����ͻ��д�Ĵ���ָ��
    for (i=0; i<8; i++)     //����д��8�ֽ�����
    {
        DS1302ByteWrite(dat[i]);
    }
    DS1302_CE = 0;
}
/* ��ͻ��ģʽ������ȡ8���Ĵ��������ݣ�dat-��ȡ���ݵĽ���ָ�� */
void  DS1302BurstRead(uint8 *dat)
{
    uint8 i;
    
    DS1302_CE = 1;
    DS1302ByteWrite(0xBF);  //����ͻ�����Ĵ���ָ��
    for (i=0; i<8; i++)     //������ȡ8���ֽ�
    {
        dat[i] = DS1302ByteRead();
    }
    DS1302_CE = 0;
}
/* ��ȡʵʱʱ�䣬����ȡDS1302��ǰʱ�䲢ת��Ϊʱ��ṹ���ʽ */
void GetRealTime(struct sTime *time)
{
    uint8 buf[8];
    
    DS1302BurstRead(buf);
    time->year = buf[6] + 0x2000;
    time->mon  = buf[4];
    time->day  = buf[3];
    time->hour = buf[2];
    time->min  = buf[1];
    time->sec  = buf[0];
    time->week = buf[5];
}
/* �趨ʵʱʱ�䣬ʱ��ṹ���ʽ���趨ʱ��ת��Ϊ���鲢д��DS1302 */
void SetRealTime(struct sTime *time)
{
    uint8 buf[8];
    
    buf[7] = 0;
    buf[6] = time->year;
    buf[5] = time->week;
    buf[4] = time->mon;
    buf[3] = time->day;
    buf[2] = time->hour;
    buf[1] = time->min;
    buf[0] = time->sec;
    DS1302BurstWrite(buf);
}
/* DS1302��ʼ�����緢���������������ó�ʼʱ�� */
void InitDS1302()
{
    uint8 dat;
    struct sTime code InitTime[] = {  0x2018,0x09,0x16, 0x12,0x00,0x00, 0x06};
    
    DS1302_CE = 0;  //��ʼ��DS1302ͨ������
    DS1302_CK = 0;
    dat = DS1302SingleRead(0);  //��ȡ��Ĵ���
    if ((dat & 0x80) != 0)      //����Ĵ������λCH��ֵ�ж�DS1302�Ƿ���ֹͣ
    {
        DS1302SingleWrite(7, 0x00);  //����д����������д������
        SetRealTime(&InitTime);      //����DS1302ΪĬ�ϵĳ�ʼʱ��
    }
}
