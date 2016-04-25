/*
    Dylan Greene and Kyle Gifaldi, image.cpp
*/

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

using namespace cv; //OpenCV namespace
using namespace std;

//*************************************************************************************************//
//SubImage Class Definition//
//*************************************************************************************************//

class SubImage{ 
	public:
		SubImage(Mat im, int row, int col); //non-default constructor
		
		//accessors 
		Mat getSubImage();
		int getRow();
		int getCol();
		vector< vector< vector<int> > > getRGB();
		vector< vector< vector<int> > > getYCbCr();

	private:
		Mat subIm; //the sub image
		
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		
		//position in parent image
		int row;
		int col;

		//member helper functions
		void fillRGB();
		void fillYCbCr();
};

//*************************************************************************************************//
//Image Class Defintion//
//*************************************************************************************************//

//Composed of SubImage(s)
class Image{ 
	public:
		Image();
		Image(string fp); //non default constructor which takes an OpenCV Mat
		
		void displayFilter(int n); 

		//accessors
		Mat getImage(); //returns the OpenCV Mat of the image
		vector< vector<SubImage> > getSubIms();
		vector< vector< vector<int> > > getRGB(); //returns 3D vector of RGB data
		vector< vector< vector<int> > > getYCbCr(); //returns 3D vector of YCbCr data

	private:
		string imagePath;
		Mat image; //the OpenCV Mat 
		int width;
		int height;
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		vector< vector<SubImage> > subIms;

		void divideImage(); //divided image into 8x8 sub images
		
	protected:
		void fillRGB();
		void fillYCbCr();
};

//*************************************************************************************************//
//CompressedImage Class Definition//
//*************************************************************************************************//

//inherits from Image
class CompressedImage: public Image{
	public:
		CompressedImage(Image im);

	private:
		Image image;
		vector< vector<SubImage> > subIms;
		vector< vector<SubImage> > compressedSubIms;
		Mat compressedIm;
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		void compressYCbCrAverages(SubImage si);
		void compressDiscreteCosine(SubImage csi);
		
		void compressSubIms();
		void compressIm();
};
//************************************************************************************************//
//Image Class Implementation//
//************************************************************************************************//

