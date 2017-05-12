#ifndef _BIGNUM
#define _BIGNUM

/*
 *  ×Ö·û´®×ª»¯
 *
 *
 */

#include <vector>
#include <iostream>
using namespace std;

class Bignum
{
public:
	Bignum();
	virtual ~Bignum();

public:
	void stol(const string, vector<long> &);			/* trans string to long int */
	void show(const vector<long>data);					/* show the result			*/

private:
	void string_to_long(const string, vector<long> &);	/* trans string to long int */
	int  char_to_int(const char);						/* trans char   to int		*/

	long my_pow(const int base, const long n);			/* my pow					*/
};
#endif