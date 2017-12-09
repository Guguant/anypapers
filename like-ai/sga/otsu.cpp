#include "opencv2/opencv.hpp"
#include <iostream>

#include <direct.h>


using namespace std;
using namespace cv;

int Otsu(const Mat src);
void ThresholdByOtsu(const Mat src,Mat & Dst);

//  the path of output image
static string g_kOutputPath;

int main(int argc, unsigned char* argv[])
{	
	string path = "image_result";

	// create a folder to save output images
	_mkdir(path.c_str());

	g_kOutputPath = path + "\\";

	// define the path of input image
	string input_image_path;
	cin >> input_image_path;

	// read the input image data, define the dst image
	Mat img = imread(input_image_path);
	Mat dst = Mat(img.rows, img.cols, CV_8UC1);

	imwrite((g_kOutputPath + "src.bmp"), img);
	// Threshold source image
	ThresholdByOtsu(img, dst);

	// if the inpout is not empty, imshow the dst image.
	if (!dst.empty())
	{
		imshow("dst", dst);
	}

	imwrite((g_kOutputPath + "dst.bmp"), dst);

	cout << "all output images are saved in " << g_kOutputPath << endl;
	waitKey(0);
	return 0;
}

// splite input image
void ThresholdByOtsu(const Mat src,  Mat & Dst)
{
	// split Mat src to Vector src_v
	vector<Mat>src_v;
	split(src, src_v);

	imshow("r_src", src_v[0]);
	imshow("g_src", src_v[1]);
	imshow("b_src", src_v[2]);

	// save origin image r, g, b
	imwrite((g_kOutputPath + "r_src.bmp"), src_v[0]);
	imwrite((g_kOutputPath + "g_src.bmp"), src_v[1]);
	imwrite((g_kOutputPath + "b_src.bmp"), src_v[2]);

	// get threshold
	int threshold_r, threshold_g, threshold_b = 0;
	threshold_r = Otsu(src_v[0]);
	threshold_g = Otsu(src_v[1]);
	threshold_b = Otsu(src_v[2]);

	// output r, g, b threshold after otsu.
	cout << "after otsu:" << endl;
	cout << "------------------" << endl;
	cout << "threshold_r:" << threshold_r << endl;
	cout << "threshold_g:" << threshold_g << endl;
	cout << "threshold_b:" << threshold_b << endl;

	// ��ֵ��
	vector<Mat>out_v;
	threshold(src_v[0], src_v[0], threshold_r, 255, CV_THRESH_BINARY);
	threshold(src_v[1], src_v[1], threshold_g, 255, CV_THRESH_BINARY);
	threshold(src_v[2], src_v[2], threshold_b, 255, CV_THRESH_BINARY);


	// save r, g, b image after otsu
	imwrite((g_kOutputPath + "r_otsu.bmp"), src_v[0]);
	imwrite((g_kOutputPath + "g_otsu.bmp"), src_v[1]);
	imwrite((g_kOutputPath + "b_otsu.bmp.bmp"), src_v[2]);

	// merge three chanels image with r_otsu, g_otsu, b_otsu
	merge(src_v, Dst);

	// clear
	src_v.clear();
}

// Otsu algorithm
int Otsu(const Mat src)
{
	int height = src.rows;
	int width  = src.cols;
	int size   = height * width;
	unsigned char* data = src.data;

	// histogram    
	long histogram[256] = { 0 };
	
	// pixel gray value count
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

	/*
	 * u: ����ͼ�����ƽ���Ҷ�
	 * maxVariance: �����䷽��
	 */
	int i, j;
	double u = 0;
	double maxVariance = 0;

	// Ŀ����ֵ
	int threshold = 0;

	// ���α���ÿ������
	for (i = 1; i < 256; i++)
	{
		// ��ʼ��
		sum0 = 0;  sum1 = 0;
		cnt0 = 0;  cnt1 = 0;
		u0 = 0;  u1 = 0;
 		w0 = 0;  w1 = 0;

		// ǰ��
		for (j = 0; j < i; j++)
		{
			cnt0 += histogram[j];
			sum0 += (long)(j * histogram[j]);
		}

		// u0:ǰ��ƽ���Ҷ�  w0:ǰ�����ص�����ռȫ�����ص�ı���
		u0 = (double)(sum0 * 1.0 / cnt0);
		w0 = (double)(cnt0 * 1.0  / size);


		// ����
		for (j = i; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += (long)(j * histogram[j]);
		}

		// u1:����ƽ���Ҷ�  w1:�������ص���ռȫ�����ص�ı���
		u1 = (double)(sum1 * 1.0 / cnt1);
		w1 = 1 - w0;

		// u:ͼ��ƽ���Ҷ�   variancn:��䷽��
		//u = u0 * w0 + u1 * w1;
		variance = w0 * w1 *  (u0 - u1) * (u0 - u1);

		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}

	return (threshold);
}