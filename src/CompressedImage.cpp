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
#include <cmath>

#include "Image.h"
#include "SubImage.h"
#include "CompressedImage.h"

using namespace cv; //OpenCV namespace
using namespace std;


//Constructor
CompressedImage::CompressedImage(){}
CompressedImage::CompressedImage(Image im){
	//initialize the quality
	q = 25;

	//set the data members based on the input Image object
	image = im;
	subIms = image.getSubIms();
	compressedSubIms = subIms;
	compressedIm = image.getImage().clone();

	//fill the initial rgb data 
	fillRGB();
	fillYCbCr();
	
	//run the actual compression algorithms
	compressIm();
}

//the compress function which calls on the compression algrorithm and then recombines the subImages
void CompressedImage::compressIm(){
	//actually compress all of the SubImages
	compressSubIms();

	//loop through all the pixels of our final image Mat and put the compressed SubImage data into it
	for(int i = 0; i < compressedIm.rows; i+=8){
		for(int j = 0; j < compressedIm.cols; j+=8){
			
			for(int k = 0; k < 8; k++){
				for(int l = 0; l < 8; l++){     
					
					//set all of the rgb data for each pixel from each subimage
					RGB[i+k][j+l][0] = compressedSubIms[i/8][j/8].getRGB(k, l, 0);
					RGB[i+k][j+l][1] = compressedSubIms[i/8][j/8].getRGB(k, l, 1);
					RGB[i+k][j+l][2] = compressedSubIms[i/8][j/8].getRGB(k, l, 2);
						
				}
			}

		}
	}
	
	compressSubIms();
	writeRGB();

}

//calls the compression algorithms for each of the sub images
void CompressedImage::compressSubIms(){
	//loop through the subImages, downspample, and then do the DCT
	for(int i = 0; i < subIms.size(); i++){
		for(int j = 0; j < subIms[0].size(); j++){
			compressYCbCrAverages(subIms[i][j]);
			//compressDiscreteCosine(compressedSubIms[i][j]);
		}
	}
}

