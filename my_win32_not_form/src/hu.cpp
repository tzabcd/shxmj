#include "hu.h"
#include <algorithm>
#include <map>
#include <queue>
using namespace std;

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
bool compare_maj(const maj &lhs, const maj &rhs)
{
	return lhs._code < rhs._code;
}

static void reduce_single(vector<maj> &judge_set, const maj &mj)
{
	for (int i = 0; i != judge_set.size(); i++) {
		if (mj == judge_set[i]) {
			judge_set.erase(judge_set.begin() + i);
			break;
		}
	}
}

static void reduce_pair(vector<maj> &judge_set, const maj &mj)
{
	reduce_single(judge_set, mj);
	reduce_single(judge_set, mj);
}

static void reduce_shunzi(vector<maj> &judge_set, const maj &mj)
{
	reduce_single(judge_set, mj);
	reduce_single(judge_set, mj + 1);
	reduce_single(judge_set, mj + 2);
}

static void reduce_kanzi(vector<maj> &judge_set, const maj &mj)
{
	reduce_single(judge_set, mj);
	reduce_single(judge_set, mj);
	reduce_single(judge_set, mj);
}

static int count_of_maj_in_set(vector<maj> &judge_set, const maj &mj)
{
	int count = 0;
	for (int i = 0; i != judge_set.size(); i++)
		if (judge_set[i] == mj)
			count++;
	return count;
}

bool lan::judge(const player &p)
{
	std::vector<maj> judge_set = p._majs;
	if (judge_set.size() != 14)
		return false;
	sort(judge_set.begin(), judge_set.end(), compare_maj);
	int i;
	for (i = 0; i != 13; i++)
		if (judge_set[i + 1] - judge_set[i] < 3)
			break;

	if (i == 13)
		return true;
	
	vector<maj> judge_set_no_wildcard;
	for (int i = 0; i != 14; i++) {
		if (judge_set[i] == p.get_wildcard())
			continue;
		judge_set_no_wildcard.push_back(judge_set[i]);
	}
	for (int i = 0; i != judge_set_no_wildcard.size() - 1; i++)
		if (judge_set[i + 1] - judge_set[i] < 3)
			return false;

	bool have_wildcard = judge_set_no_wildcard.size() == 14 ? false : true;
	if (have_wildcard && p._num_wild_hearts == 0)
		return false;

	return true;
}

bool nine_one::judge(const player &p)
{
		std::vector<maj> judge_set = p._majs;
		bool have_wildcard = false;
		if (judge_set.size() % 3 != 2) return false;
		for (int i = 0; i != judge_set.size(); i++) {
			if (judge_set[i] == p.get_wildcard()) {
				have_wildcard = true;
				continue;
			}
			if (!is_91(judge_set[i]))
				return false;
		}
		if (!p._chi.empty())
			return false;
		for (int i = 0; i != p._peng.size(); i++) {
			if (!is_91(p._peng[i]))
				return false;
		}
		for (int i = 0; i != p._gang.size(); i++) {
			if (!is_91(p._gang[i]))
				return false;
		}
		if (judge_set.size() != 14 && have_wildcard && p._num_wild_hearts == 0)
			return false;

		return true;
}

bool seven_pairs::judge(const player &p)
{
	std::vector<maj> judge_set = p._majs;
	if (judge_set.size() != 14) return false;
	return do_judge(judge_set, p.get_wildcard());
};

bool seven_pairs::do_judge(vector<maj> &judge_set, const maj &wildcard)
{
	if (judge_set.empty())
		return true;

	queue<maj> pairs;
	int num_wildcards = 0;
	for (int i = 0; i != judge_set.size(); i++) {
		if (judge_set[i] == wildcard) {
			num_wildcards++;
			continue;
		}
		if (exist_pair(judge_set, judge_set[i]))
			pairs.push(judge_set[i]);
	}
	for (int i = 0; i != num_wildcards / 2; i++)
		pairs.push(wildcard);

	while (!pairs.empty()) {
		maj pair_mj = pairs.front();
		pairs.pop();
		reduce_pair(judge_set, pair_mj);
		if (do_judge(judge_set, wildcard)) {
			if (num_wildcards > 0) 
				return true;// seven pair ������ԭ *_*
			else 
				return true; // ��ͨ��ͷ
		}
		if (pair_mj == wildcard) {
			put_back_pair(judge_set, pair_mj);
		}
	}

	num_wildcards = 0;
	int num_singles = 0;
	queue<maj> singles;
	for (int i = 0; i != judge_set.size(); i++) {
		if (judge_set[i] == wildcard) {
			num_wildcards++;
			continue;
		}
		if (!exist_pair(judge_set, judge_set[i]))
			num_singles ++;
	}
	
	if (num_singles > num_wildcards)
		return false;
	else {
		/* if (new_fetch mj not in singles && no wild hearts)
			return false;
		*/
		return true;
	}
}

