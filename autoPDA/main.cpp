#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace cv;
using namespace std;

Mat trim(Mat src, int mag)
{
    int cutC, cutR;
    cutC = src.cols % mag;
    cutR = src.rows % mag;
    
    Mat roi;
    Size rsize(src.cols - cutC, src.rows - cutR);
    resize(src, roi, rsize, INTER_CUBIC);
    
    return roi;
}

Mat mozaic(Mat src)
{
    int sum = 0, mag = 12;
    Mat dst(src.size(), src.type());
    
    for (int y = 0; y < src.rows; y += mag)
    {
        for (int x = 0; x < src.cols; x += mag)
        {
            for (int i = 0; i < mag; i++)
            {
                for (int j = 0; j < mag; j++)
                {
                    sum += src.data[(y + i) * src.step + (x + j)];
                }
            }
            
            for (int i = 0; i < mag; i++)
            {
                for (int j = 0; j < mag; j++)
                {
                    dst.data[(y + i) * dst.step + (x + j)] = (int)(sum/(12*12));
                }
            }
            sum = 0;
        }
    }
    
    return dst;
}

int main(int argc, char *argv[])
{
    string filename = "lena.jpg";
    
    int cnum, rnum, mag = 12, lnum = 7;
    string fd[7];
    Mat dmat[7];
    
    for (int i = 0; i < lnum; i++)
        fd[i] = to_string(i) + ".png";
    for (int i = 0; i < lnum; i++)
        dmat[i] = imread(fd[i], 0);
    
    Mat src = imread(filename, 0);
    Mat dst(src.size(), src.type());
    
    src = trim(src, mag);
    dst = mozaic(src);
    cnum = src.cols / mag;
    rnum = src.rows / mag;
    
    int result[rnum][cnum];
    
    Mat *himg = new Mat[cnum];
    Mat *vimg = new Mat[rnum];
    
    for (int y = 0; y < src.rows; y += mag)
    {
        for (int x = 0; x < src.cols; x += mag)
        {
            result[y/mag][x/mag] = dst.data[(y) * dst.step + (x)];
        }
    }
    
    int max = 0, min = 255;
    for (int i = 0; i < rnum; i++)
    {
        for (int j = 0; j < cnum; j++)
        {
            if (result[i][j] >= max)
                max = result[i][j];
            if (result[i][j] <= min)
                min = result[i][j];
        }
    }
    int ld;
    int level[256] = {0};
    ld = (int)((max - min) / lnum);
    
    int end = 0, cnt = 0;
    for (int i = max; i > min+ld; i -= ld)
    {
        for (int j = 0; j < ld; j++)
        {
            level[i-j] = cnt;
            end = i - j;
        }
        cnt++;
    }
    while (end >= min)
    {
        level[end] = 6;
        end--;
    }
    
    cnt=0;
    Mat vdst;
    for (int i = 0; i < rnum; i++)
    {
        for (int j = 0; j < cnum; j++)
        {
            himg[j] = imread(fd[level[result[i][j]]], 0);
        }
        hconcat(himg, cnum, vimg[i]);
    }
    
    vconcat(vimg, rnum, vdst);
    
    namedWindow("gray", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    imshow("gray", src);
    
    namedWindow("mozaic", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    imshow("mozaic", dst);
    
    namedWindow("dice", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    imshow("dice", vdst);
    
    delete[] himg;
    delete[] vimg;
    
    waitKey(0);
}