#ifndef __GETLEN_H
#define __GETLEN_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

typedef vector<Point2d> vecP2d;

//template <typename T>
//int fitLine(const vecP2d &vecP, T &A, T &B, T &C){
//	//拟合成功返回1，拟合失败返回0
//	double SigmaX = 0.0, SigmaX_2 = 0.0, SigmaXY = 0.0, SigmaY = 0.0;
//	const int num_p = vecP.size();
//
//	for (int i = 0; i < num_p; i++){
//		double y = vecP[i].y, x = vecP[i].x;
//		SigmaX += x;
//		SigmaX_2 += x * x;
//		SigmaXY += x * y;
//		SigmaY += y;
//	}
//	SigmaX /= num_p; SigmaX_2 /= num_p; SigmaXY /= num_p; SigmaY /= num_p;//防止数据过大
//
//	double Mod = SigmaX_2 - SigmaX * SigmaX;
//	if (Mod > 1e-4){
//		double k = SigmaXY - SigmaX * SigmaY;
//		double b = SigmaX_2 * SigmaY - SigmaX * SigmaXY;
//		k /= Mod;
//		b /= Mod;
//		A = k; B = -1.0; C = b;
//
//		return 1;
//	}
//
//
//	//该直线斜率较大时
//	SigmaX = 0.0, SigmaX_2 = 0.0, SigmaXY = 0.0, SigmaY = 0.0;
//	for (int i = 0; i < num_p; i++){
//		double x = vecP[i].y, y = vecP[i].x;
//		SigmaX += x;
//		SigmaX_2 += x * x;
//		SigmaXY += x * y;
//		SigmaY += y;
//	}
//	SigmaX /= num_p; SigmaX_2 /= num_p; SigmaXY /= num_p; SigmaY /= num_p;//防止数据过大
//
//	Mod = SigmaX_2 - SigmaX * SigmaX;
//	if (Mod < 1e-4){
//		cout << "in fitLine: faile to fit the line, please input other points" << endl;
//		A = 0; B = 0; C = 0;
//		return 0;
//	}
//	double k = SigmaXY - SigmaX * SigmaY;
//	double b = SigmaX_2 * SigmaY - SigmaX * SigmaXY;
//	k /= Mod;
//	b /= Mod;
//	A = 1.0; B = -k; C = -b;
//
//	return 1;
//}


