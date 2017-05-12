#include "rsa.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <cassert>

using namespace std;

Rsa::Rsa()
{
}

Rsa::~Rsa()
{
}

/************************************
* �ӽ��ܺ��� ��ģ�����㡿
* *********************************/
//long Rsa::endecrypt(const long msg, const long key, const long pkey)
//{
//	long msg_des = 1;
//	long cm      = msg;
//	long edkey   = key;
//	while (edkey)
//	{
//		if (edkey & 1)
//			msg_des = (msg_des * cm) % pkey;
//		edkey >>= 1;
//		cm = (cm * cm) % pkey;
//	}
//	return msg_des;
//}
long Rsa::endecrypt(const long msg, const long key, const long pkey)
{
	long a = msg;
	long b = key;
	long c = pkey;
	assert(a && (b >= 0) && c); // assert

	long tmp;
	if (0 == b)
	{
		return 1;
	}
	else
	{
		if (b & 1) // odd
		{
			tmp = a * Rsa::endecrypt(a, b - 1, c) % c;
			return tmp;
		}
		else // even
		{
			tmp = Rsa::endecrypt(a, b / 2, c);
			return (tmp * tmp % c);
		}
	}
}

/************************************
* ����Կ��
* *********************************/
Key Rsa::produce_keys()
{
	long prime1 = produce_prime();		/* ������� prime1 */
	long prime2 = produce_prime();		/* ������� prime2 */
	while ( prime2 == prime1 )
		prime2 = produce_prime();

	Key key;
	long orla = produce_orla(prime1, prime2);	/* ŷ���� orla = (prime1 - 1) * (prime2 - 1) */
	key.pkey  = produce_pkey(prime1, prime2);	/* ��Կ����		pkey	*/
	key.ekey  = produce_ekey(orla);				/* ���ɼ�����Կ ekey	*/
	key.dkey  = produce_dkey(key.ekey, orla);	/* ���ɽ�����Կ	dkey	*/
	return key;
}

/************************************
*	���ɹ�Կ
* *********************************/
long Rsa::produce_pkey(const long prime1, const long prime2)
{
	return prime1 * prime2;
}

/************************************
*	����ŷ����
* *********************************/
long Rsa::produce_orla(const long prime1, const long prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

/************************************
*	���ɼ�����Կ
* *********************************/
long Rsa::produce_ekey(const long orla)
{
	/* ekey�� orla ���� */
	long ekey;

	while (true)
	{
		ekey = rand() % orla;
		if (ekey >= 2 && produce_gcd(ekey, orla) == 1)
			break;
	}
	return ekey;
}

/************************************
*	���ɽ���Կ��
* *********************************/
long Rsa::produce_dkey(const long ekey, const long orla)
{
	/* dkey Ϊekey��ģ��Ԫ�� */
	long dkey = orla / ekey;
	while (true)
	{
		if (((dkey * ekey) % orla) == 1)
			break;
		else
			++dkey;
	}
	return dkey;
}

/************************************
*	���������������ΧΪ[100, 200)
* *********************************/
long Rsa::produce_prime()
{
	long prime = 0;
	srand((unsigned)time(0));
	while (true)
	{
		prime = rand() % 100 + 100;
		if (is_prime(prime))
			break;
	}
	return prime;
}

/************************************
* ���������������Լ�� gcd
* *********************************/
long Rsa::produce_gcd(const long a, const long b)
{
	long dividend = a;
	long divisor  = b;
	long residual = dividend % divisor;
	while (residual)
	{
		dividend = divisor;
		divisor  = residual;
		residual = dividend % divisor;
	}
	return divisor;
}

/************************************
* �ж��Ƿ�Ϊ����
* *********************************/
bool Rsa::is_prime(const long num)
{
	//������С�����⴦��  
	if (num == 2 || num == 3)
		return true;

	//����6�ı��������һ����������  ��������ȡֵ 6k - 1, 6k + 1
	if (num % 6 != 1 && num % 6 != 5)
		return false;

	long tmp = (long)sqrt(num);

	//��6�ı��������Ҳ���ܲ�������  
	for (int i = 5;i <= tmp; i += 6)
		if (num %i == 0 || num % (i + 2) == 0)
			return false;

	//�ų����У�ʣ���������
	return true;
}