//constructor
Image::Image(){}
Image::Image(string fp){
	imagePath = fp;
	image = imread(imagePath, CV_LOAD_IMAGE_COLOR); //read the image into a Mat	
    /*
        imread second argument options for future reference:
        CV_LOAD_IMAGE_UNCHANGED (<0) loads the image as is (including the alpha channel if present)
        CV_LOAD_IMAGE_GRAYSCALE ( 0) loads the image as an intensity one
        CV_LOAD_IMAGE_COLOR (>0) loads the image in the BGR format
    */

    if(!image.data ){ //checking that image data was correctly loaded
        cout << "Could not open or find the image" << endl ;
        exit(-1);
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

void Image::divideImage(){
	for(int i = 0; i < height/8; i++){
		vector<SubImage> tmp;
		for(int j = 0; j < width/8; j++){
			SubImage tmpSubImage(image, i, j);
			tmp.push_back(tmpSubImage);
		}
		subIms.push_back(tmp);
		tmp.clear();
	}
}

void Image::fillRGB(){
	int r, g, b;
	vector< vector<int> > tmpRGB;
	vector<int> innerTmpRGB;
	
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

void Image::fillYCbCr(){
	int r, g, b;
	vector< vector<int> > tmpYCbCr;
	vector<int> innerTmpYCbCr;
	
	for(int i = 0 ; i < height; i++){
		for(int j = 0; j < width; j++){
			r = image.at<Vec3b>(i, j)[2];
			g = image.at<Vec3b>(i, j)[1];
			b = image.at<Vec3b>(i, j)[0];

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

//displays y, Cb or Cr based on the int passed (0-5)
//0: r, 1: g, 2: b, 3: Y, 4: Cb, 5: Cr
void Image::displayFilter(int n){
	Mat tmp = image;
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


//************************************************************************************************//
//Compressed Image Class Implementation//
//************************************************************************************************//

//inherits from Image
CompressedImage::CompressedImage(Image im){
	image = im;
	subIms = image.getSubIms();
	compressedSubIms = subIms;
	compressedIm = image.getImage();

	//fillRGB();
	//fillYCbCr();

	//compressIm();

}

void CompressedImage::compressIm(){
	compressSubIms();
	
	for(int i = 0; i < compressedIm.rows; i++){
		for(int j = 0; j < compressedIm.cols; j++){
			
			SubImage tmp = subIms[i][j];
		
			for(int k = 0; k < 8; k++){
				for(int l = 0; l < 8; l++){
					RGB = tmp.getRGB();
					compressedIm.at<Vec3b>(i+k, j+l)[2] = RGB[k][l][0];
					compressedIm.at<Vec3b>(i+k, j+l)[1] = RGB[k][l][1];
					compressedIm.at<Vec3b>(i+k, j+l)[0] = RGB[k][l][2];
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

	for(int i = si.getRow(); i < si.getRow() + 4; i+=2){
		for(int j = si.getCol(); j < si.getCol() + 4; j+=2){
			for(int k = 0; k < 2; k++){
				for(int l = 0; l < 2; l++){
					averageCb += YCbCr[i+k][j+l][1];
					averageCr += YCbCr[i+k][j+l][2];
				}
			}
			averageCb /= 4;
			averageCr /= 4;
			
			for(int k = 0; k < 2; k++){
				for(int l = 0; l < 2; l++){
					YCbCr[i+k][j+l][1] = averageCb;
					YCbCr[i+k][j+l][2] = averageCr;

					Y = YCbCr[i+k][j+l][0]; 
					Cb = YCbCr[i+k][j+l][1]; 
					Cr = YCbCr[i+k][j+l][2]; 
					
					RGB[i+k][j+l][0] = 0.2362797506*Y - 0.00000169*Cb + 0.33126364*Cr;
					RGB[i+k][j+l][1] = 0.2362797506*Y - 0.08131169*Cb - 0.16873636*Cr;
					RGB[i+k][j+l][2] = 0.2362797506*Y + 0.41868831*Cb + 0.00000364*Cr;
				}
			}			
		}
	}

}

void CompressedImage::compressDiscreteCosine(SubImage csi){
	
}


//************************************************************************************************//
//SubImage Class Implementation//
//************************************************************************************************//

SubImage::SubImage(Mat im, int r, int c){
	subIm = im;
	row = r;
	col = c;

	fillRGB();
	fillYCbCr();
}

void SubImage::fillRGB(){
	int r, g, b;
	vector< vector<int> > tmpRGB;
	vector<int> innerTmpRGB;
	
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
		RGB.push_back(tmpRGB);
		tmpRGB.clear();
	}
}

void SubImage::fillYCbCr(){
	int r, g, b;
	vector< vector<int> > tmpYCbCr;
	vector<int> innerTmpYCbCr;
	
	for(int i = row; i < row + 8; i++){
		for(int j = col; j < col + 8; j++){
			r = subIm.at<Vec3b>(i, j)[2];
			g = subIm.at<Vec3b>(i, j)[1];
			b = subIm.at<Vec3b>(i, j)[0];

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

vector< vector< vector<int> > > SubImage::getYCbCr(){
	return YCbCr;
}

//************************************************************************************************//
//Main//
//************************************************************************************************//


int main(int argc, char** argv){

    if(argc != 2){ //make sure valid usage
     cout << " Usage: ./readAndDisp ImageToLoadAndDisplay" << endl;
     return -1;
    }
	string imPath = argv[1];

	Image im(imPath);
	im.displayFilter(0);
	/*
	im.displayFilter(1);
	im.displayFilter(2);
	im.displayFilter(3);
	im.displayFilter(4);
	im.displayFilter(5);
	im.displayFilter(6);
	*/

	CompressedImage compressedIm(im);
	compressedIm.displayFilter(0);


	waitKey(0); //wait for a keystroke in the window (parameter is how long it should wait in milli: 0 is forever)
    return 0;
}
