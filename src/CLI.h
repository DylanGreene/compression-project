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
		CLI();
		CLI(string ip);

		void run();

	private:
		string imagePath;

		Image image;
		CompressedImage compressed;

		void menu();
		void prompt();
		void filterPrompt(bool isCompressed);
		void writePrompt();

};


#endif

