#ifndef _RSA
#define _RSA

typedef struct key
{
	long pkey;	/* 秘钥长度 */
	long ekey;	/* 公钥     */
	long dkey;	/* 私钥     */
} Key;

class Rsa
{
public:
	Rsa();
	virtual ~Rsa();

public:
	Key	 produce_keys();											/* 秘钥			*/
	long endecrypt(const long msg, const long key, const long pkey);/* 加密解密		*/

private:
	long produce_pkey(const long prime1, const long prime2);		/* 公钥			*/
	long produce_ekey(const long orla);								/* e 加密秘钥	*/
	long produce_dkey(const long ekey, const long orla);			/* d 解密秘钥	*/
	long produce_prime();											/* 生成随机素数	*/
	long produce_orla(const long prime1, const long prime2);		/* 欧拉函数		*/
	long produce_gcd(const long a, const long b);					/* 最大公约数	*/
	bool is_prime(const long num);									/* 判断素数		*/
};

#endif