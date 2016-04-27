/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * SubImage.cpp
 * 		Implenation of the SubImage class found in SubImage.h
 *
 * 		Creates 8x8x3 subimages which are crucial for the Discrete
 * 		Cosine Transform which is the heart of our compression strategy.
 * 		The DCT is effective over the 8x8 subimages by reducing the high 
 * 		frequencies which the human eye is not sensitive to
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


// Constructor
// non-default, takes the original mat is decomposing as well as
// the position in the mat so it knows the place for use when recomposing
SubImage::SubImage(Mat im, int r, int c){
	//clone the full image as the sub image for same size Mat
	subIm = im.clone();
	
	//store the position in of the 8x8 subimage in the original
	row = r;
	col = c;

	//fill the 8x8x3 vectors for the subimage with appropriate color space data
	fillRGB();
	fillYCbCr();

	/*
	*	Used for testing the when displaying the subimages.
	*	Since the image size is more easily manipulated when
	*	Mat sizes are constant, this was used to set unused
	*	pixels to be black  	
	*
	for(int i = 0; i < subIm.rows; i++){
		for(int j = 0; j < subIm.cols; j++){
			
			if(i >= row && i < row+8 && j >= col && j < col+8){             
				subIm.at<Vec3b>(i, j)[2] = RGB[i-row][j-col][0];
				subIm.at<Vec3b>(i, j)[1] = RGB[i-row][j-col][1];
				subIm.at<Vec3b>(i, j)[0] = RGB[i-row][j-col][2];
				continue;
			}

			subIm.at<Vec3b>(i, j)[2] = 0;
			subIm.at<Vec3b>(i, j)[1] = 0;
			subIm.at<Vec3b>(i, j)[0] = 0;
			
		}
	}
	*/

}

//fills the RGB color space vector for the SubImage, 8x8x3 3D vector
void SubImage::fillRGB(){
		//temporary variables to make assignments easier
        int r, g, b;
        vector< vector<int> > tmpRGB;
        vector<int> innerTmpRGB;
        
		//loop through all 64 pixels in the 8x8 subimage and snag the RGBs
        for(int i = row; i < row + 8; i++){
                for(int j = col; j < col + 8; j++){
                        r = subIm.at<Vec3b>(i, j)[2];
                        g = subIm.at<Vec3b>(i, j)[1];
                        b = subIm.at<Vec3b>(i, j)[0];

                        innerTmpRGB.push_back(r);
                        innerTmpRGB.push_back(g);
                        innerTmpRGB.push_back(b);
                        
                        tmpRGB.push_back(innerTmpRGB);
                        innerTmpRGB.clear();
                }
				//push_back into the 3D vector for use later
                RGB.push_back(tmpRGB);
                tmpRGB.clear();
        }
}

//fills the YCbCr color space vector for the SubImage, 8x8x3 3D vector
void SubImage::fillYCbCr(){
		//temporary variables to make assignments easier
        int r, g, b;
        vector< vector<int> > tmpYCbCr;
        vector<int> innerTmpYCbCr;
        
		//loop through all 64 pixels in the 8x8 subimage
		//get all of the RGB data for each pixel first
		//then convert to the YCbCr color space
        for(int i = row; i < row + 8; i++){
                for(int j = col; j < col + 8; j++){
                        r = subIm.at<Vec3b>(i, j)[2];
                        g = subIm.at<Vec3b>(i, j)[1];
                        b = subIm.at<Vec3b>(i, j)[0];

						//linear matrix transformation to convert from the RGB
						//color space to the YCbCr colorspace
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

//Following are a bunch of accessor functions
// thier names are self explanatory just returning the 
// specified data member

Mat SubImage::getSubImage(){
        return subIm;
}

int SubImage::getRow(){
        return row;
}

int SubImage::getCol(){
        return col;
}

vector< vector< vector<int> > > SubImage::getRGB(){
        return RGB;
}

//this overloaded accessor just makes it easier to access
//individual RGB data
int SubImage::getRGB(int i, int j, int n){
        return RGB[i][j][n];
}

vector< vector< vector<int> > > SubImage::getYCbCr(){
        return YCbCr;
}