template <typename T>
int FitLine(const vecP2d &vecP, T &A, T &B, T &C, double RmProp = 0, double step = 1e-4){
	if (RmProp >= 0.3 || RmProp < 0){
		A = 0; B = 0; C = 0;
		cout << "in fitLine: \n" 
			<< "RmProp should not be more than or equal to 0.5 or be negetive" << endl;
		return 0;
	}

	const int num_p = vecP.size();
	if (RmProp != 0){
		if (num_p < 5){
			A = 0; B = 0; C = 0;
			cout << "in fitLine: \n" 
				<< "the number of the points should not be less than 5" << endl;
			return 0;
		}
	}
	else{
		if (num_p < 2){
			A = 0; B = 0; C = 0;
			cout << "in fitLine: \n"
				<< "the number of the points should not be less than 2" << endl;
			return 0;
		}
	}

	if (step >= 0.1 || step < 0){
		A = 0; B = 0; C = 0;
		cout << "in fitLine: \n" 
			<< "RmProp should not be more than 0.1 or negetive" << endl;
		return 0;
	}

	double sin_sita, cos_sita, ro;
	double var = 0;
	double Gain = 3;
	for (double SinS = -Gain + step; SinS <= Gain; SinS += step){
		double CosS = Gain;
		double v = 0.0, m = 0.0;

		//计算本次方差（去掉n个最大和n个最小）
			//求出各个ro并且去掉干扰点的ro
		vector<double> dist;
		for (int i = 0; i < num_p; i++){
			double d = CosS * vecP[i].y - SinS * vecP[i].x;
			int j = 0;
			for (; j < i; j++){
				if (d < dist[j])
					break;
			}
			dist.insert(dist.begin() + j, d);
		}
		for (int i = 0; i < RmProp * num_p; i++)
			dist.pop_back();
		for (int i = 0; i < RmProp * num_p; i++)
			dist.erase(dist.begin());

		int num_dis = dist.size();
		for (int i = 0; i < num_dis; i++){
			m += dist[i];
			v += dist[i] * dist[i];
		}
		m /= num_dis;
		v /= num_dis;
		v -= m * m;

		//是否已经是最小方差
		if (SinS == -Gain + step){
			var = v;
			sin_sita = SinS;
			cos_sita = CosS;
			ro = m;
		}
		else{
			if (v < var){
				var = v;
				sin_sita = SinS;
				cos_sita = CosS;
				ro = m;
			}
		}
	}

	for (double CosS = -Gain; CosS < Gain; CosS += step){
		double SinS = Gain;
		double v = 0.0, m = 0.0;

		//计算本次方差（去掉n个最大和n个最小）
			//求出各个ro并且去掉干扰点的ro
		vector<double> dist;
		for (int i = 0; i < num_p; i++){
			double d = CosS * vecP[i].y - SinS * vecP[i].x;
			int j = 0;
			for (; j < i; j++){
				if (d < dist[j])
					break;
			}
			dist.insert(dist.begin() + j, d);
		}
		for (int i = 0; i < RmProp * num_p; i++)
			dist.pop_back();
		for (int i = 0; i < RmProp * num_p; i++)
			dist.erase(dist.begin());

		int num_dis = dist.size();
		for (int i = 0; i < num_dis; i++){
			m += dist[i];
			v += dist[i] * dist[i];
		}
		m /= num_dis;
		v /= num_dis;
		v -= m * m;

		//是否已经是最小方差
		if (v < var){
			var = v;
			sin_sita = SinS;
			cos_sita = CosS;
			ro = m;
		}
	}

	A = sin_sita; B = -cos_sita; C = ro;
	return 1;
}


inline double PtoLine(const Point2d p, double A, double B,double C){
	if (A == 0 && B == 0 && C == 0){
		cout << "in PtoLine: \n" 
			<< "it is not a line" << endl;
		return -1;
	}
	double dis = A * p.x + B * p.y + C;
	return fabs(dis) / sqrt(A * A + B * B);
}


//inline Point2d PprojLine(Point2d P, double A, double B, double C){
//	//要验证函数返回的点是否在直线上（误差不超过1e-3）
//	if (A == 0 && B == 0 && C == 0){
//		cout << "in PprojLine: \n" 
//			<< "it is not a line" << endl;
//		return Point2d(-1, -1);
//	}
//	double x = B * B * P.x - A * B * P.y - A * C;
//	double y = A * A * P.y - A * B * P.x - B * C;
//	double Mod = A * A + B * B;
//	x /= Mod;
//	y /= Mod;
//	return Point2d(x, y);
//}






#define ULPTS_LINE_NOFIT 0
#define ULPTS_LINE_FIT 1
#define ULPTS_UP 0
#define ULPTS_LW 1
#define ULPTS_UL 2

class ULPts{
	int Size;
	vecP2d *Pts;
	double (*Lines)[3];
	int *LineState;
	ULPts(const ULPts &ulp){}
	void operator= (const ULPts&){}
public:
	ULPts(const vector<vecP2d> &pts);
	int Fitting(int theLine, double RmProp = 0.2, double step = 1e-3);
	int TransPts(const Mat &CamaraM, const Mat &Tvec, const Mat &Rmat, const Mat &DisCoeff, int thePts);
	int GetDistPtL(double &dist, int up, int lw, double RmProp = 0.2, int BaseLine = ULPTS_LW);
	int GetCurrentPts(vecP2d &pts, int thePts);
	int GetLines(int theLine, double &A, double &B, double &C);
	int GetSize();
	~ULPts();
};

inline int ULPts::GetSize(){
	return Size;
}

#endif
