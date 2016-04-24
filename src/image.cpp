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
		int getHeight();
		int getWidth();

	private:
		Mat subIm; //the sub image
		
		vector< vector< vector<int> > > RGB;
		vector< vector< vector<int> > > YCbCr;
		
		//position in parent image
		int row;
		int col;

		//dims of sub image
		int width;
		int height;
};

//*************************************************************************************************//
//Compressed SubImage Definition//
//*************************************************************************************************//

//Inherits from SubImage (Compressed SubImage IS a SubImage
class CompressedSubImage: public SubImage{
	public:
		CompressedSubImage(); //default contructor
		CompressedSubImage();

	private:
		vector< vector< vector<int> > > compressedRGB;
		vector< vector< vector<int> > > compressedYCbCr;
		Mat compressedSubIm;
};

//*************************************************************************************************//
//CompressedImage Class Definition//
//*************************************************************************************************//

//Composed of CompressedSubImage(s)
class CompressedImage{
	public:
		CompressedImage();
		CompressedImage(vector< vector<SubImage> > subIms);

	private:
		SubImage subIms;
		vector< vector<CompressedSubImage> >;
		Mat compressedIm;

		vector< vector<CompressedSubImage> > compressSubIms()
};

//*************************************************************************************************//
//Image Class Defintion//
//*************************************************************************************************//

//Composed of SubImage(s)
class Image{ 
	public:
		Image(string fp); //non default constructor which takes an OpenCV Mat
		
		void displayFilter(int n); //displays y, Cb or Cr based on the int passed (0, 1 or 2)

		//accessors
		Mat getImage(); //returns the OpenCV Mat of the image
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
		void fillRGB();
		void fillYCbCr();
};

//************************************************************************************************//
//Image Class Implementation//
//************************************************************************************************//

//constructor
Image::Image(string fp){
	imagePath = fp;
	image = imread(imagePath, CV_LOAD_IMAGE_COLOR); //read the image into a Mat
	
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
	for(int i = 0; i < 8; i++){
		vector<SubImage> tmp;
		for(int j = 0; j < 8; j++){
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
		RGB.push_back(tempRGB);
		tempRGB.clear();
	}
}

void Image::fillYCbCr(){
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
		YCbCr.push_back(tempYCbCr);
		tempYCbCr.clear();
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
					tmp.at<Vec3b>(i, j)[1] = 0;
					tmp.at<Vec3b>(i, j)[0] = 0;
				case 1: 					
					tmp.at<Vec3b>(i, j)[2] = 0;
					tmp.at<Vec3b>(i, j)[1] = RGB[i][j][1];
					tmp.at<Vec3b>(i, j)[0] = 0;
				case 2: 
					tmp.at<Vec3b>(i, j)[2] = 0;
					tmp.at<Vec3b>(i, j)[1] = 0;
					tmp.at<Vec3b>(i, j)[0] = RGB[i][j][2];
				case 3:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][0]*0.2362797506;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][0]*0.2362797506;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][0]*0.2362797506;
				case 4:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][1]*-0.00000169;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][1]*-0.08131169;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][1]*0.41868831;
				case 5:
					tmp.at<Vec3b>(i, j)[2] = YCbCr[i][j][2]*0.33126364;
					tmp.at<Vec3b>(i, j)[1] = YCbCr[i][j][2]*-0.16873636;
					tmp.at<Vec3b>(i, j)[0] = YCbCr[i][j][2]*0.0000037;

			}
			switch(n){
				case 0: 
					imshow("Red Channel", tmp);
				case 1:
					imshow("Green Channel", tmp);
				case 2:
					imshow("Blue Channel", tmp);
				case 3:
					imshow("Luminance", tmp);
				case 4:
					imshow("Blue Chrominance", tmp);
				case 5:
					imshow("Red Chrominance", tmp);
			}
		}
	}
}

Mat Image::getImage(){
	return image;
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



//************************************************************************************************//
//SubImage Class Implementation//
//************************************************************************************************//



//************************************************************************************************//
//CompressedSubImage Class Implementation//
//************************************************************************************************//



//************************************************************************************************//
//Main//
//************************************************************************************************//


int main(int argc, char** argv){

    if(argc != 2){ //make sure valid usage
     cout << " Usage: ./readAndDisp ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image; //stores teh data of the loaded image
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR); //loads the image name in BGR format
    /*
        imread second argument options for future reference:
        CV_LOAD_IMAGE_UNCHANGED (<0) loads the image as is (including the alpha channel if present)
        CV_LOAD_IMAGE_GRAYSCALE ( 0) loads the image as an intensity one
        CV_LOAD_IMAGE_COLOR (>0) loads the image in the BGR format
    */

    if(!image.data ){ //checking that image data was correctly loaded
        cout << "Could not open or find the image" << endl ;
        return -1;
    }

    namedWindow("Display window", WINDOW_AUTOSIZE); //create a window for display
    imshow("Display window", image); //display the image in the window

    waitKey(0); //wait for a keystroke in the window (parameter is how long it should wait in milli: 0 is forever)
    return 0;
}
