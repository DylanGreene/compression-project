/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * main.cpp
 *
 * 		Driver for our project
 * 		Implements a command line interface based on the various
 * 		image related classes we have created
 *
 */

#include <opencv2/core/core.hpp> //core section: basic building blocks of libriary
#include <opencv2/highgui/highgui.hpp> //contains functions for input and output
#include <iostream>
#include <vector>

#include "Image.h"
#include "CompressedImage.h"

using namespace cv; //OpenCV namespace
using namespace std;
 

int main(int argc, char** argv){
	
	string imPath;

    if(argc > 2){ //make sure valid usage
     cout << " Usage: ./compress_image [IMAGE]" << endl;
     return -1;
    }else if(argc == 2){
		imPath = argv[1];
	}

	CLI interface(imPath ? imPath : );
	interface.run();

	waitKey(0); //wait for a keystroke in the window (parameter is how long it should wait in milli: 0 is forever)
    return 0;
}

