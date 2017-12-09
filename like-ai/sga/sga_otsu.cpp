/*
 * �����Ŵ��㷨��ʵ��ͼ��ǰ���ͱ�����ָ�
 *
 *
 *
 */


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <list>
#include <algorithm> 

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define SIZE 64			// ��Ⱥ��ģ
#define GeneticSize 8	// �����С
#define Pc   0.95		// ������
#define Pm   0.05		// ������
#define T    10000		// ��Ⱥ����

// Ⱦɫ����Ϣ
struct ga {
	int threshold;  // ��ֵ
	double fitness; // ��Ӧ��
};

// ��Ӧ�Ⱥ���
double otsu(const Mat src, int threshold); 
// ������Ӧ��
void CalculateFitnessValue(const Mat src, struct ga  s[SIZE]);
// �����Ӧ��
int BestFitness(struct ga s0[SIZE]);
// ѡ��-����
void choose_copy(struct ga s0[SIZE], struct ga s1[SIZE]);
// ����λ��
void cross_pos(int x, int y, int pos, int end, struct ga s1[SIZE]);
// ����
void cross(struct ga s1[SIZE], struct ga s2[SIZE]);
// ����
void Mutation(struct ga s2[SIZE], struct ga s3[SIZE]);
// ��ʮ����ת��Ϊ������
void DecimalToBinary(const int n, int a[8]);
// ��������ת��Ϊʮ����
int BinaryToDecimal(const int arr[], int bit);
// һ��Ⱥ�己����һ��Ⱥ��
void NextGeneric(struct ga FA[SIZE], struct ga SON[SIZE]);
// ���Ŵ��㷨
int sga(const Mat src);


// �Ҷȷָ�
void single_channel(const Mat src, Mat &Dst);
// ��ͨ���ָ�
void three_channel(const Mat src, Mat &Dst);


int main(int argc, uchar* argv[])
{
	string image_name;
	cin >> image_name;
	Mat SrcImg = imread(image_name);
	Mat Dst;

	// ��ͨ��
	if (SrcImg.channels() == 1) {
		single_channel(SrcImg, Dst);
	}
	// ��ͨ��
	else if (SrcImg.channels() == 3) {
		three_channel(SrcImg, Dst);
	}
	else { ; }

	if (!Dst.empty())
	{
		imshow("After Simple Genetic Algorithm", Dst);
	}

	string ss = "C:\\Users\\sunyiming\\Desktop\\lena.tiff";
	imwrite(ss, Dst);

	getchar();
	waitKey(0);
	system("pause");
	return 0;
}

// �Ҷȷָ�
void single_channel(const Mat src, Mat & Dst)
{
	Mat src_gray;

	cvtColor(src, src_gray, CV_RGB2GRAY);

	int thresh = 0;

	if (!src_gray.empty())
	{
		thresh = sga(src_gray);
		cout << "���Ž�: " << thresh << endl;
	}
	// �ָ�
	threshold(src_gray, Dst, thresh, 255, CV_THRESH_BINARY);
}

// ��ͨ���ָ�
void three_channel(const Mat src, Mat &Dst)
{
	vector<Mat>mv;
	split(src, mv);

	imshow("R", mv[0]);
	imshow("G", mv[1]);
	imshow("B", mv[2]);

	// get thresh
	int threshR, threshG, threshB = 0;
	threshR = sga(mv[0]);
	threshG = sga(mv[1]);
	threshB = sga(mv[2]);

	cout << "R:" << threshR << " G: " << threshG << " B: " << threshB << endl;

	threshold(mv[0], mv[0], threshR, 255, CV_THRESH_BINARY);
	threshold(mv[1], mv[1], threshG, 255, CV_THRESH_BINARY);
	threshold(mv[2], mv[2], threshB, 255, CV_THRESH_BINARY);

	imshow("R1", mv[0]);
	imshow("G1", mv[1]);
	imshow("B1", mv[2]);

	//save
	vector<Mat>mv_back;
	split(src, mv_back);

	// ԭʼ
	imwrite("C:\\Users\\sunyiming\\Desktop\\R.tiff", mv_back[0]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\G.tiff", mv_back[1]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\B.tiff", mv_back[2]);

	// ��ֵ����
	imwrite("C:\\Users\\sunyiming\\Desktop\\R1.tiff", mv[0]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\G1.tiff", mv[1]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\B1.tiff", mv[2]);

	merge(mv, Dst);

	mv.clear();
	mv_back.clear();
}

