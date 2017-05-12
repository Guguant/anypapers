#include "rsa.h"
#include "bignum.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void test();

int main()
{
	cout << "/***********************************" << endl;
	cout << "* RSA �ǶԳ���Կ�㷨" << endl;
	cout << "* pkey | ekey | dkey " << endl;
	cout << "***********************************/" << endl;

	while (true)
	{
		test();
		cout << "----------------------------------------" << endl;
		getchar();
		if ('q' == getchar() || 'Q' == getchar())/* break */
		{
			break;
		}
	}
	return 0;
}

void test(void)
{
	Rsa rsa;
	Key key = rsa.produce_keys();

	cout << "RSA Keys" << endl;
	cout << "��Կ      : " << key.pkey << endl;
	cout << "������Կ: " << key.ekey << endl;
	cout << "������Կ: " << key.dkey << endl;
	cout << endl;

	string msg;
	cout << "����������ܵ�����: " << endl;
	cin >> msg;
	cout << endl;

	Bignum bignum;
	/* trans msg to lmsg */
	vector<long>lmsg;
	long lmsg_len = 0;

	bignum.stol(msg, lmsg);
	lmsg_len = (long)lmsg.size();

	cout << "ԭʼ: "; bignum.show(lmsg);
	
	long msg_des = 0;
	/* e_lmsg �������� */
	vector<long>e_lmsg;
	for (int i = 0;i < lmsg_len;i++)
	{
		msg_des = rsa.endecrypt(lmsg[i], key.ekey, key.pkey);
		e_lmsg.push_back(msg_des);
	}
	cout << "����: "; bignum.show(e_lmsg);

	/* d_lmsg �������� */
	vector<long>d_lmsg;
	for (int i = 0;i < lmsg_len;i++)
	{
		msg_des = rsa.endecrypt(e_lmsg[i], key.dkey, key.pkey);
		d_lmsg.push_back(msg_des);
	}
	cout << "����: "; bignum.show(d_lmsg);
}