#include "getlen.h"



ULPts::ULPts(const vector<vecP2d> &pts){
	Size = pts.size();
	Pts = new vecP2d[Size];
	Lines = new double[Size][3];
	LineState = new int[Size];
	for (int i = 0; i < Size; i++){
		Pts[i] = pts[i];
		LineState[i] = ULPTS_LINE_NOFIT;
		Lines[i][0] = 0;
		Lines[i][1] = 0;
		Lines[i][2] = 0;
	}
}



ULPts::~ULPts(){
	delete[] Pts;
	delete[] Lines;
	delete[] LineState;
}



int ULPts::Fitting(int theLine, double RmProp, double step){
	if (theLine >= Size){
		cout << "in ULPts::Fitting: \n"
			<< "please input correct index" << endl;
		return 0;
	}

	if (!FitLine(Pts[theLine], Lines[theLine][0], Lines[theLine][1], Lines[theLine][2], RmProp, step)){
		cout << "in ULPts::Fitting: \n" 
			<< "the " << theLine << "th Pts fit error" << endl;
		return 0;
	}

	LineState[theLine] = ULPTS_LINE_FIT;

	return 1;
}



int ULPts::TransPts(const Mat &CameraM, const Mat &Tvec, const Mat &Rmat, const Mat &DisCoeff, int thePts){
	if (thePts >= Size){
		cout << "in ULPts::TransPts: \n"
			<< "please input correct index" << endl;
		return 0;
	}

    undistortPoints(Pts[thePts], Pts[thePts], CameraM, DisCoeff, cv::noArray(), CameraM);

	//准备参数
	double alpha = CameraM.at<double>(0, 2), beta = CameraM.at<double>(1, 2);
	double fx = CameraM.at<double>(0, 0), fy = CameraM.at<double>(1, 1);
	Mat trans_i(3, 3, CV_64F), trans(3, 3, CV_64F);

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			trans_i.at<double>(i, j) = Rmat.at<double>(i, j);

	trans_i.at<double>(2, 2) = 1.0;


	int num_p = Pts[thePts].size();
	for (int i = 0; i < num_p; i++){
		Point2d &p = Pts[thePts][i];
		trans_i.at<double>(0, 2) = (p.x - alpha) / fx;
		trans_i.at<double>(1, 2) = (p.y - beta) / fy;
		invert(trans_i, trans);
		trans = -trans * Tvec;
		p.x = trans.at<double>(0, 0);
		p.y = trans.at<double>(1, 0);
	}

	return 1;
}



int ULPts::GetDistPtL(double &dist, int up, int lw, double RmProp, int BaseLine){
	if (up >= Size){
		cout << "in ULPts::GetDistPtL: \n"
			<< "the uppts index is out of range" << endl;
		return 0;
	}

	if (lw >= Size){
		cout << "in ULPts::GetDistPtL: \n"
			<< "the lwpts index is out of range" << endl;
		return 0;
	}

	if (RmProp >= 0.3 || RmProp < 0){
		cout << "in ULPts::GetDistPtL: \n"
			<< "RmProp should not be more than or equal to 0.5 or be negetive" << endl;
		return 0;
	}
	
	int pth, lth;
	switch (BaseLine){
	case ULPTS_LW: pth = up; lth = lw; break;
	case ULPTS_UP: pth = lw; lth = up; break;
	case ULPTS_UL: 
		double d1, d2;
		if (!GetDistPtL(d1, up, lw, RmProp, ULPTS_LW)){
			cout << "in ULPts::GetDistPtL: \n"
				<< "faile to get distance under ULPTS_UL model" << endl;
			return 0;
		}
		if (!GetDistPtL(d2, up, lw, RmProp, ULPTS_UP)){
			cout << "in ULPts::GetDistPtL: \n"
				<< "faile to get distance under ULPTS_UL model" << endl;
			return 0;
		}
		dist = (d1 + d2) / 2.0;
		return 1;
	default:
		cout << "in ULPts::GetDistPtL: \n"
			<< "please input correct BaseLine" << endl;
		return 0;
	}

	if (LineState[lth] == ULPTS_LINE_NOFIT){
		cout << "in ULPts::GetDistPtL: the line has not been fitted, so it will be fitted with default parameter: "
			<< "RmProp = " << 0.2 << "  step = " << 1e-3 << endl;
		if (!Fitting(lth)){
			cout << "in ULPts::GetDistPtL: \n"
				<< "fail to fit " << lth << "th pts" << endl;
			return 0;
		}
	}

	const int num_up = Pts[pth].size();
	if (num_up < 1){
		cout << "in ULPts::GetDistPtL: \n"
			<< "the number of the points should not be 0" << endl;
		return 0;
	}

	vector<double> vecd;
	vecd.push_back(PtoLine(Pts[pth][0], Lines[lth][0], Lines[lth][1], Lines[lth][2]));
	for (int i = 1; i < num_up; i++){
		double d = PtoLine(Pts[pth][i], Lines[lth][0], Lines[lth][1], Lines[lth][2]);
		int j = 0;
		for (; j < i; j++){
			if (d < vecd[j])
				break;
		}
		vecd.insert(vecd.begin() + j, d);
	}

	dist = 0;
	for (int i = RmProp * num_up; i < num_up - (int)(RmProp * num_up); i++){
		dist += vecd[i];
	}
	dist /= num_up - 2 * (int)(RmProp * num_up);

	return 1;
}



int ULPts::GetCurrentPts(vecP2d &pts, int thePts){
	if (thePts >= Size){
		cout << "in ULPts::GetCurrentPts: \n"
			<< "please input correct index" << endl;
		return 0;
	}

	pts = Pts[thePts];
	return 1;
}



int ULPts::GetLines(int theLine, double &A, double &B, double &C){
	if (theLine >= Size){
		cout << "in ULPts::GetLines: \n"
			<< "please input correct index" << endl;
		return 0;
	}

	if (LineState[theLine] == ULPTS_LINE_NOFIT){
		cout << "in ULPts::GetLines: the line has not been fitted, so it will be fitted with default parameter: "
			<< "RmProp = " << 0.2 << "  step = " << 1e-3 << endl;
		if (!Fitting(theLine)){
			cout << "in ULPts::GetLines: \n"
				<< "fail to fit " << theLine << "th pts" << endl;
			return 0;
		}
	}

	A = Lines[theLine][0]; B = Lines[theLine][1]; C = Lines[theLine][2];
	return 1;
}
