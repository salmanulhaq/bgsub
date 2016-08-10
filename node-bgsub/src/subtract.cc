#include "subtract.h"
#include <chrono>

using namespace std;
using namespace cv;

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

BGSubError subtract(string path, string outputPath, double& latency) {

	try{
		auto t0 = chrono::high_resolution_clock::now();
		
		Mat input = imread(path, -1);
		Mat mask, fg, bg;
		mask = Scalar(0);

		if(input.empty()) {
			latency = 0.0;
			return OPENCV_ERROR;
		}

		if(input.channels() == 4) {
			cvtColor(input, input, CV_BGRA2BGR);
		}

		grabCut(input, mask, Rect(10,10,input.cols-10,input.rows-10), bg, fg, 0, GC_INIT_WITH_RECT);
		grabCut(input, mask, Rect(), bg, fg, 4, GC_EVAL);
		compare(mask, GC_PR_FGD, mask, CMP_EQ);

		int dilation_size = 1;
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2*dilation_size+1, 2*dilation_size+1), Point(dilation_size, dilation_size));
	  	dilate(mask, mask, element);

	  	Mat output;
		input.copyTo(output, mask);

		cvtColor(output, output, CV_BGR2BGRA);

		vector<Mat> channels;
		split(output, channels);
		channels[3] = mask;
		merge(channels, output);

		auto t1 = chrono::high_resolution_clock::now();

		imwrite(outputPath, output);

		latency = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
		
		return SUCCESS;

	} catch(cv::Exception& ex) {
		latency = 0.0;
		return OPENCV_ERROR;
	}
}