/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * Image.cpp
 * 		Implenation of the Image class found in Image.h
 *
 * 		Base class for CompressedImage
 * 		Composed of SubImage objects
 * 		
 */

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

#include "Image.h"
#include "SubImage.h"
#include "CompressedImage.h"

using namespace cv; //OpenCV namespace
using namespace std;


//constructor
Image::Image(){}
Image::Image(string fp){
	imagePath = fp;
	image = imread(imagePath, CV_LOAD_IMAGE_COLOR); //read the image into a Mat     
	/*
	* imread second argument options for future reference:
	* 	CV_LOAD_IMAGE_UNCHANGED (<0) loads the image as is (including the alpha channel if present)
	* 	CV_LOAD_IMAGE_GRAYSCALE ( 0) loads the image as an intensity one
	* 	CV_LOAD_IMAGE_COLOR (>0) loads the image in the BGR format
	*/

	while(!image.data ){ //checking that image data was correctly loaded
		cout << "Could not open or find the image \"" << imagePath << "\"" << endl ;
		cout << "Enter an image path: ";
		cin >> imagePath;
		image = imread(imagePath, CV_LOAD_IMAGE_COLOR); //read the image into a Mat 
	}

	//set the image width and height
	width = image.cols;
	height = image.rows;

	//divide the image to a 2D vector of subims
	divideImage();

	//fill in the RGB and YCbCr
	fillRGB();
	fillYCbCr();
}

//divides image to composition of SubImage objects stored in 2D vector
void Image::divideImage(){
	//loop through 8x8 blocks of pixels and store them in SubImages
	for(int i = 0; i < height; i+=8){
		vector<SubImage> tmp;
		for(int j = 0; j < width; j+=8){
			SubImage tmpSubImage(image, i, j);
			tmp.push_back(tmpSubImage);
		}
		//push back the SubImages into the 2D vector storing them all
		subIms.push_back(tmp);
		tmp.clear();
	}
}

//fills the RGB vector of vectors for all the pixels
void Image::fillRGB(){
	//temp variables to store information before push_backing
	int r, g, b;
	vector< vector<int> > tmpRGB;
	vector<int> innerTmpRGB;
	
	//loop through all of the pixels and store the rgb data
	for(int i = 0 ; i < height; i++){
		for(int j = 0; j < width; j++){
			r = image.at<Vec3b>(i, j)[2];
			g = image.at<Vec3b>(i, j)[1];
			b = image.at<Vec3b>(i, j)[0];

			innerTmpRGB.push_back(r);
			innerTmpRGB.push_back(g);
			innerTmpRGB.push_back(b);
			
			tmpRGB.push_back(innerTmpRGB);
			innerTmpRGB.clear();
		}
		RGB.push_back(tmpRGB);
		tmpRGB.clear();
	}
}


//fills the YCbCr color space vector of vectors for all the pixels
void Image::fillYCbCr(){
	//temp variables to store information before push_backing
	int r, g, b;
	vector< vector<int> > tmpYCbCr;
	vector<int> innerTmpYCbCr;
	
	//loop through all of the pixels and get the YCbCr data
	for(int i = 0 ; i < height; i++){
		for(int j = 0; j < width; j++){
			r = image.at<Vec3b>(i, j)[2];
			g = image.at<Vec3b>(i, j)[1];
			b = image.at<Vec3b>(i, j)[0];

			//converts from RGB color space to the YCbCr colorspace
			//this is done using linear algebra and matrix transformations
			innerTmpYCbCr.push_back(r*0.299 + g*0.587 + b*0.114);
			innerTmpYCbCr.push_back(r*-0.16874 + g*-0.33126 + b*0.5);
			innerTmpYCbCr.push_back(r*0.5 + g*-0.41869 + b*-0.08131);
			
			tmpYCbCr.push_back(innerTmpYCbCr);
			innerTmpYCbCr.clear();
		}
		YCbCr.push_back(tmpYCbCr);
		tmpYCbCr.clear();
	}
}

//displays y, Cb or Cr based on the int passed (0-6)
//0: Actual, 1: r, 2: g, 3: b, 4: Y, 5: Cb, 6: Cr
void Image::displayFilter(int n){
	//clone the image into a temp Mat to edit it to display filters
	Mat tmp = image.clone();

	//loop through all of the pixels of the temp Mat and adjust the
	//color spaces to the appropriate one
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			switch(n){
				case 0:
					tmp.at<Vec3b>(i, j)[2] = RGB[i][j][0];
					tmp.at<Vec3b>(i, j)[1] = RGB[i][j][1];
					tmp.at<Vec3b>(i, j)[0] = RGB[i][j][2];
					break;
				case 1:
					tmp.at<Vec3b>(i, j)[2] = RGB[i][j][0];
					tmp.at<Vec3b>(i, j)[1] = 0;
					tmp.at<Vec3b>(i, j)[0] = 0;
					break;
				case 2:                                         
					tmp.at<Vec3b>(i, j)[2] = 0;
					tmp.at<Vec3b>(i, j)[1] = RGB[i][j][1];
					tmp.at<Vec3b>(i, j)[0] = 0;
					break;
				case 3: 
					tmp.at<Vec3b>(i, j)[2] = 0;
					tmp.at<Vec3b>(i, j)[1] = 0;
					tmp.at<Vec3b>(i, j)[0] = RGB[i][j][2];
					break;
				case 4:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][0]*0.2362797506;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][0]*0.2362797506;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][0]*0.2362797506;
					break;
				case 5:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][1]*-0.00000169;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][1]*-0.08131169;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][1]*0.41868831;
					break;
				case 6:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][2]*0.33126364;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][2]*-0.16873636;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][2]*0.0000037;
					break;

			}
		}
	}
	
	//switch between to display the proper one once it has been edited
	//and converted to the appropriate color space
	switch(n){
		case 0:
			imshow("Image", tmp);
			break;
		case 1: 
			imshow("Red Channel", tmp);
			break;
		case 2:
			imshow("Green Channel", tmp);
			break;
		case 3:
			imshow("Blue Channel", tmp);
			break;
		case 4:
			imshow("Luminance", tmp);
			break;
		case 5:
			imshow("Blue Chrominance", tmp);
			break;
		case 6:
			imshow("Red Chrominance", tmp);
			break;
	}
}

//The folowing functions are accessors that just return a copy of the data member
//Yay oop and data hiding!

Mat Image::getImage(){
	return image;
}

vector< vector<SubImage> > Image::getSubIms(){
    return subIms;
}

vector< vector< vector<int> > > Image::getRGB(){
    return RGB;
}

vector< vector< vector<int> > > Image::getYCbCr(){
    return YCbCr;
}


