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

	// 二值化
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

	/*
	 * u: 整幅图像的总平均灰度
	 * maxVariance: 最大类间方差
	 */
	int i, j;
	double u = 0;
	double maxVariance = 0;

	// 目标阈值
	int threshold = 0;

	// 依次遍历每个像素
	for (i = 1; i < 256; i++)
	{
		// 初始化
		sum0 = 0;  sum1 = 0;
		cnt0 = 0;  cnt1 = 0;
		u0 = 0;  u1 = 0;
 		w0 = 0;  w1 = 0;

		// 前景
		for (j = 0; j < i; j++)
		{
			cnt0 += histogram[j];
			sum0 += (long)(j * histogram[j]);
		}

		// u0:前景平均灰度  w0:前景像素点数量占全部像素点的比例
		u0 = (double)(sum0 * 1.0 / cnt0);
		w0 = (double)(cnt0 * 1.0  / size);


		// 背景
		for (j = i; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += (long)(j * histogram[j]);
		}

		// u1:背景平均灰度  w1:背景像素点数占全部像素点的比例
		u1 = (double)(sum1 * 1.0 / cnt1);
		w1 = 1 - w0;

		// u:图像平均灰度   variancn:类间方差
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