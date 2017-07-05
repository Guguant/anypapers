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
* 加解密函数 【模幂运算】
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
* 生成钥匙
* *********************************/
Key Rsa::produce_keys()
{
	long prime1 = produce_prime();		/* 随机质数 prime1 */
	long prime2 = produce_prime();		/* 随机质数 prime2 */
	while ( prime2 == prime1 )
		prime2 = produce_prime();

	Key key;
	long orla = produce_orla(prime1, prime2);	/* 欧拉数 orla = (prime1 - 1) * (prime2 - 1) */
	key.pkey  = produce_pkey(prime1, prime2);	/* 秘钥长度		pkey	*/
	key.ekey  = produce_ekey(orla);				/* 生成加密秘钥 ekey	*/
	key.dkey  = produce_dkey(key.ekey, orla);	/* 生成解密秘钥	dkey	*/
	return key;
}

/************************************
*	生成公钥
* *********************************/
long Rsa::produce_pkey(const long prime1, const long prime2)
{
	return prime1 * prime2;
}

/************************************
*	生成欧拉数
* *********************************/
long Rsa::produce_orla(const long prime1, const long prime2)
{
	return (prime1 - 1) * (prime2 - 1);
}

/************************************
*	生成加密密钥
* *********************************/
long Rsa::produce_ekey(const long orla)
{
	/* ekey与 orla 互质 */
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
*	生成解密钥匙
* *********************************/
long Rsa::produce_dkey(const long ekey, const long orla)
{
	/* dkey 为ekey的模反元素 */
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
*	产生随机素数，范围为[100, 200)
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
* 产生两个数的最大公约数 gcd
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
* 判断是否为素数
* *********************************/
bool Rsa::is_prime(const long num)
{
	//两个较小数另外处理  
	if (num == 2 || num == 3)
		return true;

	//不在6的倍数两侧的一定不是质数  质数可能取值 6k - 1, 6k + 1
	if (num % 6 != 1 && num % 6 != 5)
		return false;

	long tmp = (long)sqrt(num);

	//在6的倍数两侧的也可能不是质数  
	for (int i = 5;i <= tmp; i += 6)
		if (num %i == 0 || num % (i + 2) == 0)
			return false;

	//排除所有，剩余的是质数
	return true;
}