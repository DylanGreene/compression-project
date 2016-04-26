/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * CompressedImage.cpp
 * 		Class implementation for the CompressedImage class defined in CompressedImage.h
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

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

using namespace cv; //OpenCV namespace
using namespace std;


//Constructor
CompressedImage::CompressedImage(Image im){
	image = im;
	subIms = image.getSubIms();
	compressedSubIms = subIms;
	compressedIm = image.getImage().clone();

	fillRGB();
	fillYCbCr();
	
	compressIm();

	fillRGB();
	fillYCbCr();

}

void CompressedImage::compressIm(){
	compressSubIms();

	for(int i = 0; i < compressedIm.rows / 8; i++){
		for(int j = 0; j < compressedIm.cols / 8; j++){
			
			for(int k = 0; k < 8; k++){
				for(int l = 0; l < 8; l++){     

					compressedIm.at<Vec3b>(i+k, j+l)[2] = subIms[i][j].getRGB(k, l, 0);
					compressedIm.at<Vec3b>(i+k, j+l)[1] = subIms[i][j].getRGB(k, l, 1);
					compressedIm.at<Vec3b>(i+k, j+l)[0] = subIms[i][j].getRGB(k, l, 2);
						
				}
			}

		}
	}
}

void CompressedImage::compressSubIms(){
	for(int i = 0; i < subIms.size(); i++){
		for(int j = 0; j < subIms[0].size(); j++){
			compressYCbCrAverages(subIms[i][j]);
			compressDiscreteCosine(compressedSubIms[i][j]);
		}
	}
}

void CompressedImage::compressYCbCrAverages(SubImage si){
	int averageCb = 0;
	int averageCr = 0;
	int Y, Cb, Cr;

	for(int i = si.getRow(); i < si.getRow() + 4; i++){
		for(int j = si.getCol(); j < si.getCol() + 4; j++){
			for(int k = 0; k < 4; k++){
				for(int l = 0; l < 4; l++){
						averageCb += YCbCr[i+k][j+l][1];
						averageCr += YCbCr[i+k][j+l][2];
				}
			}
			averageCb /= 16;
			averageCr /= 16;

			for(int k = 0; k < 4; k++){
				for(int l = 0; l < 4; l++){
					YCbCr[i+k][j+l][1] = averageCb;
					YCbCr[i+k][j+l][2] = averageCr;

					Y = YCbCr[i+k][j+l][0]; 
					Cb = YCbCr[i+k][j+l][1]; 
					Cr = YCbCr[i+k][j+l][2]; 
					
					RGB[i+k][j+l][0] = Y + 1.402*(Cr);
					RGB[i+k][j+l][1] = Y - 0.34414*(Cb) - 0.71414*(Cr);
					RGB[i+k][j+l][2] = Y + 1.772*(Cb);
				}
			}

			averageCb = 0;
			averageCr = 0;
			
		}
	}

}

//displays a filter or the original based on the int passed (0-6)
//0: orginal 1: r, 2: g, 3: b, 4: Y, 5: Cb, 6: Cr
void CompressedImage::displayFilter(int n){
	Mat tmp = compressedIm.clone();
	for(int i = 0; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
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

	switch(n){
		case 0:
			imshow("Compressed Image", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 1: 
			imshow("Compressed Red Channel", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 2:
			imshow("Compressed Green Channel", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 3:
			imshow("Compressed Blue Channel", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 4:
			imshow("Compressed Luminance", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 5:
			imshow("Compressed Blue Chrominance", tmp);
			imwrite("result.jpg", tmp);
			break;
		case 6:
			imshow("Compressed Red Chrominance", tmp);
			imwrite("result.jpg", tmp);
			break;
	}
}

void CompressedImage::fillRGB(){
	int r, g, b;
	vector< vector<int> > tmpRGB;
	vector<int> innerTmpRGB;
	
	for(int i = 0 ; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			r = compressedIm.at<Vec3b>(i, j)[2];
			g = compressedIm.at<Vec3b>(i, j)[1];
			b = compressedIm.at<Vec3b>(i, j)[0];

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

void CompressedImage::fillYCbCr(){
	int r, g, b;
	vector< vector<int> > tmpYCbCr;
	vector<int> innerTmpYCbCr;
	
	for(int i = 0 ; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			r = compressedIm.at<Vec3b>(i, j)[2];
			g = compressedIm.at<Vec3b>(i, j)[1];
			b = compressedIm.at<Vec3b>(i, j)[0];

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

