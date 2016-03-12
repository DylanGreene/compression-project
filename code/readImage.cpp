//code copied from https://help.ubuntu.com/community/OpenCV
//testing to see if OpenCV library functions
//Libraries were pasted as instructed to from OpenCV tutorial
#include<opencv2/highgui/highgui.hpp>
#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include <vector>
using namespace cv;



class Image{
 public:
	Image(int w, int h, Mat image);
	Mat getCompressed();
	void displayFilter(Mat, int);
 private:
	Mat compressedImage = imread("flower.jpg", CV_LOAD_IMAGE_COLOR);
	int width;
	int height;
   std::vector<vector<vector<int> > > imageRGB;//(w, vector<vector<int> >(w, vector<int>(3))); // 3-d vector to store the rgb data
   std::vector<vector<vector<int> > > imageYBR;//(h, vector<vector<int> >(w, vector<int>(3))); // 3-d vector to store the luminance,Cb,Cr data

};





int main(){
	Mat image = imread("flower.jpg", CV_LOAD_IMAGE_COLOR); // reads in image into image object from the Matrix? class, not sure what CV_LO... is
//	Mat ycbcr;
//	cvtColor(image,ycbcr,CV_BGR2YCBCR);
//	Mat chan[3];
//	split(ycbcr,chan);
//	Mat y = chan[0];
//	Mat cb = chan[1];
//	Mat cr = chan[2];
	Image compressed(image.cols, image.rows, image);
	compressed.displayFilter(image, 0);
//	imshow("opencvtest", compressed.getCompressed()); // displays the image with title opencvtest
//	imshow("ycbr", y);
	waitKey(0); // not sure what this does
	return 0;
}


void Image::displayFilter(Mat image, int filterIndex){
	for(int i = 0; i < image.rows; i++){
		for(int j = 0; j < image.cols; j++){
			image.at<Vec3b>(i, j)[2] = imageYBR[i][j][filterIndex];
			image.at<Vec3b>(i, j)[1] = imageYBR[i][j][filterIndex];
			image.at<Vec3b>(i, j)[0] = imageYBR[i][j][filterIndex];
		}
	}
	imshow("Filtered Image:", image);
}



Image::Image(int w, int h, Mat image){
	width = w;
	height = h;
	int temp, r, g, b;
 //populate imageRGB matrix with rgb values of the image, now that i think about it, this is unnessesary since OpenCV already has the values stored which we can acces    s at any time, I leave it for now just in case this is more intuitive
	std::vector<vector<int> > tempVectorRGB;
	std::vector<int> innerTempVectorRGB;
	std::vector<vector<int> > tempVectorYBR;
	std::vector<int> innerTempVectorYBR; // should we use ints or double for this?????????????????????????????
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			r = image.at<Vec3b>(i, j)[2];
			g = image.at<Vec3b>(i, j)[1];
			b = image.at<Vec3b>(i, j)[0];

			innerTempVectorRGB.push_back(r);
			innerTempVectorRGB.push_back(g);
			innerTempVectorRGB.push_back(b);
			tempVectorRGB.push_back(innerTempVectorRGB);
			innerTempVectorRGB.clear();
			innerTempVectorYBR.push_back(r*.299 + g*.587 + b*.114);
			innerTempVectorYBR.push_back(r*-.16874 + g*-.33126 + b*.5);
			innerTempVectorYBR.push_back(r*.5 + g*-.41869 + b*-.08131);
			tempVectorYBR.push_back(innerTempVectorYBR);
			innerTempVectorYBR.clear();
		}		
		imageRGB.push_back(tempVectorRGB);
		tempVectorRGB.clear();
		imageYBR.push_back(tempVectorYBR);
		tempVectorYBR.clear();
	}
}


Mat Image::getCompressed(){
	return compressedImage;
}
