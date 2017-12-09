/*
 * 基于遗传算法，实现图像前景和背景风分割
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

#define SIZE 64			// 种群规模
#define GeneticSize 8	// 基因大小
#define Pc   0.95		// 交叉率
#define Pm   0.05		// 变异率
#define T    10000		// 种群代数

// 染色体信息
struct ga {
	int threshold;  // 阈值
	double fitness; // 适应度
};

// 适应度函数
double otsu(const Mat src, int threshold); 
// 计算适应度
void CalculateFitnessValue(const Mat src, struct ga  s[SIZE]);
// 最佳适应度
int BestFitness(struct ga s0[SIZE]);
// 选择-复制
void choose_copy(struct ga s0[SIZE], struct ga s1[SIZE]);
// 交叉位置
void cross_pos(int x, int y, int pos, int end, struct ga s1[SIZE]);
// 交叉
void cross(struct ga s1[SIZE], struct ga s2[SIZE]);
// 变异
void Mutation(struct ga s2[SIZE], struct ga s3[SIZE]);
// 将十进制转换为二进制
void DecimalToBinary(const int n, int a[8]);
// 将二进制转换为十进制
int BinaryToDecimal(const int arr[], int bit);
// 一个群体繁衍下一个群体
void NextGeneric(struct ga FA[SIZE], struct ga SON[SIZE]);
// 简单遗传算法
int sga(const Mat src);


// 灰度分割
void single_channel(const Mat src, Mat &Dst);
// 三通道分割
void three_channel(const Mat src, Mat &Dst);


int main(int argc, uchar* argv[])
{
	string image_name;
	cin >> image_name;
	Mat SrcImg = imread(image_name);
	Mat Dst;

	// 单通道
	if (SrcImg.channels() == 1) {
		single_channel(SrcImg, Dst);
	}
	// 三通道
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

// 灰度分割
void single_channel(const Mat src, Mat & Dst)
{
	Mat src_gray;

	cvtColor(src, src_gray, CV_RGB2GRAY);

	int thresh = 0;

	if (!src_gray.empty())
	{
		thresh = sga(src_gray);
		cout << "最优解: " << thresh << endl;
	}
	// 分割
	threshold(src_gray, Dst, thresh, 255, CV_THRESH_BINARY);
}

// 三通道分割
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

	// 原始
	imwrite("C:\\Users\\sunyiming\\Desktop\\R.tiff", mv_back[0]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\G.tiff", mv_back[1]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\B.tiff", mv_back[2]);

	// 二值化后
	imwrite("C:\\Users\\sunyiming\\Desktop\\R1.tiff", mv[0]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\G1.tiff", mv[1]);
	imwrite("C:\\Users\\sunyiming\\Desktop\\B1.tiff", mv[2]);

	merge(mv, Dst);

	mv.clear();
	mv_back.clear();
}

// 适应度函数
double otsu(const Mat src, int threshold)
{
	int height = src.rows;
	int width  = src.cols;
	int size   = height * width;
	unsigned char* data = src.data;


	// histogram    
	long histogram[256] = { 0 };

	// histogram 保存每个特定像素值大小的个数
	for (int i = 0; i < height; i++)
	{
		unsigned char* p = data + i * src.step;
		for (int j = 0; j < width; j++)
		{
			histogram[int(*p++)]++;
		}
	}

	/*
	* sum0:前景的灰度总和     sum1:背景灰度总和
	* cnt0:前景像素的总个数   cnt1:背景像素的总个数
	* w0:  前景像素个数占整幅图像像素的比例
	* w1:  背景像素个数占整幅图像像素的比例
	* u0:  前景的平均灰度     u1: 背景的平均灰度
	* variance: 类间方差
	*/
	long sum0 = 0, sum1 = 0;
	long cnt0 = 0, cnt1 = 0;
	double w0 = 0, w1 = 0;
	double u0 = 0, u1 = 0;
	double variance = 0;

	// 初始化
	sum0 = 0;  sum1 = 0;
	cnt0 = 0;  cnt1 = 0;
	u0 = 0;  u1 = 0;
	w0 = 0;  w1 = 0;

	// 前景
	for (int j = 0; j < threshold; j++)
	{
		cnt0 += histogram[j];
		sum0 += (long)(j * histogram[j]);
	}

	// u0:前景平均灰度  w0:前景像素点数量占全部像素点的比例
	u0 = (double)(sum0 * 1.0 / cnt0);
	w0 = (double)(cnt0 * 1.0 / size);


	// 背景
	for (int j = threshold; j <= 255; j++)
	{
		cnt1 += histogram[j];
		sum1 += (long)(j * histogram[j]);
	}

	// u1:背景平均灰度  w1:背景像素点数占全部像素点的比例
	u1 = (double)(sum1 * 1.0 / cnt1);
	w1 = 1 - w0;

	// variancn:类间方差
	variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
	return (variance);
}

