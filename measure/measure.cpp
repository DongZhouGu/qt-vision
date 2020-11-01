#include "measure.h"
#include <stdio.h>
#include "standard.h"   //标定参数获取
#include "camera/cameracrl.h"  //相机控制
#include "mainwindow.h"


void DispPts(Mat &fitpic, const Mat &img, const Rect &r,
             const vector<vecP2d> &vpts, const Mat &CamaraM,
             const Mat &DisCoeff, double RmProp_fit, double RmProp_uldist,
             double RmProp_mdist) {
  ////对所有点进行反畸变处理(点的坐标是在整张图像上的坐标)
  // for (int i = 0; i < vpts.size(); i++) {
  //    undistortPoints(vpts[i], vpts[i], CamaraM, DisCoeff, cv::noArray(),
  //    CamaraM);
  //}
  ULPts S_Pts(vpts);
  ////图像反畸变
  // undistort(img, fitpic, CamaraM, DisCoeff);
  // fitpic = fitpic(r);
  img.copyTo(fitpic);
  fitpic = fitpic(r);


  //对1,2两组点进行直线拟合(获得的直线方程是在整张图片上的)
  S_Pts.Fitting(1, RmProp_fit, 1e-2);
  S_Pts.Fitting(2, RmProp_fit, 1e-2);
  double a1, b1, c1, a2, b2, c2;
  S_Pts.GetLines(1, a1, b1, c1);
  S_Pts.GetLines(2, a2, b2, c2);
  //求roi内坐标与图像上直线的差值
  double diss1 = a1 * r.x + b1 * r.y + c1;
  double diss2 = a2 * r.x + b2 * r.y + c2;
  //显示点
  for (int i = 0; i < vpts.size(); i++)
    for (int j = 0; j < vpts[i].size(); j++)
      circle(fitpic, Point(vpts[i][j].x - r.x, vpts[i][j].y - r.y), 5,
             Scalar(0, 0, 255), 1);
  //显示直线
  for (int i = 0; i < fitpic.rows; i++)
    for (int j = 0; j < fitpic.cols; j++)
      //加上diss可以得到整张图像上的坐标否在直线上
      if (fabs(a1 * j + b1 * i + diss1) <= 1.5 ||
          fabs(a2 * j + b2 * i + diss2) <= 1.5) {
        fitpic.at<Vec3b>(i, j)[0] = 0;
        fitpic.at<Vec3b>(i, j)[1] = 255;
        fitpic.at<Vec3b>(i, j)[2] = 0;
      }


}

int SamplePts(const Mat &mark, vector<vecP2d> &vpts, const double offsetx,
              const double offsety, const int num) {
  if (num < 0) {
    cout << "in SamplePts: \n"
         << "the number of sample points should not be negtive" << endl;
    return 0;
  }

  if (num >= mark.cols - 8) {
    cout << "in SamplePts: \n"
         << "the number of sample points is too large" << endl;
    return 0;
  }

  vecP2d pts[4];
  int step = (mark.cols - 9) / (num - 1);

  for (int j = 4; j < mark.cols - 4; j += step) {
    int fg1 = 0, fg2 = 0, fg3 = 0, fg4 = 0;
    //��ȡ������һ���
    for (int i = 4; i < mark.rows / 2; i++) {
      if (mark.at<int>(i, j) == -1) {
        pts[0].push_back(Point2d(j + offsetx, i + offsety));
        fg1 = 1;
        break;
      }
    }
    if (fg1 == 0) {
      pts[0].push_back(Point2d(j + offsetx, 4 + offsety));
    }
    //��ȡ�ڶ����
    for (int i = mark.rows / 2 - 1; i > 4; i--) {
      if (mark.at<int>(i, j) == -1) {
        pts[1].push_back(Point2d(j + offsetx, i + offsety));
        fg2 = 1;
        break;
      }
    }
    if (fg2 == 0) {
      pts[1].push_back(Point2d(j + offsetx, mark.rows / 2 + offsety));
    }
    //��ȡ�������
    for (int i = mark.rows / 2; i < mark.rows - 4; i++) {
      if (mark.at<int>(i, j) == -1) {
        pts[2].push_back(Point2d(j + offsetx, i + offsety));
        fg3 = 1;
        break;
      }
    }
    if (fg3 == 0) {
      pts[2].push_back(Point2d(j + offsetx, mark.rows / 2 + offsety));
    }
    //��ȡ�������
    for (int i = mark.rows - 5; i > mark.rows / 2; i--) {
      if (mark.at<int>(i, j) == -1) {
        pts[3].push_back(Point2d(j + offsetx, i + offsety));
        fg4 = 1;
        break;
      }
    }
    if (fg4 == 0) {
      pts[3].push_back(Point2d(j + offsetx, mark.rows - 5 + offsety));
    }
  }

  for (int i = 0; i < 4; i++) {
    vpts.push_back(pts[i]);
  }

  return 1;
}

