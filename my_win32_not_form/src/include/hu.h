#pragma once
#include "maj_model.h"

/*

˳�ӣ� ����������
���ӣ� ������ͬ��
���ƣ� ����

���ͣ� ƽ����| ������  | �߶�  | ����  |��ʮ���á�
ƽ���� ˳�� {0,4} ����
������ ������ {0,4} ����
�߶ԣ� ����{7}
������ [�� | ������]{2|5|8|11|14}
ʮ���ã������Ƽ�����2,

ƽ���жϵĹ���
 ������� != 2 5 8 11 14��
  �๫���˳�
 ������� == 2
  ��ͬ��������򲻺�
 ������� > 2
  �ҳ��������п��ܵĿ��Ӻ�˳�ӣ�
  ����Ҳ����������ǹ��ţ�
   �������˳�
  ���ҵ��Ŀ��Ӻ�˳�������
  1���Ӷ���ȡ��һ�����ӻ�˳�ӣ���������ѿգ�����
  2������ɾ��������ӻ�˳�ӣ���ʣ�µ��ƽ����к������ݹ���ô��к����̣�
  3������ܺ�
   ��ӡ������ӻ�˳�ӣ��˳�
  4���������
   ��ȡ���Ŀ��ӻ�˳�����·Ż�
  5����ת��1
*/

class hu
{
protected:
	maj _wildcard;
	maj _new_fetch;
	bool _have_wildcard;
	int _num_pseudos;
public:
	hu(const std::wstring &name) : name(name){};
	std::wstring name;
	virtual bool judge(const player &majs) = 0;
};

class lan : public hu
{
public:
	lan() : hu((wchar_t *)("ʮ����")) {};
	bool judge(const player &majs);	
};

class nine_one : public hu
{
	bool is_91(const maj &mj) 
	{
		int code = mj.get_code();
		if (code % 10 != 1 && code % 10 != 9 && code < 81)
			return false;
		return true;
	}
public:
	nine_one() : hu(TEXT("��һ")) {};
	bool judge(const player &majs);
};

class seven_pairs : public hu
{
	bool exist_pair(const std::vector<maj>&, const maj &);
	void reduce_pair(std::vector<maj> &, const maj &);
	void put_back_pair(std::vector<maj> &, const maj &);
	bool do_judge(std::vector<maj> &judge_set, const maj &wildcard);
public:
	seven_pairs() : hu(TEXT("�߶�")) {};
	bool judge(const player &majs);
};

class pengpeng : public hu
{
	bool do_judge(std::vector<maj> &);
public:
	pengpeng() : hu(TEXT("������")) {_have_wildcard = false;};
	bool judge(const player &majs);
};

class normal : public hu
{
public:
	normal() : hu(TEXT("ƽ��")) {};
	bool judge(const player &majs);
private:
	bool do_judge(std::vector<maj> &judge_set);
	bool exist_shunzi(std::vector<maj> &set, maj &mj);
	bool exist_kanzi(std::vector<maj> &set, maj &mj);
	void delete_shunzi(std::vector<maj> &set, maj &mj);
	void delete_kanzi(std::vector<maj> &set, maj&mj);
	void put_back_shunzi(std::vector<maj> &set, maj &mj);
	void put_back_kanzi(std::vector<maj> &set, maj &mj);
};