void seven_pairs::reduce_pair(vector<maj> &judge_set, const maj &mj)
{
	for (int d = 0; d != 2; d++) {
		for (int i = 0; i != judge_set.size(); i++) {
			if (judge_set[i] == mj) {
				judge_set.erase(judge_set.begin() + i);
				break;
			}
		}
	}
}

void seven_pairs::put_back_pair(vector<maj> &judge_set, const maj &mj)
{
	judge_set.push_back(mj);
	judge_set.push_back(mj);
}

bool seven_pairs::exist_pair(const vector<maj> &judge_set, const maj &mj)
{
	int count_mj = 0;
	for (int i = 0; i != judge_set.size(); i++)
		if (judge_set[i] == mj)
			count_mj++;
	return count_mj >= 2;
}

bool pengpeng::judge(const player &p)
{
	_num_pseudos = 0;
	_have_wildcard = false;
	_wildcard = p.get_wildcard();
	_new_fetch = p.get_new_fetch();

	vector<maj> judge_set = p._majs;
	if (judge_set.size() % 3 != 2)
		return false;
	if (p._chi.size() > 0)
		return false;
	
	for (int i = 0; i != judge_set.size(); i++)
		if (judge_set[i] == p.get_wildcard())
			_have_wildcard = true;
	return do_judge(judge_set);
}

