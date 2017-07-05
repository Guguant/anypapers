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
	cout << "* RSA 非对称密钥算法" << endl;
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
	cout << "公钥      : " << key.pkey << endl;
	cout << "加密密钥: " << key.ekey << endl;
	cout << "解密密钥: " << key.dkey << endl;
	cout << endl;

	string msg;
	cout << "请输入待加密的明文: " << endl;
	cin >> msg;
	cout << endl;

	Bignum bignum;
	/* trans msg to lmsg */
	vector<long>lmsg;
	long lmsg_len = 0;

	bignum.stol(msg, lmsg);
	lmsg_len = (long)lmsg.size();

	cout << "原始: "; bignum.show(lmsg);
	
	long msg_des = 0;
	/* e_lmsg 加密数据 */
	vector<long>e_lmsg;
	for (int i = 0;i < lmsg_len;i++)
	{
		msg_des = rsa.endecrypt(lmsg[i], key.ekey, key.pkey);
		e_lmsg.push_back(msg_des);
	}
	cout << "加密: "; bignum.show(e_lmsg);

	/* d_lmsg 解密数据 */
	vector<long>d_lmsg;
	for (int i = 0;i < lmsg_len;i++)
	{
		msg_des = rsa.endecrypt(e_lmsg[i], key.dkey, key.pkey);
		d_lmsg.push_back(msg_des);
	}
	cout << "解密: "; bignum.show(d_lmsg);
}