#include "standard.h"

void GetCmaraParam(Mat &CamaraM, Mat &Tvec, Mat &Rmat, Mat &DisCoeff){
	Mat(3, 3, CV_64F).copyTo(CamaraM);
	Mat(3, 1, CV_64F).copyTo(Tvec);
	Mat(3, 3, CV_64F).copyTo(Rmat);
	Mat(1, 5, CV_64F).copyTo(DisCoeff);

	for (int i = 0; i < 3; i++){
		Tvec.at<double>(i, 0) = tvecs[i];
		for (int j = 0; j < 3; j++){
			CamaraM.at<double>(i, j) = camatrix[i][j];
			Rmat.at<double>(i, j) = rvecs[i][j];
		}
	}
	
	for (int i = 0; i < 5; i++)
		DisCoeff.at<double>(0, i) = distCoeff[i];
}