// ��Ӧ�Ⱥ���
double otsu(const Mat src, int threshold)
{
	int height = src.rows;
	int width  = src.cols;
	int size   = height * width;
	unsigned char* data = src.data;


	// histogram    
	long histogram[256] = { 0 };

	// histogram ����ÿ���ض�����ֵ��С�ĸ���
	for (int i = 0; i < height; i++)
	{
		unsigned char* p = data + i * src.step;
		for (int j = 0; j < width; j++)
		{
			histogram[int(*p++)]++;
		}
	}

	/*
	* sum0:ǰ���ĻҶ��ܺ�     sum1:�����Ҷ��ܺ�
	* cnt0:ǰ�����ص��ܸ���   cnt1:�������ص��ܸ���
	* w0:  ǰ�����ظ���ռ����ͼ�����صı���
	* w1:  �������ظ���ռ����ͼ�����صı���
	* u0:  ǰ����ƽ���Ҷ�     u1: ������ƽ���Ҷ�
	* variance: ��䷽��
	*/
	long sum0 = 0, sum1 = 0;
	long cnt0 = 0, cnt1 = 0;
	double w0 = 0, w1 = 0;
	double u0 = 0, u1 = 0;
	double variance = 0;

	// ��ʼ��
	sum0 = 0;  sum1 = 0;
	cnt0 = 0;  cnt1 = 0;
	u0 = 0;  u1 = 0;
	w0 = 0;  w1 = 0;

	// ǰ��
	for (int j = 0; j < threshold; j++)
	{
		cnt0 += histogram[j];
		sum0 += (long)(j * histogram[j]);
	}

	// u0:ǰ��ƽ���Ҷ�  w0:ǰ�����ص�����ռȫ�����ص�ı���
	u0 = (double)(sum0 * 1.0 / cnt0);
	w0 = (double)(cnt0 * 1.0 / size);


	// ����
	for (int j = threshold; j <= 255; j++)
	{
		cnt1 += histogram[j];
		sum1 += (long)(j * histogram[j]);
	}

	// u1:����ƽ���Ҷ�  w1:�������ص���ռȫ�����ص�ı���
	u1 = (double)(sum1 * 1.0 / cnt1);
	w1 = 1 - w0;

	// variancn:��䷽��
	variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
	return (variance);
}

// ������Ӧ��
void CalculateFitnessValue(const Mat src, struct ga  s[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		s[i].fitness = otsu(src, s[i].threshold);
	}
}

// �����Ӧ��
int BestFitness(struct ga s0[SIZE])
{
	// fitness_val: Ŀ����ֵ MaxFitness: �����Ӧ��
	int fitness_val = 0;
	double MaxFitness = 0;

	for (int i = 0;i < SIZE;i++)
	{
		if (s0[i].fitness > MaxFitness)
		{
			MaxFitness = s0[i].fitness;
			fitness_val = s0[i].threshold;
		}
	}

	return (fitness_val);
}

// ѡ��-����
void choose_copy(struct ga s0[SIZE], struct ga s1[SIZE])
{
	// ǰ׺����Ӧ��
	double addfit[SIZE] = { 0.0 };

	// ������� p	����Ӧ�� sum_fitness
	double p, sum_fitness = 0.0;

	for (int i = 0;i < SIZE;i++)
	{
		sum_fitness += s0[i].fitness;
	}

	// �������������
	for (int i = 0;i < SIZE;i++)
	{
		addfit[i] = s0[i].fitness / sum_fitness;
	}
	// �����������������ǰ׺��
	for (int i = 1;i < SIZE;i++)
	{
		addfit[i] = addfit[i - 1] + addfit[i];
	}

	// �����㷨
	for (int i = 0;i < SIZE;i++)
	{
		p = rand() % 1000 / 1000.0;
		int index = 0;
		while (p >= addfit[index])
		{
			index++;
		}
		// ѡ��-���� s0 -> s1
		s1[i] = s0[index];
	}
}

// ����λ��
void cross_pos(int x, int y, int pos, int end, struct ga s1[SIZE])
{
	// ������Ⱦɫ���ֵ
	int temp_x, temp_y = 0;
	temp_x = s1[x].threshold;
	temp_y = s1[y].threshold;

	int arr_x[8] = { 0 };
	int arr_y[8] = { 0 };
	// �� x ת��Ϊ��������
	DecimalToBinary(temp_x, arr_x);

	// �� y ת��Ϊ��������
	DecimalToBinary(temp_y, arr_y);
	
	// ���� x �� y �� [pos,end] λ
	int temp = 0;
	for (int i = pos;i <= end;i++) {
		temp = arr_x[i];
		arr_x[i] = arr_y[i];
		arr_y[i] = temp;
	}

	temp_x = BinaryToDecimal(arr_x, 8);
	temp_y = BinaryToDecimal(arr_y, 8);

	s1[x].threshold = temp_x;
	s1[y].threshold = temp_y;
	
	return;
}

