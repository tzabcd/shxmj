#include "maj.h"
#include "gui.h"

// ÿ����һ���һ�����ƣ������������׽����,UI���
class play_control
{
	player _my_majs;
public:
	bool chi(maj & own1, maj &own2, maj &eat_one);
	bool peng(maj &mj);
	bool gang(maj &mj);
	void out_hand(maj &mj);
	void hu(); // well designed;
};

class frame
{
	
};

/*
�������� ��ͼ �� ���ƣ�

����  ����   ׽��  �����������Ŀ��ƽ��б仯�� ������Ҫ��ʾ�仯��

���ֿ�����������UI�ؼ�����


���̣�
 �ؼ�������ʱ���������ƣ�
 ������ƣ� ׽�ƣ��ԣ�ѡ�ƣ�ȷ����������ģ�ͣ���ͼ����
 */

