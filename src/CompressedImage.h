/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * CompressedImage.h
 * 		Class definition for the CompressedImage class
 *
 *		This class inherits from the Image class found in Image.h and implented in Image.cpp
 *
 * 		This class actually performs all of the image compression
 * 		We basically compress in two steps:
 * 			First, we do downsampling which basically averages the chrominance
 * 			values of all 2x2 blocks of pixels. This is done because the human is
 * 			not great at percieving small chominance differences
 *
 * 			Second, we perform a Discrete Cosine Transform (DCT), quantized the DCT,
 * 			and then preformed the inverse DCT and conversion back to the RGB color
 * 			space. This is the heart of our compression strategy and idealy where
 * 			the compression ration is acheived.
 *
 */

#ifndef COMPRESSED_IMAGE_H
#define COMPRESSED_IMAGE_H

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

#include "Image.h"

using namespace cv; //OpenCV namespace
using namespace std;


class CompressedImage: public Image{
	public:
		CompressedImage(Image im);
		
		void displayFilter(int n);

	private:
		Image image;
		vector< vector<SubImage> > subIms;
		vector< vector<SubImage> > compressedSubIms;
		int q; // integer quality value between 0-100 (higher = better quality)
		Mat compressedIm;
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		void compressYCbCrAverages(SubImage si);
		void compressDiscreteCosine(SubImage csi);
		
		void compressSubIms();
		void compressIm();
		void fillRGB();
		void fillYCbCr();
};


#endif