// ����
void cross(struct ga s1[SIZE], struct ga s2[SIZE])
{
	double p = 0.0;
	bool pi[SIZE] = { false };

	for (int i = 0;i < SIZE;i++)
	{
		p = rand() % 1000 / 1000.0;
		pi[i] = (p < Pc);
	}
	// �������
	list<int>crosslist;
	for (int i = 0;i < SIZE;i++)
	{
		if (pi[i])
		{
			crosslist.push_back(i);
		}
	}
	// �Ƿ�Ϊż��
	if (crosslist.size() % 2 != 0)
	{
		crosslist.pop_back();
	}

	// ��ʼ����
	while (!crosslist.empty())
	{
		int x = crosslist.front(); crosslist.pop_front();
		int y = crosslist.front(); crosslist.pop_front();

		int p1, p2 = 0;
		p1 = rand() % GeneticSize; // [0, 7]
		p2 = rand() % GeneticSize; // [0, 7]

		int temp = 0;
		if (p1 > p2)
		{
			temp = p2;
			p2 = p1;
			p1 = temp;
		}
		// cross ���淽ʽ ��[p1, p2]�м��λȫ������
		cross_pos(x, y, p1, p2, s1);
	}


	// s1 -> s2
	NextGeneric(s1, s2);

	return;
}

// ����
void Mutation(struct ga s2[SIZE], struct ga s3[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		int x = 0;
		x = s2[i].threshold;

		// ��������
		int arr_x[8] = { 0 };

		// �� x �Ķ�������
		DecimalToBinary(x, arr_x);

		// �������
		double p = 0.0;
		for (int i = 0;i < GeneticSize;i++)
		{
			p = rand() % 1000 / 1000.0;
			if (p < Pm)
			{
				// ��ʼ����
				if (arr_x[i] == 0)
				{
					arr_x[i] = 1;
				}
				else
				{
					arr_x[i] = 0;
				}
			}
		}

		// arr_x ������ת��Ϊʮ����
		int sum = 0;
		sum = BinaryToDecimal(arr_x, 8);

		// �����
		s2[i].threshold = sum;
	}

	// s2 -> s3
	NextGeneric(s2, s3);
}

// ��ʮ����ת��Ϊ������
void DecimalToBinary(const int n, int a[8])
{
	int num = 0;
	num = n;
	for (int i = 0;i < 8;i++) {
		a[i] = num % 2;
		num /= 2;
	}
}

// ��������ת��Ϊʮ����
int BinaryToDecimal(const int arr[], int bit)
{
	int sum = 0;
	for (int i = 0;i < bit;i++)
	{
		if (arr[i] == 1)
		{
			sum += (int)pow(2, i);
		}
	}

	return (sum);
}

// һ��Ⱥ�己����һ��Ⱥ��
void NextGeneric(struct ga FA[SIZE], struct ga SON[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		SON[i] = FA[i];
	}
}

// ���Ŵ��㷨
int sga(const Mat src)
{
	int BestThreshold = 0;
	srand((unsigned)time(NULL));

	// ��ʼ��
	int t = 1;
	struct ga s0[SIZE] = { 0, 0.0 };
	for (int i = 0;i < SIZE;i++)
	{
		s0[i].threshold = rand() % 256;
	}

	// ������Ӧ��
	CalculateFitnessValue(src, s0);

	struct ga s1[SIZE] = { 0, 0.0 };
	struct ga s2[SIZE] = { 0, 0.0 };
	struct ga s3[SIZE] = { 0, 0.0 };
	
	while (1) 
	{
		// ������Ӧ��
		CalculateFitnessValue(src, s0);

		// ��Ѹ���
		BestThreshold = BestFitness(s0);

		// ѡ��-���� s0 -> s1
		choose_copy(s0, s1);

		// ���� s1 -> s2
		cross(s1, s2);

		// ���� s2 -> s3
		Mutation(s2, s3);

		// s3 -> s0
		NextGeneric(s3, s0);

		t++;
		if (t > T) {
			break;
		}
	}
	return (BestThreshold);
}