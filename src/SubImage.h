/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * SubImage.h
 * 		Class definition for the SubImage class
 *
 * 		Basically stores 8x8 blocks of images which Image objects are composed of
 * 		It is necessary to create the SubImages to calculate the Discrete Cosine Transform
 * 		(DCT) which is the heart of the compression algortihm
 *
 */

#ifndef SUB_IMAGE_H
#define SUB_IMAGE_H

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

using namespace cv; //OpenCV namespace
using namespace std;


class SubImage{ 
	public:
	 	SubImage(Mat im, int row, int col); //non-default constructor
		
		//accessors with self explanatory names
		Mat getSubImage();
		int getRow();
		int getCol();
		vector< vector< vector<int> > > getRGB();
		int getRGB(int i, int j, int n);
		int getYCbCr(int i, int j, int n);
		vector< vector< vector<int> > > getYCbCr();
		void setYCbCr(int i, int j, int n, int v);		
		void setRGB(int i, int j, int n, int v);		

	private:
		Mat subIm; //the sub image
		
		//vectors to store the SubImage color space data 8x8x3 vectors
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		
		//position in parent image
		int row;
		int col;

		//member helper functions which fill the color space vectors
		void fillRGB();
		void fillYCbCr();

};


#endif

