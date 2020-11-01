#ifndef __STANDARD_H
#define __STANDARD_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "param.h"

using namespace cv;
using namespace std;

void GetCmaraParam(Mat &CamaraM, Mat &Tvec, Mat &Rmat, Mat &DisCoeff);


#endif
