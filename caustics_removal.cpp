#include "filtermatches.h"
#include "RobustMatcher.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <time.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/ximgproc/disparity_filter.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/core/utility.hpp>
using namespace cv;
using namespace std;
using namespace cv::ximgproc;

int main(int argc, char** argv) {

	// Read epipolar images from directory
	cv::String imagesPath = "D:/Newfolder/epipolar"; // it has filters, too !
	vector<cv::String> fn;
	glob(imagesPath, fn, true); // recursive, if you want
	int countIter = 0;

	// Read disparities from directory
	cv::String disparitiesPath = "D:/Newfolder/disparities"; // it has filters, too !
	vector<cv::String> fn1;
	glob(disparitiesPath, fn1, true); // recursive, if you want
	int countIter1 = 0;

	// Read masks from directory
	cv::String masksPath = "D:/Newfolder/masks"; // it has filters, too !
	vector<cv::String> fn2;
	glob(masksPath, fn2, true); // recursive, if you want
	int countIter2 = 0;

	for (int i = 0; i < fn.size(); i++)
	{
		countIter += 1;
		std::cout << "iteration " << countIter << std::endl;
		std::cout << "images " << i << " and " << i + 1 << std::endl;
		std::cout << "first pair is " << fn[i] << " and " << fn[i + 1] << std::endl;

		countIter1 += 1;
		std::cout << "iteration " << countIter1 << std::endl;
		std::cout << "disparities " << i << " and " << i + 1 << std::endl;
		std::cout << "first pair is " << fn1[i] << " and " << fn1[i + 1] << std::endl;

		countIter2 += 1;
		std::cout << "iteration " << countIter2 << std::endl;
		std::cout << "masks " << i << " and " << i + 1 << std::endl;
		std::cout << "first pair is " << fn2[i] << " and " << fn2[i + 1] << std::endl;


		cv::Mat inputImage1 = imread(fn[i]);
		if (!inputImage1.data) {
			std::cout << "Input image 1 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		cv::Mat inputImage2 = imread(fn[i + 1]);
		if (!inputImage2.data) {
			std::cout << "Input image 2 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		cv::Mat disparity1 = imread(fn1[i]);
		if (!disparity1.data) {
			std::cout << "Disp 1 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		cv::Mat disparity2 = imread(fn1[i + 1]);
		if (!disparity2.data) {
			std::cout << "Disp 2 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		cv::Mat mask1 = imread(fn2[i]);
		if (!mask1.data) {
			std::cout << "maskInv 1 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		cv::Mat mask2 = imread(fn2[i + 1]);
		if (!mask2.data) {
			std::cout << "maskInv 2 not found" << std::endl;
			std::cout << "Press enter to exit" << std::endl;
			std::cin.get();
			exit(-2);
		}

		Mat source = inputImage1.clone();
		Mat target = inputImage2.clone();

		// increase i for correct selection of image pairs
		i = i + 1;

		Mat masked1, masked2;

		inputImage1.copyTo(masked1, mask1);
		inputImage2.copyTo(masked2, mask2);

		imwrite("D:/Newfolder/masked1.png", masked1);
		imwrite("D:/Newfolder/masked2.png", masked2);
		std::cout << "Press enter to exit" << std::endl;
		cv::waitKey(0);

		float disparityscale = 4.001;


		// Color correction of images
		for (int i = 0; i < mask1.rows; i++)
		{
			for (int j = 0; j < mask1.cols; j++)
			{
				// Color correction of image 1 from image 2
				if (mask1.at<cv::Vec3b>(i, j)[0] != 0)
				{
					int x2 = j;
						//(j - (disparityscale * disparity1.at<cv::Vec3b>(i, j)[0]));
					if (x2 > 0 && x2 < inputImage2.cols && 
						inputImage2.at<cv::Vec3b>(i, x2)[0] > 5 &&
						inputImage2.at<cv::Vec3b>(i, x2)[1] > 5 &&
						inputImage2.at<cv::Vec3b>(i, x2)[2] > 5)
					{
						for (int k = 0; k < inputImage1.channels(); k++)
						{
							inputImage1.at<cv::Vec3b>(i, j)[k] = inputImage2.at<cv::Vec3b>(i, x2)[k];
						}
					}
				}
			}
		}
		//Mat inputImage11;
		//textureFlattening(inputImage1, mask1, inputImage11,50,150,3);

		imwrite("D:/Newfolder/correctedL.png", inputImage1);

	for (int i = 0; i < mask2.rows; i++)
	{
		for (int j = 0; j < mask2.cols; j++)
		{

			// Color correction of image 2 from image 1
			if (mask2.at<cv::Vec3b>(i, j)[0] != 0)
			{
				int x1 = j;
				//	(j + disparityscale * disparity2.at<cv::Vec3b>(i, j)[0]);
				if (x1>0 && x1<inputImage1.cols && 
					inputImage1.at<cv::Vec3b>(i, x1)[0] > 5 &&
					inputImage1.at<cv::Vec3b>(i, x1)[1] > 5 &&
					inputImage1.at<cv::Vec3b>(i, x1)[2] > 5)
				{
					for (int k = 0; k < inputImage2.channels(); k++)
					{
						inputImage2.at<cv::Vec3b>(i, j)[k] = inputImage1.at<cv::Vec3b>(i, x1)[k];
					}
				}
			}
		}
	}

	imwrite("D:/Newfolder/correctedR.png", inputImage2);

	}

	std::cout << "Press enter to exit" << std::endl;
	cv::waitKey(0);
}
