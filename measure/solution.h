#ifndef __SOLUTION_H
#define __SOLUTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc/types_c.h>
using namespace cv;
using namespace std;



#define PROC_DILATE 0
#define PROC_EROD 1
#define PROC_DBINARY_OUT 0
#define PROC_DBINARY_IN 1

class Proc{
    void operator= (const Proc&){};
protected:
    Mat img;
public:
    Proc(){};
    Proc(const Mat &src);
    Proc(const Proc &P);
    void ImgWrite(const Mat &src);//图像写入
    void HistEqu();//直方图均衡化
    void PartHistEq(const Rect &r);//局部直方图均衡化
    void Binary(double thresh, int Mod = THRESH_BINARY);//二值化
    void DublBinary(double thresh1, double thresh2, int Mod = PROC_DBINARY_OUT);//双阈值二值化
    void PartDilatEro(const Rect &r, Mat &Mask, int D_E = PROC_DILATE, int times = 1, Point p = Point(-1, -1));//部分腐蚀膨胀
    void DilatEro(Mat &Mask, int D_E = PROC_DILATE, int times = 1, Point p = Point(-1, -1));//腐蚀或膨胀
    void GetEdge_Bin(int width = 3, int times = 3);//获取二值图像粗边缘
    int CountImg();//计算连通域
    void CountFilt(double thresh);//滤除面积小的连通域,并标记
    void GaussFilt(Size s, double SigmaX, double SigmaY, int times = 1);//高斯滤波
    void GetRsutImg(Mat &dir);//获取img
    void GetImgSize(int &cols, int &rows);//获取图片尺寸
    void ResetImg();//将图片变为全0灰度
    void SetImg(const Rect &r, int gray);//将图片局部区域设置为gray灰度值
    ~Proc(){};
};



inline Proc::Proc(const Mat &src){
    img = src.clone();
}



inline Proc::Proc(const Proc &P){
    img = P.img.clone();
}



inline void Proc::ImgWrite(const Mat &src){
    src.copyTo(img);
}



inline void Proc::HistEqu(){
    equalizeHist(img, img);
}



inline void Proc::PartHistEq(const Rect &r){
    Mat img_p = img(r);
    equalizeHist(img_p, img_p);
}



inline void Proc::Binary(double thresh, int Mod){
    threshold(img, img, thresh, 255, Mod);
}



inline void Proc::GaussFilt(Size s, double SigmaX, double SigmaY, int times){
    for (int i = 0; i < times; i++)
        GaussianBlur(img, img, s, SigmaX, SigmaY);
}



inline void Proc::GetRsutImg(Mat &dir){
    img.copyTo(dir);
}



inline void Proc::GetImgSize(int &cols, int &rows){
    cols = img.cols;
    rows = img.rows;
}



inline void Proc::ResetImg(){
    Mat m = Mat::zeros(img.size(), CV_8UC1);
    m.copyTo(img);
}



inline void Proc::SetImg(const Rect &r, int gray){
    Mat m(r.size(), CV_8UC1, Scalar(gray));
    m.copyTo(img(r));
}








#define SOL_GRAY 0
#define SOL_RAW 1
#define SOL_TEMP 2

class Solu :public Proc{
    Mat img_raw;//原始图像
    Mat img_gray;//灰度图像
    Mat img_t;//可操作图像
    Rect ROI;
    Solu(const Solu& S) {};
    void operator= (const Solu&){};
public:
    Solu(const Mat& src);
    void LoadImgt(int sel = SOL_GRAY);//将原图或者灰度图加载到img_t，注意可能会影响img
    void SetRoi(const Rect& r);//img指向img_t区域
    void GetRoi(Mat& dir, int sel = SOL_GRAY);//获取ROI图像
    void CutRoi(const Rect &r);//img_t剪裁区域到img，img与img_t无关联
    void CutRla();//剪断img与img_t的关联，一般再不知道img与img_t是否有关联时候使用
    void GetSolImg(Mat& dir, int sel = SOL_GRAY);//获取img_gray, img_raw或img_t
    ~Solu(){};
};



inline Solu::Solu(const Mat& src){
    img_raw = src.clone();
    cvtColor(img_raw, img_gray, CV_BGR2GRAY);
    img_t = img_gray.clone();
    img = img_t;
    ROI.x = 0; ROI.y = 0; ROI.width = img_raw.cols; ROI.height = img_raw.rows;
}



inline void Solu::LoadImgt(int sel){
    if (sel == SOL_GRAY){
        img_gray.copyTo(img_t);
        return;
    }

    if (sel == SOL_RAW){
        img_raw.copyTo(img_t);
        return;
    }

    cout << "in Solu::LoadImgt\n" << "input correct sel: 0 or 1" << endl;
}



inline void Solu::SetRoi(const Rect &r){
    ROI = r;
    img = img_t(ROI);
}



inline void Solu::GetRoi(Mat &dir, int sel){
    if (sel == SOL_GRAY){
        img_gray(ROI).copyTo(dir);
        return;
    }

    if (sel == SOL_RAW){
        img_raw(ROI).copyTo(dir);
        return;
    }

    if (sel == SOL_TEMP){
        img_t(ROI).copyTo(dir);
        return;
    }

    cout << "in Solu::GetRoi\n" << "input correct sel: 0, 1 or 2" << endl;
}



inline void Solu::CutRoi(const Rect &r){
    ROI = r;
    img_t(r).copyTo(img);
}



inline void Solu::CutRla(){
    img = img.clone();
}



inline void Solu::GetSolImg(Mat &dir, int sel){
    if (sel == SOL_GRAY){
        img_gray.copyTo(dir);
        return;
    }

    if (sel == SOL_RAW){
        img_raw.copyTo(dir);
        return;
    }

    if (sel == SOL_TEMP){
        img_t.copyTo(dir);
        return;
    }

    cout << "in Solu::GetRoi\n" << "input correct sel: 0, 1 or 2" << endl;
}





void WaterShed(Solu& S);//分水岭
void S_Run(Solu &S);
void GetWaterEdge(const Mat &src, const Rect &r, Mat& edge);



#endif