bool pengpeng::do_judge(vector<maj> &judge_set)
{
	queue<maj> kanzi;
	queue<maj> pairs;
	queue<maj> singles;

	if (judge_set.size() == 2) {
		if (judge_set[0] == judge_set[1]) {
			if (_have_wildcard && _num_pseudos == 0) {
				hu::name = TEXT("�����������ޱ�");
				return true; // ������ ������ԭ
			} else if (_have_wildcard) {
				hu::name = TEXT("�������б�");
				return true; // ������ �б�
			} else {
				hu::name = TEXT("��������ץ");
				return true; // ��ץ
			}
		} else if (judge_set[0] == _wildcard || judge_set[1] == _wildcard) {
			if (_new_fetch == _wildcard) {
				hu::name = TEXT("�������б�");
				return true; //������ �б�
			} if (_new_fetch != judge_set[0] && _new_fetch != judge_set[1]) {
				hu::name = TEXT("�������б�");
				return true;// ������ �б�
			} else {
				hu::name = TEXT("��������ͷ");
				return true; // ��ͷ
			}
		}
	}

	int num_wildcards = 0;
	for (int i = 0; i != judge_set.size(); i++) {
		if (judge_set[i] == _wildcard) {
			num_wildcards++;
			continue;
		}
		if (count_of_maj_in_set(judge_set, judge_set[i]) == 3)
			kanzi.push(judge_set[i]);
	}
	for (int i = 0; i != judge_set.size(); i++) {
		if (judge_set[i] == _wildcard)
			continue;
		if (count_of_maj_in_set(judge_set, judge_set[i]) == 2)
			pairs.push(judge_set[i]);
	}
	for (int i = 0; i != judge_set.size(); i++) {
		if (judge_set[i] == _wildcard)
			continue;
		if (count_of_maj_in_set(judge_set, judge_set[i]) == 1)
			singles.push(judge_set[i]);
	}
	switch (num_wildcards) {
	case 2:
		pairs.push(_wildcard);
		break;
	case 3:
		pairs.push(_wildcard);
		kanzi.push(_wildcard);
		break;
	case 4:
		pairs.push(_wildcard);
		pairs.push(_wildcard);
		kanzi.push(_wildcard);
	}

	while (!kanzi.empty()) {
		maj mj = kanzi.front();
		kanzi.pop();
		reduce_kanzi(judge_set, mj);
		if (do_judge(judge_set))
			return true;
		judge_set.push_back(mj);
		judge_set.push_back(mj);
		judge_set.push_back(mj);
	}

	while (!pairs.empty()) {
		maj mj = pairs.front();
		pairs.pop();
		if (num_wildcards == 0)
			return false;
		reduce_pair(judge_set, mj);
		reduce_single(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return true;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(mj);
		judge_set.push_back(_wildcard);
	}

	while (!singles.empty()) {
		maj mj = singles.front();
		singles.pop();
		if (num_wildcards < 2)
			return false;
		reduce_single(judge_set, mj);
		reduce_pair(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return true;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(_wildcard);
		judge_set.push_back(_wildcard);
	}
	return false;
}

bool normal::judge(const player &p)
{
	_num_pseudos = 0;
	_have_wildcard = false;
	_wildcard = p.get_wildcard();
	_new_fetch = p.get_new_fetch();

	std::vector<maj> judge_set = p._majs;
	_wildcard = p.get_wildcard();
	if (judge_set.size() % 3 != 2) 
		return false;
	for (int i = 0; i != judge_set.size(); i++)
		if (judge_set[i] == p.get_wildcard())
			_have_wildcard = true;
	return do_judge(judge_set);
}
	
bool normal::do_judge(std::vector<maj> &judge_set)
{
	//if (judge_set.size() == 2)
	//	return judge_set[0] == judge_set[1];
	if (judge_set.size() == 2) {
		hu::name = TEXT("ƽ��");
		return true;
		if (judge_set[0] == judge_set[1]) {
			if (_have_wildcard && _num_pseudos == 0) {
				hu::name = TEXT("ƽ�������ޱ�");
				return true; // ������ԭ
			} else if (_have_wildcard) {
				hu::name = TEXT("ƽ���б�");
				return true; // �б�
			} else {
				hu::name = TEXT("ƽ���ޱ�");
				return true; // ��ץ
			}
		} else if (judge_set[0] == _wildcard || judge_set[1] == _wildcard) {
			if (_new_fetch == _wildcard) {
				hu::name = TEXT("ƽ���б�");
				return true;
			} else if (_new_fetch != judge_set[0] && _new_fetch != judge_set[1]) {
				hu::name = TEXT("ƽ���б�");
				return true;// �б�
			} else {
				hu::name = TEXT("��ͷ");
				return true; // ��ͷ
			}
		}
	}

	queue<maj> shunzi;
	queue<maj> kanzi;
	queue<maj> shunzi12;
	queue<maj> shunzi13;
	queue<maj> pairs;
	queue<maj> singles;
	int num_wildcards = 0;

	for (int i = 0; i != judge_set.size(); i++) {
		int distance = _wildcard - judge_set[i];
		if (distance < 0 || distance > 2) {
			if (count_of_maj_in_set(judge_set, judge_set[i] + 1) >= 1 
				&& count_of_maj_in_set(judge_set, judge_set[i] + 2) >= 1)
				shunzi.push(judge_set[i]);
			if (count_of_maj_in_set(judge_set, judge_set[i] + 2) >= 1)
				shunzi13.push(judge_set[i]);
		}
		if (distance < 0 || distance > 1 && count_of_maj_in_set(judge_set, judge_set[i] + 1) >= 1)
			shunzi12.push(judge_set[i]);
		if (distance != 0) {
			if (count_of_maj_in_set(judge_set, judge_set[i]) >= 3)
				kanzi.push(judge_set[i]);
			if (count_of_maj_in_set(judge_set, judge_set[i]) >= 2)
				pairs.push(judge_set[i]);
			singles.push(judge_set[i]);
		}
	}

	for (int i = 0; i != judge_set.size(); i++) {
		int distance = _wildcard - judge_set[i];
		if (distance >= 0 && distance <= 2) {
			if (count_of_maj_in_set(judge_set, judge_set[i] + 1) >= 1
				&& count_of_maj_in_set(judge_set, judge_set[i] + 2) >= 1)
				shunzi.push(judge_set[i]);
		}
		if (distance == 0) {
			num_wildcards++;
			if (count_of_maj_in_set(judge_set, judge_set[i]) >= 3)
				kanzi.push(judge_set[i]);
			if (count_of_maj_in_set(judge_set, judge_set[i]) >= 2)
				pairs.push(judge_set[i]);
		}
	}
	
	while (!shunzi.empty()) {
		maj &mj = shunzi.front();
		shunzi.pop();
		reduce_shunzi(judge_set, mj);
		if (!do_judge(judge_set))
			put_back_shunzi(judge_set, mj);
		else {
			return true; // celebrate�� 
		}
	}

	while (!kanzi.empty()) {
		maj &mj = kanzi.front();
		kanzi.pop();
		delete_kanzi(judge_set, mj);
		if (do_judge(judge_set))
			return true;
		put_back_kanzi(judge_set, mj);
	}

	if (num_wildcards == 0)
		return false;

	while (!shunzi12.empty()) {
		maj mj = shunzi12.front();
		shunzi12.pop();
		reduce_single(judge_set, mj);
		reduce_single(judge_set, mj + 1);
		reduce_single(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return true;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(mj + 1);
		judge_set.push_back(_wildcard);
	}

	while (!shunzi13.empty()) {
		maj mj = shunzi13.front();
		shunzi13.pop();
		reduce_single(judge_set, mj);
		reduce_single(judge_set, mj + 2);
		reduce_single(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return true;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(mj+2);
		judge_set.push_back(_wildcard);
	}
	
	while (!pairs.empty()) {
		maj mj = pairs.front();
		pairs.pop();
		reduce_pair(judge_set, mj);
		reduce_single(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return true;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(mj);
		judge_set.push_back(_wildcard);
	}

	if (num_wildcards < 2)
		return false;

	while (!singles.empty()) {
		maj mj = singles.front();
		singles.pop();
		reduce_single(judge_set, mj);
		reduce_pair(judge_set, _wildcard);
		_num_pseudos++;
		if (do_judge(judge_set))
			return false;
		_num_pseudos--;
		judge_set.push_back(mj);
		judge_set.push_back(_wildcard);
		judge_set.push_back(_wildcard);
	}

	return false;
}
bool normal::exist_shunzi(std::vector<maj> &set, maj &mj)
{
	int	count_of_next_mj = 0, and_next_next = 0;
	for (int i = 0; i != set.size(); i++) {
		if (set[i] - mj == 1)
			count_of_next_mj++;
		if (set[i] - mj == 2)
			and_next_next++;
	}
	return count_of_next_mj * and_next_next != 0;
}

bool normal::exist_kanzi(std::vector<maj> &set, maj &mj)
{
	int count_of_mj = 0;
	for (int i = 0; i != set.size(); i++) {
		if (set[i] == mj)
			count_of_mj++;
	}
	return count_of_mj > 2;
}

void normal::delete_shunzi(std::vector<maj> &set, maj &mj)
{
	for (int i = 0; i != 3; i++) {
		maj del_mj = maj(mj.get_code() + i);
		for (std::vector<maj>::iterator it = set.begin(); it != set.end(); it++) {
			if (*it == del_mj) {
					set.erase(it);
					break;
			}
		}
	}
}
void normal::delete_kanzi(std::vector<maj> &set, maj&mj)
{
	for (int i = 0; i != 3; i++) {
		for (std::vector<maj>::iterator it = set.begin(); it != set.end(); it++) {
			if (*it == mj) {
				set.erase(it);
				break;
			}
		}
	}
}
void normal::put_back_shunzi(std::vector<maj> &set, maj&mj)
{
	set.push_back(mj);
	set.push_back(mj + 1);
	set.push_back(mj + 2);
	sort(set.begin(), set.end(), compare_maj);
}
void normal::put_back_kanzi(std::vector<maj> &set, maj &mj)
{
	set.push_back(mj);
	set.push_back(mj);
	set.push_back(mj);
	sort(set.begin(), set.end(), compare_maj);
}