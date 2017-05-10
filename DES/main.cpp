#include <iostream>  
#include <string>
#include <cstdlib>

#include"des.h"

using namespace std;

void show();

//
// �ֽڱ��� 1byte = 8bit һ������ = 2byte = 16bit
//

int main()
{
	// ����
	char *msg = (char*)malloc(500 * sizeof(char));
	memset(msg, 0, 500);

	// ��Կ
	char *key = (char*)malloc(8 * sizeof(char));
	memset(key, 0, 8);

	// CrypteWords ������ܺ������
	char *CrypteWords = (char*)malloc(500 * sizeof(char));
	memset(CrypteWords,  0, 500);

	// DecipherWords ������ܺ������
	char *DecipherWords = (char*)malloc(500 * sizeof(char));
	memset(DecipherWords, 0, 500);

	// ����
	cout << "��������: " << endl;
	cin.getline(msg, 500);
	cout << endl;
	cout << "������Կ: " << endl;
	cin.getline(key, 8);
	cout << endl;
	show();

	// ��ʾ��������Կ
	cout << "����: " << endl;
	cout << msg << endl << endl;
	cout << "��Կ: " << endl;
	cout << key << endl << endl;
	show();

	// len : ���ĳ���
	int len = 0;
	len = strlen(msg);

	// num:     �����Ĳ�ֳ� num ����8�ֽ�Ϊ��λ������
	// end_num: ��� num �ݺ�ʣ��end_num�ֽ�
	int num, end_num = 0;
	num = len / 8;
	end_num = len % 8;

	// ����λ�� F
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

	//������Կ  
	des.SetKey(key, 8);
	//��������Կ  
	des.ProduceSubKey();

	// ���ܣ����ܹ���
	for (int i = 0; i < num + 1;i++)
	{
		// �����ܵ� 8byte ����
		char msg_temp[8] = { '0' };
		for (int j = 0;j < 8;j++)
		{
			msg_temp[j] = msg[i * 8 + j];
		}
		
		DES des;
		//��������
		des.SetMsg(msg_temp, 8);

		//����  
		des.Crypte();

		//����  
		des.Decipher();

		// ���������ݱ����� emsg ��
		char emsg[8] = { '0' };
		des.OutPutCryptedMsg(emsg);

		// ���������ݱ����� dmsg ��
		char dmsg[8] = { '0' };
		des.OutPutDecipher(dmsg);

		for (int k = 0;k < 8;k++)
		{
			CrypteWords[i * 8 + k]   = emsg[k];
			DecipherWords[i * 8 + k] = dmsg[k];
		}
	}

	// �������λ
	for (int i = len;i < 500;i++)
	{ 
		DecipherWords[i] = '\0';
	}

	cout << "����: " << endl;
	cout << CrypteWords << endl << endl;

	cout << "����: " << endl;
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