#include "solution.h"
#include "param.h"

void Proc::DilatEro(Mat &Mask, int D_E, int times, Point p){
    Mat img_temp;
    img.copyTo(img_temp);

    if (D_E == 0){
        dilate(img_temp, img_temp, Mask, p, times);
        img_temp.copyTo(img);
        return;
    }

    if (D_E == 1){
        erode(img_temp, img_temp, Mask, p, times);
        img_temp.copyTo(img);
        return;
    }

    cout << "in Proc::DilatEro\n" << "input correct mod: 0 or 1" << endl;
}

void Proc::PartDilatEro(const Rect &r, Mat &Mask, int D_E, int times, Point p){
    Mat img_temp;
    img(r).copyTo(img_temp);

    if (D_E == 0){
        dilate(img_temp, img_temp, Mask, p, times);
        img_temp.copyTo(img(r));
        return;
    }

    if (D_E == 1){
        erode(img_temp, img_temp, Mask, p, times);
        img_temp.copyTo(img(r));
        return;
    }

    cout << "in Proc::DilatEro\n" << "input correct mod: 0 or 1" << endl;
}


void Proc::DublBinary(double thresh1, double thresh2, int Mod){
    double threshUp, threshLow;
    if (thresh1 > thresh2){
        threshUp = thresh1;
        threshLow = thresh2;
    }
    else{
        threshUp = thresh2;
        threshLow = thresh1;
    }

    Mat img_low = img.clone();
    threshold(img_low, img_low, threshLow, 255, CV_THRESH_BINARY_INV);
    threshold(img, img, threshUp, 255, CV_THRESH_BINARY);
    img_low = img + img_low;

    if (Mod == PROC_DBINARY_IN){
        img_low = 255 - img_low;
        img_low.copyTo(img);
        return;
    }

    if (Mod == PROC_DBINARY_OUT){
        img_low.copyTo(img);
        return;
    }

    cout << "in Proc::DblBinary\n" << "input correct mod: 0 or 1" << endl;
}



void Proc::GetEdge_Bin(int width, int times){

    Mat Mask = Mat::ones(width, width, CV_8UC1);
    Mat img_n = 255 - img;
    Mat img_t = img.clone();
    erode(img_t, img_t, Mask, Point(-1, -1), times);
    erode(img_n, img_n, Mask, Point(-1, -1), times);

    img_n = img_n + img_t;
    img_n.copyTo(img);
}



int Proc::CountImg(){
    vector<vector<Point>> contours;
    findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    Mat marker = Mat::zeros(img.size(), CV_32SC1);

    for (int i = 0; i < contours.size(); i++) {
        drawContours(marker, contours, static_cast<int>(i), Scalar(static_cast<int>(i)+1), -1);
    }

    img.convertTo(img, CV_32SC1);
    marker.copyTo(img);
    return contours.size();
}



void Proc::CountFilt(double thresh){
    vector<vector<Point>> contours, contours_f;
    findContours(img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > thresh){
            contours_f.push_back(contours[i]);
        }
    }

    Mat marker = Mat::zeros(img.size(), CV_8UC1);

    for (int i = 0; i < contours_f.size(); i++) {
        drawContours(marker, contours_f, static_cast<int>(i), Scalar(255), -1);
    }

    img.convertTo(img, CV_8UC1);
    marker.copyTo(img);
}










void WaterShed(Solu& S){
    //???????????
    Mat marker;
    S.GetRsutImg(marker);

    //??????
    Mat img;
    S.GetRoi(img, SOL_RAW);
    S.ImgWrite(img);
    //????img??img_t??§Þ???????????????loadImgt
    S.GaussFilt(Size(5, 5), 6, 4, 2);
    S.GetRsutImg(img);

    //?????
    watershed(img, marker);
    S.ImgWrite(marker);
}



