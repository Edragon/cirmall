/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.h
* ��  ������19�� �๦�ܵ����� �������ļ���ͷ�ļ�
* �汾�ţ�v1.0.0
* ��  ע��
*******************************************************************************
*/

#ifndef _MAIN_H
#define _MAIN_H

enum eStaSystem {  //ϵͳ����״̬ö��
    E_NORMAL,E_SET_Day, E_SET_TIME, E_SET_ALARM
};

#ifndef _MAIN_C

extern enum eStaSystem staSystem;
#endif
  void WriteROM();
void RefreshTemp(uint8 ops);
void ConfigTimer0(uint16 ms);

#endif
