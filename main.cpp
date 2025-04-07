#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <set>

using namespace std;
using namespace cv;

vector<vector<Point>> dians;
float w =500, h = 600;
vector<Point> xiuzhengdian(vector<Point> dian) {
    Point temp;
    vector<int> hes, chas;
    for (int a =0 ; a < 4; a++) {
        for (int b = a + 1; b < 4; b++) {
            if (dian[a].y > dian[b].y) {
                swap(dian[a], dian[b]);
            }
        }
    }
    if (dian[0].x > dian[1].x) swap(dian[0], dian[1]);
    if (dian[2].x > dian[3].x) swap(dian[2], dian[3]);
    return dian;
}
Mat bianyuan(Mat img) {
    Mat imgGaus, imgDil, imgCan,imgGray;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgGaus, Size(3, 3), 3, 0);
    Canny(imgGaus, imgCan, 70, 90);
    Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    dilate(imgCan, imgDil, kernel);
    return imgDil;
}
Mat biao_dian(Mat yuan,Mat img) {
    vector<vector<Point>> contours;
    vector<vector<Point>> moni;
    vector<Vec4i> peir;
    findContours(img, contours, peir, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    moni = contours;
    cout << contours.size() << endl;
    int max[2] = { 0 };
    for (int a = 0; a < contours.size(); a++) {
        float peri = arcLength(contours[a], true);
        approxPolyDP(contours[a], moni[a], peri * 0.02, true);
        int area = contourArea(moni[a]);
        if (area > max[0]&&moni[a].size()==4) {
            max[0] = area;
            max[1] = a;
            cout << area << endl;
        }
    }
    cout << max[1] << endl;
    /*Rect juxing;
    juxing = boundingRect(moni[max[1]]);
    rectangle(yuan, juxing, Scalar(0, 0, 255), 5);*/

    vector<Point> jisuandian = moni[max[1]];
    vector<Point> jieguo=xiuzhengdian(jisuandian);
    //修正
    Mat matrix, imgWarp;
    Point2f src[4] = { jieguo[0],jieguo[1],jieguo[2],jieguo[3] };
    Point2f dst[4] = { {0,0},{w,0},{0,h},{w,h} };
    matrix = getPerspectiveTransform(src, dst);
    warpPerspective(yuan, imgWarp, matrix, Point(w, h));
    //jieguo = jisuandian;
    drawContours(yuan, moni, max[1], Scalar(0, 255, 0), 3);

    circle(yuan, jieguo[0], 8, Scalar(255, 0, 0), FILLED);
    putText(yuan, "1", jieguo[0], FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 2);
    circle(yuan, jieguo[1], 8, Scalar(255, 0, 0), FILLED);
    putText(yuan, "2", jieguo[1], FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 2);
    circle(yuan, jieguo[2], 8, Scalar(255, 0, 0), FILLED);
    putText(yuan, "3", jieguo[2], FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 2);
    circle(yuan, jieguo[3], 8, Scalar(255, 0, 0), FILLED);
    putText(yuan, "4", jieguo[3], FONT_HERSHEY_COMPLEX, 0.7, Scalar(0, 255, 0), 2);
    imshow("yuan", yuan);

    return imgWarp;
}
int main() {
    //string path = "cvceshi/xiuzheng3.png";
    //Mat img = imread(path);
    Mat img;
    VideoCapture cap(0);
    while (1) {
        cap.read(img);
        Mat imgXiu, imgErzhi;
        //resize(img, imgXiu, Size(), 0.6, 0.6);
        imgXiu = img;
        imgErzhi = bianyuan(imgXiu);

        imshow("imgXiu", biao_dian(imgXiu, imgErzhi));
        waitKey(5);
    }
    return 0;
}