//this is the downsampling part of the algorithm
void CompressedImage::compressYCbCrAverages(SubImage si){
	//temporary varibales to help store intermediate values
	int averageCb = 0;
	int averageCr = 0;
	int Y, Cb, Cr;

	//looping through the pixels in the subImage and computing average
	//luminance and chrominance values over 2x2 blocks
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

			// we can then average the differences over those 2x2 blocks
			// this should help the DCT be more effective while not also being very noticible
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

//the main compression algorithm of performing a Discrete Cosine Transform
//and then quantizing that matrix with 
void CompressedImage::compressDiscreteCosine(SubImage & csi){

	// Copy the values from SubImage into 3d vector
	int Y, Cb, Cr;
	vector< vector< vector<int> > > compressedYCbCr;
	vector< vector<int> > tmpYCbCr;
	vector<int> innerTmpYCbCr;
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			Y = csi.getYCbCr(i, j, 0);
			Cb = csi.getYCbCr(i, j, 1);
			Cr = csi.getYCbCr(i, j, 2);

			innerTmpYCbCr.push_back(Y);
			innerTmpYCbCr.push_back(Cb);
			innerTmpYCbCr.push_back(Cr);

			tmpYCbCr.push_back(innerTmpYCbCr);
			innerTmpYCbCr.clear();
		}
		compressedYCbCr.push_back(tmpYCbCr);
		tmpYCbCr.clear();
	} 


	//the arrays will contain the DCT data for the subimage
	double G_Y[8][8] = {0};
	double G_Cb[8][8] = {0};
	double G_Cr[8][8] = {0};
	
	//perform the DCT
	double alpha_u = 1, alpha_v = 1;
	double F = 0;
	double cos1, cos2;
	for(int i = 0; i < 3; i++){		
		for(int u = 0; u < 8; u++){
			for(int v = 0; v < 8; v++){
				if(u == 0) alpha_u = 1/sqrt(2);
				if(v == 0) alpha_v = 1/sqrt(2);
				for(int x = 0; x < 8; x++){
					for(int y = 0; y < 8; y++){
						cos1 = cos(((2*x + 1)*M_PI*u)/16);
						cos2 = cos(((2*y + 1)*M_PI*v)/16);
						F += (compressedYCbCr[x][y][i] - 128)*cos1*cos2;
					}
				}
				F = F * (1/4) * alpha_u * alpha_v;
				
				if(i == 0){
					G_Y[u][v] = F;
				}else if(i == 1){
					G_Cb[u][v] = F;
				}else if(i == 2){
					G_Cr[u][v] = F;
				}

				F = 0;
				alpha_u = 1;
				alpha_v = 1;
			}
		}
	}


	//These arrays are the standard jgi matrices for quantization
	int Standard_Y[8][8] = 
	{
		{16,11,10,16,24,40,51,61},
		{12,12,14,19,26,58,60,55},
		{14,13,16,24,40,57,69,56},
		{14,17,22,29,51,87,80,62},
		{18,22,37,56,68,109,103,77},
		{24,35,55,64,81,104,113,92},
		{49,64,78,87,103,121,120,101},
		{72,92,95,98,112,100,103,99}
	};
	
	int Standard_C[8][8] = 
	{
		{17,18,24,47,99,99,99,99},
		{18,21,26,66,99,99,99,99},
		{24,26,56,99,99,99,99,99},
		{47,66,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99}		
	};
	

	//these will hold the modified standard matrices based on the quality paramter q
	double Q_Y[8][8] = {0};
	double Q_C[8][8] = {0};


	//determine a factor to use when calculating the quantization tables also based on q
	double S;
	if(q < 50){
		S = 5000/q;
	}else{
		S = 200 - (2*q);
	}
	
	//make the Q_Y and the Q_C
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			Q_Y[i][j] = floor((S*Standard_Y[i][j] + 50)/100);
			if(Q_Y[i][j] == 0){
				Q_Y[i][j] = 1;
			}

			Q_C[i][j] = floor((S*Standard_C[i][j] + 50)/100);
			if(Q_C[i][j] == 0){
				Q_C[i][j] = 1;
			}
		}
	}


	//these will hold the final quantized image data for the entire subimage
	double B_Y[8][8] = {0};
	double B_Cb[8][8] = {0};
	double B_Cr[8][8] = {0};

	//actually calculate and quantize the YCbCr data
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			B_Y[i][j] = int((G_Y[i][j])/(Q_Y[i][j]));
			B_Cb[i][j] = int((G_Cb[i][j])/(Q_C[i][j]));
			B_Cr[i][j] = int((G_Cr[i][j])/(Q_C[i][j]));
		}
	}


	// Inverse DCT
	// loop through Y, Cb, Cr
	for(int i = 0; i < 3; i++){
		// loop through subImage pixels
		for(int x = 0; x < 8; x++){
			for(int y = 0; y < 8; y++){
				compressedYCbCr[x][y][i] = 0;
				for(int u = 0; u < 8; u++){
					for(int v = 0; v < 8; v++){
						if(u == 0) alpha_u = 1/sqrt(2);
						else alpha_u = 1;
						if(v == 0) alpha_v = 1/sqrt(2);
						else alpha_v = 1;
						cos1 = cos(((2*x + 1)*M_PI*u)/16);
						cos2 = cos(((2*y + 1)*M_PI*v)/16);

						// Calculate the Inverse Discrete Cosine Transform
						if(i == 0){
							compressedYCbCr[x][y][0] += alpha_u * alpha_v * B_Y[u][v] * cos1 * cos2;
						}else if(i == 1){
							compressedYCbCr[x][y][1] += alpha_u * alpha_v * B_Cb[u][v] * cos1 * cos2;
						}else if(i == 2){
							compressedYCbCr[x][y][2] += alpha_u * alpha_v * B_Cr[u][v] * cos1 * cos2;
						}
					}
				}

				// set the YCbCr values
				if(i == 0){
					csi.setYCbCr(x, y, 0, (compressedYCbCr[x][y][0] / 4) + 128);
				}else if(i == 1){
					csi.setYCbCr(x, y, 1, (compressedYCbCr[x][y][1] / 4) + 128);
				}else if(i == 2){
					csi.setYCbCr(x, y, 2, (compressedYCbCr[x][y][2] / 4) + 128);
				}
			}
		}
	}

	int r, g, b;
	//Set subImage RGB
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			r = csi.getYCbCr(i, j, 0) + 1.402*(csi.getYCbCr(i, j, 2));
			g = csi.getYCbCr(i, j, 0) - 0.34414*(csi.getYCbCr(i, j, 1)) - 0.71414*(csi.getYCbCr(i, j, 2));
			b = csi.getYCbCr(i, j, 0) + 1.772*(csi.getYCbCr(i, j, 1));
				 
			csi.setRGB(i, j, 0, r);
			csi.setRGB(i, j, 1, g);
			csi.setRGB(i, j, 2, b);
		}
	}
}

