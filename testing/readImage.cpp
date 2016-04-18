#include <opencv2/highgui/highgui.hpp>
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
//#include "opencv2/contrib/contrib.hpp"
#include <vector>
using namespace cv;
using namespace std;


class Image{
 public:
	Image(){};
    Image(int w, int h, Mat image);
    Mat getCompressed();
    void displayFilter(Mat, int);
    void divide_image();
    vector<vector<vector<int> > > imageRGB;//(w, vector<vector<int> >(w, vector<int>(3))); // 3-d vector to store the rgb data
    vector<vector<vector<int> > > imageYBR;//(h, vector<vector<int> >(w, vector<int>(3))); // 3-d vector to store the luminance,Cb,Cr data

private:
	Mat compressedImage = imread("flower.jpg", CV_LOAD_IMAGE_COLOR);
	int width;
	int height;
	vector<vector<Mat> > sub_images;// 2-d vector of sub-images
};

// sub image is an image: 8x8 sub images will be instantiated
class Sub_Image: public Image{
 public:
	Sub_Image();
    Sub_Image(int w, int h, Mat image, int num_row, int num_col);
    void compress_sub_image();
    void cosine_transform();
	Mat self;
 private:
    int row;
    int col;
    int width;
    int height;
    Image father;
};



int main(){
    Mat image = imread("flower.jpg", CV_LOAD_IMAGE_COLOR); // reads in image into image object from the Matrix? class, not sure what CV_LO... is 
    Mat imageYCBCR; // this was code I found online to convert RGB to YcBcR, but i couldnt get CV_BGR2YCBCR to function
    cvtColor(image,imageYCBCR,CV_BGR2YCrCb); // converts image from BGR to ycbcr YCBCR 
    Mat filter[3];
    split(imageYCBCR,filter);
    Mat y = filter[0];
    Mat cr = filter[1];
    Mat cb = filter[2];
    Image compressed(image.cols, image.rows, image); 
    compressed.displayFilter(image, 0);  // this doesn't do what I want it to do
	imshow("opencvtest", compressed.getCompressed()); // displays the image with title opencvtest
//	compressed.divide_image();
//	imshow("ycbr", y);
//	imshow("luminance", y);
    imshow("lumiance", y);
    imshow("RED:", cr);
    imshow("BLUE:", cb);
    imshow("FLOWER:", image);	
    waitKey(0); // not sure what this does
    return 0;
}

// displays either the Luminance or Chromiance of an image
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

Sub_Image::Sub_Image(){
	width = 1;
	height = 1;
	row = 1;
	col = 1;
}

// non-default constructor for sub-image: sets Mat.color position to the imageRGB of inherited Image 
Sub_Image::Sub_Image(int w, int h, Mat image, int num_row, int num_col){
    width = w;
    height = h;
    row = num_row;
    col = num_col;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            // setting the self's color equal to its RGB position
            self.at<Vec3b>(i, j)[2] = imageRGB[num_row*i][num_col*j][2];
            self.at<Vec3b>(i, j)[1] = imageRGB[num_row*i][num_col*j][1];
            self.at<Vec3b>(i, j)[0] = imageRGB[num_row*i][num_col*j][0];
            // does sub_image inherit vector,vectorRGB?
        }
    }
}

// Since the human eye is less sensitive to an image's difference in chromiance
// we can down-sample the red and blue of the image from the pixels to 2x2 blocks of pixels
void Sub_Image::compress_sub_image(){
    int average_blue = 0;
    int average_red = 0;
	int y, cB, cR;
    for(int i = row; i < row + (width / 2); i++){
        for(int j = col; j < col + (height / 2); j++){

            // Blue Chromiance is stored here
            average_blue = (father.imageYBR[i][j][1] + father.imageYBR[i + 1][j][1] + father.imageYBR[i][j + 1][1] + father.imageYBR[i + 1][j + 1][1])/4;

            // Red Chromiance is stored here
            average_red = (father.imageYBR[i][j][2] + father.imageYBR[i + 1][j][2] + father.imageYBR[i][j + 1][2] + father.imageYBR[i + 1][j + 1][2])/4;

            // Assign the father image the compressed chromiance values            
            father.imageYBR[i][j][1] = father.imageYBR[i + 1][j][1] = father.imageYBR[i][j + 1][1] = father.imageYBR[i + 1][j + 1][1] = average_blue;
            father.imageYBR[i][j][2] = father.imageYBR[i + 1][j][2] = father.imageYBR[i][j + 1][2] = father.imageYBR[i + 1][j + 1][2] = average_red;

			y = father.imageYBR[i][j][0];
			cB = father.imageYBR[i][j][1];
			cR = father.imageYBR[i][j][2];
			
			// give father image compressed RGB data translated from YcBcR
			father.imageRGB[i][j][0] = 1.0 * y + 0 * cB + 1.402 * cR; 
			father.imageRGB[i][j][1] = 1.0 * y - 0.344136 * cB - 0.714136 * cR; 
			father.imageRGB[i][j][2] = 1.0 * y + 1.772 * cB + 0 * cR; 

			// self assign commpressed RGB data
			self.at<Vec3b>(i, j)[2] = father.imageRGB[i][j][2];
            self.at<Vec3b>(i, j)[1] = father.imageRGB[i][j][1];
            self.at<Vec3b>(i, j)[0] = father.imageRGB[i][j][0];
            

        }
    }
    
    
}

// divide image into 8x8 sub images
void Image::divide_image(){
    for(int i = 0; i < 8; i++){
        vector<Mat> temp;
        for(int j = 0; j < 8; j++){
            Sub_Image sub_image(width / 8, height / 8, getCompressed(), i, j);
		//	sub_image.compress_sub_image();
			temp.push_back(sub_image.self); 
        }
        sub_images.push_back(temp);
        temp.clear();
    }
}

Image::Image(int w, int h, Mat image){
    width = w;
    height = h;
    int temp, r, g, b;
//populate imageRGB matrix with rgb values of the image, now that i think about it, this is unnessesary since OpenCV already has the values stored which we can acces    s at any time, I leave it for now just in case this is more intuitive
    vector<vector<int> > tempVectorRGB;
    vector<int> innerTempVectorRGB;
    vector<vector<int> > tempVectorYBR;
    vector<int> innerTempVectorYBR; // should we use ints or double for this?????????????????????????????
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
