#include "bignum.h"

//
Bignum::Bignum()
{
}
//
Bignum::~Bignum()
{
}

void Bignum::show(const vector<long>data)
{
	long len = 0;
	len = (long)data.size();

	for (int i = 0; i < len;i++)
	{
		printf("%6ld ", data[i]);
	}
	cout << endl;
}

void Bignum::stol(const string s, vector<long> &ll) {	/* trans string to long int */
	Bignum::string_to_long(s, ll);
}

void Bignum::string_to_long(const string s, vector<long> & ll)
{
	string str;
	long len = 0;   // str 的长度
	long len_w = 0; // % 4 后剩余的长度

	str = s;
	len = (long)str.size();
	len_w = len % 4;

	long tmp = 0;
	for (int i = 0; i < len - len_w;i = i + 4)
	{
		tmp = 0;
		for (int j = 0;j < 4;j++)
		{
			tmp += (char_to_int(s[i + j]))* my_pow(10, 3 - j);
		}
		ll.push_back(tmp);
	}

	/*
	if (len_w != 0)
	{
		tmp = 0;
		for (int i = len - len_w; i < len;i++)
		{
			tmp += (char_to_int(s[i]))* my_pow(10, (len - i));
		}
		ll.push_back(tmp);
	}
	*/

	if (len_w != 0)
	{
		int p = 0;
		tmp = 0;
		for (int i = 0; i < len_w;i++)
		{
			p = len - len_w + i;
			tmp += (char_to_int(s[p]))* my_pow(10, (len_w - i - 1));
		}
		ll.push_back(tmp);
	}
}

int Bignum::char_to_int(const char c)
{
	switch (c)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default: break;
	}
	return 0;
}

long Bignum::my_pow(const int base, const long n)
{
	long sum = 1;
	for (int i = 0;i < n;i++) {
		sum *= 10;
	}
	return (sum);
}