//displays a filter or the original based on the int passed (0-6)
//0: orginal 1: r, 2: g, 3: b, 4: Y, 5: Cb, 6: Cr
void CompressedImage::displayFilter(int n){
	Mat tmp = compressedIm.clone();
	for(int i = 0; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			//switch between the options and calculate the filter
			switch(n){
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
	
	//switch again, this time actually just display the image
	switch(n){
		case 0:
			imshow("Compressed Image", tmp);
			break;
		case 1: 
			imshow("Compressed Red Channel", tmp);
			break;
		case 2:
			imshow("Compressed Green Channel", tmp);
			break;
		case 3:
			imshow("Compressed Blue Channel", tmp);
			break;
		case 4:
			imshow("Compressed Luminance", tmp);
			break;
		case 5:
			imshow("Compressed Blue Chrominance", tmp);
			break;
		case 6:
			imshow("Compressed Red Chrominance", tmp);
			break;
	}
}

//
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

void CompressedImage::writeRGB(){
	for(int i = 0; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			compressedIm.at<Vec3b>(i, j)[2] = RGB[i][j][0];
			compressedIm.at<Vec3b>(i, j)[1] = RGB[i][j][1];
			compressedIm.at<Vec3b>(i, j)[0] = RGB[i][j][2];
		}	
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

//saves a filter of the image based on the int passed (0-6)
//0: Actual, 1: r, 2: g, 3: b, 4: Y, 5: Cb, 6: Cr
void CompressedImage::saveFilter(int n){
	cout << "Enter the file path you wish to save this filter as: ";
	string pathToNewFile = "";
	cin >> pathToNewFile;

	//clone the image into a temp Mat to edit it to display filters
	Mat tmp = compressedIm.clone();

	//loop through all of the pixels of the temp Mat and adjust the
	//color spaces to the appropriate one
	for(int i = 0; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			
			if(n == 0){
				tmp.at<Vec3b>(i, j)[2] = RGB[i][j][0];
				tmp.at<Vec3b>(i, j)[1] = RGB[i][j][1];
				tmp.at<Vec3b>(i, j)[0] = RGB[i][j][2];
			}else if(n == 1){	
				tmp.at<Vec3b>(i, j)[2] = RGB[i][j][0];
				tmp.at<Vec3b>(i, j)[1] = 0;
				tmp.at<Vec3b>(i, j)[0] = 0;
			}else if(n == 2){
				tmp.at<Vec3b>(i, j)[2] = 0;
				tmp.at<Vec3b>(i, j)[1] = RGB[i][j][1];
				tmp.at<Vec3b>(i, j)[0] = 0;
			}else if(n == 3){
				tmp.at<Vec3b>(i, j)[2] = 0;
				tmp.at<Vec3b>(i, j)[1] = 0;
				tmp.at<Vec3b>(i, j)[0] = RGB[i][j][2];
			}else if(n == 4){
				tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][0]*0.2362797506;
				tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][0]*0.2362797506;
				tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][0]*0.2362797506;
			}else if(n == 5){
				tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][1]*-0.00000169;
				tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][1]*-0.08131169;
				tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][1]*0.41868831;
			}else if(n == 6){
				tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][2]*0.33126364;
				tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][2]*-0.16873636;
				tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][2]*0.0000037;
			}
		}
	}
	
	//write the image file
	imwrite(pathToNewFile, tmp);

	cout << "The image has been written to: " << pathToNewFile << endl;
}

//quality mutator
void CompressedImage::setQuality(int n){
	if(n < 1){
		q = 1;
	}else if(n > 99){
		q = 99;
	}else{
		q = n;
	}
}

