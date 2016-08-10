#include "internal.h"
#include <string>

using namespace std;
using namespace cv;

string getErrorString(enum BGSubError error) {
	switch(error) {
		case SUCCESS:
			return "SUCCESS";
			break;
		case ERROR:
			return "ERROR";
			break;
		case OPENCV_ERROR:
			return "OPENCV_ERROR";
			break;
		default:
			return "GENERAL ERROR";
			break;
	}
}