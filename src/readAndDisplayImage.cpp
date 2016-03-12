/*
    Dylan Greene, readAndDisplayImage.cpp

    Read in an image from the command line and display it using the opencv highgui module
    Made to have some practice with OpenCV and reading/displaying images
    Procedure for process aquired from the OpenCV doc
*/

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>

using namespace cv; //OpenCV namespace
using namespace std;

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
