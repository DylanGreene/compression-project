/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * Image.h
 * 		Class definition for the Image class
 *
 * 		Composed of SubImage objects
 * 		Base class for CompressedImage
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>
#include <string>

#include "SubImage.h"

using namespace cv; //OpenCV namespace
using namespace std;

class Image{ 
	public:
		Image(); //default constructor
		Image(string fp); //non default constructor which takes an OpenCV Mat
		
		//displays various filters based on n
		//for n = 0: Original, 1: R, 2: G, 3: B, 4: Y, 5: Cb, 6: Cr
		virtual void displayFilter(int n); 
		virtual void saveFilter(int n);

		//accessors
		Mat getImage(); //returns the OpenCV Mat of the image
		vector< vector<SubImage> > getSubIms(); //returns subIms
		vector< vector< vector<int> > > getRGB(); //returns 3D vector of RGB data
		vector< vector< vector<int> > > getYCbCr(); //returns 3D vector of YCbCr data

	private:
		string imagePath; //path to orignal image
		Mat image; //the OpenCV Mat 
		
		//dimensions of image
		int width; 
		int height;

		//color space data
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		
		//Composed of these SubImage objects
		vector< vector<SubImage> > subIms;

		void divideImage(); //divided image into 8x8 sub images
		
		//virtual functions to fill the color space vectors 
		virtual void fillRGB();
		virtual void fillYCbCr();
};


#endif

