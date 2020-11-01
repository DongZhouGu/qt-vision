#ifndef __MEASURE_H
#define __MEASURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include "solution.h"
#include "getlen.h"
#include "mainwindow.h"
using namespace cv;
using namespace std;

int SamplePts(const Mat &mark, vector<vecP2d> &vpts, const double offsetx,
              const double offsety, const int num = 20);

void DispPts(Mat &fitpic, const Mat &img, const Rect &r,
             const vector<vecP2d> &vpts, const Mat &CamaraM,
             const Mat &DisCoeff, double RmProp_fit, double RmProp_uldist,
             double RmProp_mdist);

int DistofPts(const vector<vecP2d> &vpts, double dist[3], const Mat &CamaraM,
              const Mat &Tvec, const Mat &Rmat, const Mat &DisCoeff,
              double RmProp_fit = 0.2, double RmProp_uldist = 0.2,
              double RmProp_mdist = 0.2);

int GetPts(vector<vecP2d> &vpts, Mat &edge, const Mat &img, const Rect &r,
           const int num = 20);

int Measure(Mat &edge, Mat &fitpic, double dist[3], const Mat &img,
            const Rect &r, const Mat &CamaraM, const Mat &Tvec, const Mat &Rmat,
            const Mat &DisCoeff, int num = 20, double RmProp_fit = 0.2,
            double RmProp_uldist = 0.2, double RmProp_mdist = 0.2);
int MainMeasure(Mat &img,Mat &fitpic,double &mainwidth,double &sidewidth1,double &sidewidth2);
QImage MatToQImage(const Mat& mat);

#endif
