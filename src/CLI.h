/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * CLI.h
 * 		Class definition for the Command line interface (CLI) class
 *
 * 		Runs our command line interface and interfaces with the Image and 
 * 		CompressedImage objects
 *
 */

#ifndef CLI_H
#define CLI_H

#include <iostream>
#include <vector>
#include <string>

#include "Image.h"
#include "CompressedImage.h"

using namespace std;


class CLI{ 
	public:
		CLI(); //default constructor 
		CLI(string ip); //non default constructor, takes an image path

		void run(); //runs the interface

	private:
		string imagePath; //path the the image being compressed

		Image image; //image object
		CompressedImage compressed; //compressed iamge object

		//helper functions
		void menu(); //prints the menu
		void prompt(); //prompts the user to select something from the menu
		void filterPrompt(bool isCompressed); //prompts the user to select what filter they want
		void writePrompt(); //prompts the user to select where and what image they want to save

};


#endif

