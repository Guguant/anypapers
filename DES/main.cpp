#include <iostream>  
#include <string>
#include <cstdlib>

#include"des.h"

using namespace std;

void show();

//
// 字节编码 1byte = 8bit 一个汉字 = 2byte = 16bit
//

int main()
{
	// 明文
	char *msg = (char*)malloc(500 * sizeof(char));
	memset(msg, 0, 500);

	// 密钥
	char *key = (char*)malloc(8 * sizeof(char));
	memset(key, 0, 8);

	// CrypteWords 保存加密后的数据
	char *CrypteWords = (char*)malloc(500 * sizeof(char));
	memset(CrypteWords,  0, 500);

	// DecipherWords 保存解密后的数据
	char *DecipherWords = (char*)malloc(500 * sizeof(char));
	memset(DecipherWords, 0, 500);

	// 输入
	cout << "输入明文: " << endl;
	cin.getline(msg, 500);
	cout << endl;
	cout << "输入密钥: " << endl;
	cin.getline(key, 8);
	cout << endl;
	show();

	// 显示明文与秘钥
	cout << "明文: " << endl;
	cout << msg << endl << endl;
	cout << "密钥: " << endl;
	cout << key << endl << endl;
	show();

	// len : 明文长度
	int len = 0;
	len = strlen(msg);

	// num:     将明文拆分成 num 份以8字节为单位的数据
	// end_num: 拆成 num 份后，剩余end_num字节
	int num, end_num = 0;
	num = len / 8;
	end_num = len % 8;

	// 不足位补 F
	if (end_num != 0)
	{
		for (int i = len; i < len + 8 - end_num;i++)
		{
			msg[i] = 'F';
		}
	}
	else
	{
		;
	}

	DES des;

	//设置密钥  
	des.SetKey(key, 8);
	//生产子密钥  
	des.ProduceSubKey();

	// 加密，解密过程
	for (int i = 0; i < num + 1;i++)
	{
		// 待加密的 8byte 数据
		char msg_temp[8] = { '0' };
		for (int j = 0;j < 8;j++)
		{
			msg_temp[j] = msg[i * 8 + j];
		}
		
		DES des;
		//设置明文
		des.SetMsg(msg_temp, 8);

		//加密  
		des.Crypte();

		//解密  
		des.Decipher();

		// 将加密数据保存在 emsg 中
		char emsg[8] = { '0' };
		des.OutPutCryptedMsg(emsg);

		// 将解密数据保存在 dmsg 中
		char dmsg[8] = { '0' };
		des.OutPutDecipher(dmsg);

		for (int k = 0;k < 8;k++)
		{
			CrypteWords[i * 8 + k]   = emsg[k];
			DecipherWords[i * 8 + k] = dmsg[k];
		}
	}

	// 清除多余位
	for (int i = len;i < 500;i++)
	{ 
		DecipherWords[i] = '\0';
	}

	cout << "加密: " << endl;
	cout << CrypteWords << endl << endl;

	cout << "解密: " << endl;
	cout << DecipherWords << endl << endl;

	free(key);
	free(msg);
	free(DecipherWords);
	free(CrypteWords);
	
	system("pause");
	return 0;
}

/*
 * profile the show
 */
void show()
{
	for (int i = 0;i < 100;i++)
	{
		cout << "-";
	}
	cout << endl;
} 