//void S_Run(Solu &S){
//	//????????
//	int cols, rows;
//	S.GetImgSize(cols, rows);
//
//
//	Rect r(0, 0, cols, rows * 1 / 3);
//	/*S.PartHistEq(r);
//	r = Rect(0, rows - rows / 3, cols, rows / 3);
//	S.PartHistEq(r);*/
//
//	//?????
//	S.DublBinary(40, 230, PROC_DBINARY_OUT);
//
//	////???§³?????????
//	//S.CountFilt(6000);
//
//	////??????
//	//Mat Mask = Mat::ones(1, 30, CV_8UC1);
//	//S.DilatEro(Mask, PROC_EROD, 1);
//	//S.CountFilt(6000);
//	//S.DilatEro(Mask, PROC_DILATE, 1);
//
//	////????§Þ?????
//	//int cols, rows;
//	//S.GetImgSize(cols, rows);
//	//Rect r(0, rows * 3 / 16, cols, rows - rows * 3 / 8);
//	//Mask = Mat::ones(3, 201, CV_8UC1);
//	//S.PartDilatEro(r, Mask, PROC_DILATE, 30);
//	//S.PartDilatEro(r, Mask, PROC_EROD, 30);
//
//	//????????
//	//????????????
//	Mat Mask = Mat::ones(3, 51, CV_8UC1);
//	S.DilatEro(Mask, PROC_DILATE, 1);
//	//??????¡À?????????
//	Mask = Mat::ones(3, 151, CV_8UC1);
//	S.DilatEro(Mask, PROC_EROD, 4);
//	S.DilatEro(Mask, PROC_DILATE, 4);
//	//?????????????
//	r = Rect(0, rows / 3, cols, rows - rows * 2 / 3);
//	Mask = Mat::ones(30, 1, CV_8UC1);
//	S.PartDilatEro(r, Mask, PROC_DILATE, 10);
//
//	r = Rect(0, rows / 8, cols, rows - rows / 4);
//	Mask = Mat::ones(3, 501, CV_8UC1);
//	S.PartDilatEro(r, Mask, PROC_DILATE, 5);
//	//S.PartDilatEro(r, Mask, PROC_EROD, 7);
//	Mask = Mat::ones(21, 1, CV_8UC1);
//	S.PartDilatEro(r, Mask, PROC_EROD, 1);
//
//	Mask = Mat::ones(1, 401, CV_8UC1);
//	r = Rect(0, 0, cols, rows / 15);
//	S.PartDilatEro(r, Mask, PROC_DILATE, 2);
//	S.PartDilatEro(r, Mask, PROC_EROD, 2);
//	r = Rect(0, rows - rows / 15, cols, rows / 15);
//	S.PartDilatEro(r, Mask, PROC_DILATE, 2);
//	S.PartDilatEro(r, Mask, PROC_EROD, 2);
//
//
//
//
////#ifdef I34
////	S.ResetImg();
////
////	Rect r(0, 0, cols, rows * 0.095);
////	S.SetImg(r, 255);
////	r = Rect(0, rows * 0.206, cols, rows * 0.584);
////	S.SetImg(r, 255);
////	r = Rect(0, rows * 0.89, cols, rows - int(rows * 0.89));
////	S.SetImg(r, 255);
////#endif
////#ifdef I27
////	S.ResetImg();
////
////	Rect r(0, 0, cols, rows * 0.105);
////	S.SetImg(r, 255);
////	r = Rect(0, rows * 0.206, cols, rows * 0.58);
////	S.SetImg(r, 255);
////	r = Rect(0, rows * 0.9, cols, rows - int(rows * 0.9));
////	S.SetImg(r, 255);
////#endif
//
//	//?????????
//	S.GetEdge_Bin(3, 8);//???????8
//
//	//???????????
//	S.CountImg();
//
//	//?????
//	WaterShed(S);
//}

void S_Run(Solu &S){
    //????????
    int cols, rows;
    S.GetImgSize(cols, rows);

    //?????
    S.DublBinary(40, 230, PROC_DBINARY_OUT);

    //????????????
    Mat Mask = Mat::ones(1, 81, CV_8UC1);
    S.DilatEro(Mask, PROC_DILATE, 1);
    //??????¡À?????????
    Rect r = Rect(0, rows / 10, cols, rows * 2 / 9 - rows / 10);
    Mask = Mat::ones(1, 901, CV_8UC1);
    S.PartDilatEro(r, Mask, PROC_EROD, 1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 1);
    r = Rect(0, rows * 7 / 9, cols, rows * 2 / 9 - rows / 10);
    S.PartDilatEro(r, Mask, PROC_EROD, 1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 1);


    r = Rect(0, rows / 6, cols, rows / 6);
    Mask = Mat::ones(5, 1, CV_8UC1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 5);
    S.PartDilatEro(r, Mask, PROC_EROD, 5);
    Mask = Mat::ones(1, 2001, CV_8UC1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 1);

    r = Rect(0, rows * 2 / 3, cols, rows / 6);
    Mask = Mat::ones(5, 1, CV_8UC1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 5);
    S.PartDilatEro(r, Mask, PROC_EROD, 5);
    Mask = Mat::ones(1, 2001, CV_8UC1);
    S.PartDilatEro(r, Mask, PROC_DILATE, 1);

    r = Rect(0, rows / 3 - rows / 9, cols, rows / 3 + rows * 2 / 9);
    S.SetImg(r, 255);

    Mask = Mat::ones(1, 3, CV_8UC1);
    r = Rect(0, 0, cols, rows / 13);
    S.SetImg(r, 255);
    r = Rect(0, rows - rows / 13, cols, rows / 13);
    S.SetImg(r, 255);

    //?????????
    S.GetEdge_Bin(3, 8);//???????8

    //???????????
    S.CountImg();

    //?????
    WaterShed(S);
}


void GetWaterEdge(const Mat &src,const Rect &r, Mat& edge){
    Solu S(src);
    S.SetRoi(r);
    S_Run(S);
    S.GetRsutImg(edge);
}