int DistofPts(const vector<vecP2d> &vpts, double dist[3], const Mat &CamaraM,
              const Mat &Tvec, const Mat &Rmat, const Mat &DisCoeff,
              double RmProp_fit, double RmProp_uldist, double RmProp_mdist) {
  ULPts S_Pts(vpts);

  for (int i = 0; i < S_Pts.GetSize(); i++)
    if (!S_Pts.TransPts(CamaraM, Tvec, Rmat, DisCoeff, i)) {
      cout << "in DistofPts: \n"
           << "fail to transform points" << endl;
      return 0;
    }

  if (!S_Pts.Fitting(1, RmProp_fit, 1e-2)) {
    cout << "in DistofPts: \n"
         << "fail to fit line 1" << endl;
    return 0;
  }
  if (!S_Pts.Fitting(2, RmProp_fit, 1e-2)) {
    cout << "in DistofPts: \n"
         << "fail to fit line 2" << endl;
    return 0;
  }

  if (!S_Pts.GetDistPtL(dist[0], 0, 1, RmProp_uldist, ULPTS_LW)) {
    cout << "in DistofPts: \n"
         << "fail to GetDistPtL 0 1" << endl;
    return 0;
  }
  if (!S_Pts.GetDistPtL(dist[1], 1, 2, RmProp_mdist, ULPTS_UL)) {
    cout << "in DistofPts: \n"
         << "fail to GetDistPtL 0 1" << endl;
    return 0;
  }
  if (!S_Pts.GetDistPtL(dist[2], 3, 2, RmProp_uldist, ULPTS_LW)) {
    cout << "in DistofPts: \n"
         << "fail to GetDistPtL 3 2" << endl;
    return 0;
  }

  return 1;
}

int GetPts(vector<vecP2d> &vpts, Mat &edge, const Mat &img, const Rect &r,
           const int num) {
  Mat mark;
  //输入原图像,roi框r,输出roi在上提取的边界mark
  GetWaterEdge(img, r, mark);

  img.copyTo(edge);
  edge = edge(r);
  //将边界显示在原roi图上
  for (int i = 0; i < mark.rows; i++)
    for (int j = 0; j < mark.cols; j++)
      if (mark.at<int>(i, j) == -1) {
        edge.at<Vec3b>(i, j)[0] = 0;
        edge.at<Vec3b>(i, j)[1] = 0;
        edge.at<Vec3b>(i, j)[2] = 255;
      }

  //采集边界上的点,点的坐标是在原图上的坐标,vpts是四组点
  if (!SamplePts(mark, vpts, r.x, r.y, num)) {
    cout << "in Measure: \n"
         << "fail to Sample points" << endl;
    return 0;
  }
  return 1;
}

int Measure(Mat &edge, Mat &fitpic, double dist[3], const Mat &img,
            const Rect &r, const Mat &CamaraM, const Mat &Tvec, const Mat &Rmat,
            const Mat &DisCoeff, int num, double RmProp_fit,
            double RmProp_uldist, double RmProp_mdist) {
  vector<vecP2d> vpts;
  if (!GetPts(vpts, edge, img, r, num)) {
    cout << "in Measure: \n"
         << "fail to get points" << endl;
    return 0;
  }
  // edge.copyTo(fitpic);
  // cout << vpts[0].size()<< endl;
  DispPts(fitpic, img, r, vpts, CamaraM, DisCoeff, RmProp_fit, RmProp_uldist,
          RmProp_mdist);

  if (!DistofPts(vpts, dist, CamaraM, Tvec, Rmat, DisCoeff, RmProp_fit,
                 RmProp_uldist, RmProp_mdist)) {
    cout << "in Measure: \n"
         << "fail to get the distance of points" << endl;
    return 0;
  }

  return 1;
}

int MainMeasure(Mat &img,Mat &fitpic,double &mainwidth,double &sidewidth1,double &sidewidth2){

//    MVCamera camera;
//    if (camera.ConnectCamera() != MV_OK){
//        cout << "connect camera error!" << endl;
//        return 0;
//    }

//    if (camera.GetFrame() != MV_OK){
//        cout << "getframe error!" << endl;
//        return 0;
//    }

//    if (!camera.Image(img)){
//        cout << "convert image error!" <<endl;
//        return 0;
//    }


    Rect r;
    Mat CamaraM, Tvec, Rmat, DisCoeff,edge;
    GetCmaraParam(CamaraM, Tvec, Rmat, DisCoeff);
    double dist[3];
    r = Rect(int(img.cols / 3 - 20), int(img.rows * 3 / 7 - 40), int(img.cols - img.cols / 4 - img.cols / 3 - 80),
           int(img.rows / 7 + 90));

    Measure(edge, fitpic, dist, img, r, CamaraM, Tvec, Rmat, DisCoeff, 100);

    sidewidth1=dist[0];
    mainwidth=dist[1];
    sidewidth2=dist[2];

    return 0;

}

QImage MatToQImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {

        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug("ERROR: Mat could not be converted to QImage.");
        return QImage();
    }
}