// 计算适应度
void CalculateFitnessValue(const Mat src, struct ga  s[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		s[i].fitness = otsu(src, s[i].threshold);
	}
}

// 最佳适应度
int BestFitness(struct ga s0[SIZE])
{
	// fitness_val: 目标阈值 MaxFitness: 最大适应度
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

// 选择-复制
void choose_copy(struct ga s0[SIZE], struct ga s1[SIZE])
{
	// 前缀和适应度
	double addfit[SIZE] = { 0.0 };

	// 随机概率 p	总适应度 sum_fitness
	double p, sum_fitness = 0.0;

	for (int i = 0;i < SIZE;i++)
	{
		sum_fitness += s0[i].fitness;
	}

	// 求轮盘区域概率
	for (int i = 0;i < SIZE;i++)
	{
		addfit[i] = s0[i].fitness / sum_fitness;
	}
	// 求轮盘区域概率数组前缀和
	for (int i = 1;i < SIZE;i++)
	{
		addfit[i] = addfit[i - 1] + addfit[i];
	}

	// 赌轮算法
	for (int i = 0;i < SIZE;i++)
	{
		p = rand() % 1000 / 1000.0;
		int index = 0;
		while (p >= addfit[index])
		{
			index++;
		}
		// 选择-复制 s0 -> s1
		s1[i] = s0[index];
	}
}

// 交叉位置
void cross_pos(int x, int y, int pos, int end, struct ga s1[SIZE])
{
	// 待交叉染色体的值
	int temp_x, temp_y = 0;
	temp_x = s1[x].threshold;
	temp_y = s1[y].threshold;

	int arr_x[8] = { 0 };
	int arr_y[8] = { 0 };
	// 将 x 转换为二进制数
	DecimalToBinary(temp_x, arr_x);

	// 将 y 转换为二进制数
	DecimalToBinary(temp_y, arr_y);
	
	// 交叉 x 和 y 的 [pos,end] 位
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

// 交叉
void cross(struct ga s1[SIZE], struct ga s2[SIZE])
{
	double p = 0.0;
	bool pi[SIZE] = { false };

	for (int i = 0;i < SIZE;i++)
	{
		p = rand() % 1000 / 1000.0;
		pi[i] = (p < Pc);
	}
	// 交配队列
	list<int>crosslist;
	for (int i = 0;i < SIZE;i++)
	{
		if (pi[i])
		{
			crosslist.push_back(i);
		}
	}
	// 是否为偶数
	if (crosslist.size() % 2 != 0)
	{
		crosslist.pop_back();
	}

	// 开始交叉
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
		// cross 交叉方式 将[p1, p2]中间的位全部交叉
		cross_pos(x, y, p1, p2, s1);
	}


	// s1 -> s2
	NextGeneric(s1, s2);

	return;
}

// 变异
void Mutation(struct ga s2[SIZE], struct ga s3[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		int x = 0;
		x = s2[i].threshold;

		// 二进制数
		int arr_x[8] = { 0 };

		// 求 x 的二进制数
		DecimalToBinary(x, arr_x);

		// 随机概率
		double p = 0.0;
		for (int i = 0;i < GeneticSize;i++)
		{
			p = rand() % 1000 / 1000.0;
			if (p < Pm)
			{
				// 开始变异
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

		// arr_x 二进制转换为十进制
		int sum = 0;
		sum = BinaryToDecimal(arr_x, 8);

		// 变异后
		s2[i].threshold = sum;
	}

	// s2 -> s3
	NextGeneric(s2, s3);
}

// 将十进制转换为二进制
void DecimalToBinary(const int n, int a[8])
{
	int num = 0;
	num = n;
	for (int i = 0;i < 8;i++) {
		a[i] = num % 2;
		num /= 2;
	}
}

// 将二进制转换为十进制
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

// 一个群体繁衍下一个群体
void NextGeneric(struct ga FA[SIZE], struct ga SON[SIZE])
{
	for (int i = 0;i < SIZE;i++)
	{
		SON[i] = FA[i];
	}
}

// 简单遗传算法
int sga(const Mat src)
{
	int BestThreshold = 0;
	srand((unsigned)time(NULL));

	// 初始代
	int t = 1;
	struct ga s0[SIZE] = { 0, 0.0 };
	for (int i = 0;i < SIZE;i++)
	{
		s0[i].threshold = rand() % 256;
	}

	// 计算适应度
	CalculateFitnessValue(src, s0);

	struct ga s1[SIZE] = { 0, 0.0 };
	struct ga s2[SIZE] = { 0, 0.0 };
	struct ga s3[SIZE] = { 0, 0.0 };
	
	while (1) 
	{
		// 计算适应度
		CalculateFitnessValue(src, s0);

		// 最佳个体
		BestThreshold = BestFitness(s0);

		// 选择-复制 s0 -> s1
		choose_copy(s0, s1);

		// 交叉 s1 -> s2
		cross(s1, s2);

		// 变异 s2 -> s3
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