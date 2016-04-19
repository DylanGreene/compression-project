/*
    Dylan Greene, image.cpp
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
		SubImage(); //default constructor
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
		Image(); //default constuctor
		Image(Mat im); //non default constructor which takes an OpenCV Mat
		
		void displayFilter(int n); //displays y, Cb or Cr based on the int passed (0, 1 or 2)
		Mat getImage(); //returns the OpenCV Mat of the image

	private:
		Mat image; //the OpenCV Mat 
		int width;
		int height;
		vector< vector<SubImage> > subIms;
		CompressedImage compIm;

		void divideImage(); //divided image into 8x8 sub images
		void getRGB();
		void getYCbCr();
		CompressedImage compress(vector< vector<SubImage> > subIms);
};

//************************************************************************************************//
//Image Class Implementation//
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
