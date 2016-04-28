/*
 * Dylan Greene
 * Kyle Gifaldi
 *
 * CLI.cpp
 * 		Implenation of our command line interface class
 *
 * 		Runs a loop for our command line interface and bascially is in charge
 * 		of managing and interacting with our Image and CompressedImage objects
 * 		
 */

#include <iostream>
#include <vector>
#include <string>

#include "CLI.h"
#include "Image.h"
#include "CompressedImage.h"

using namespace std;

CLI::CLI(){
	
	cout << "Enter an image path: " << endl;
	cin >> imagePath;

	Image tmpIm(imagePath);
	image = tmpIm;

	CompressedImage tmpCIm(image);
	compressed = tmpCIm;

	run();
	
}

CLI::CLI(string ip){

	imagePath = ip;

	Image tmpIm(imagePath);
	image = tmpIm;

	CompressedImage tmpCIm(image);
	compressed = tmpCIm;

	run();
}

void CLI::run(){
	
	cout << "########################################################" << endl;
	cout << "                Welcome to the IMAGEn                   " << endl;
	cout << "########################################################" << endl;
	
	//execute the main interface loop which prompts the user and acts on that
	while(true){
		menu();
		prompt();
		waitKey(0);	
	}
}

void CLI::menu(){
	//display compressed image
	//dispaly a filter of original image
	//display a filter of the compressed imgae
	//write any of those those to a file
	
	cout << endl << "Options: " << endl;
	cout << "\t0: Display the orginal image" << endl;
	cout << "\t1: Display a filter of the original image" << endl;
	cout << "\t2: Display the compressed image" << endl;
	cout << "\t3: Display a filter of the compressed image" << endl;
	cout << "\t4: Write an image or filter to a file" << endl;
	cout << "\tQ: Quit the program" << endl;

}

void CLI::prompt(){
	string choice = "";
	bool done = false;
	while(!done){
		done = true;
		cout << endl << "\t\tChoose an option: ";
		cin >> choice;
		if(choice == "0"){
			image.displayFilter(0);
		}else if(choice == "1"){
			cout << "\t\tOrginal Image filter display options: " << endl;
			filterPrompt(false);
		}else if(choice == "2"){
			compressed.displayFilter(0);
		}else if(choice == "3"){
			cout << "\t\tCompressed Image filter display options: " << endl;
			filterPrompt(true);
		}else if(choice == "4"){
			cout << "\t\tWhich image would you like to write?: " << endl;
			writePrompt();
		}else if(choice == "Q"){
			exit(0);
		}else{
			cout << "Not a valid option!" << endl;
			menu();
			done = false;
		}
	}
}

void CLI::writePrompt(){
	string choice = "";
	bool isCompressed = false;
	do{
		cout << "\t\t\tWould you like ORGINAL(1) or COMPRESSED(2) image options (1 or 2): ";
		cin >> choice;
		if(choice != "1" && choice != "1"){
			cout << "Invalid option" << endl;
			continue;
		}else if(choice == "2"){
			isCompressed = true;
		}
		break;
	}while(true);

	bool done = false;
	while(!done){

		if(isCompressed){
			cout << "\t\t\tChoose an option for the compressed Image:" << endl;
		}else{
			cout << "\t\t\tChoose an option for the orginal Image:" << endl;
		}

		cout << "\t\t\t\tOptions:									0: Image" << endl;
		cout << "\t\t\t\t1: Red Channel		2: Green Channel   		3: Blue Channel" << endl;
		cout << "\t\t\t\t4: Luminance  		5: Blue Chrominance		6: Red Chrominance" << endl;

		done = true;
		cout << endl << "Choice: ";
		cin >> choice;
		
		if(choice == "0"){
			(isCompressed) ? compressed.saveFilter(0) : image.saveFilter(0);
		}else if(choice == "1"){
			(isCompressed) ? compressed.saveFilter(1) : image.saveFilter(1);
		}else if(choice == "2"){
			(isCompressed) ? compressed.saveFilter(2) : image.saveFilter(2);
		}else if(choice == "3"){
			(isCompressed) ? compressed.saveFilter(3) : image.saveFilter(3);
		}else if(choice == "4"){
			(isCompressed) ? compressed.saveFilter(4) : image.saveFilter(4);
		}else if(choice == "5"){
			(isCompressed) ? compressed.saveFilter(5) : image.saveFilter(5);
		}else if(choice == "6"){
			(isCompressed) ? compressed.saveFilter(6) : image.saveFilter(6);
		}else{
			cout << "Invalid option!" << endl;
			done = false;
		}
	}
	
}


void CLI::filterPrompt(bool isCompressed){
	string choice = "";
	bool done = false;
	while(!done){
		cout << "\t\t\t1: Red Channel		2: Green Channel   		3: Blue Channel" << endl;
		cout << "\t\t\t4: Luminance  		5: Blue Chrominance		6: Red Chrominance" << endl;

		done = true;
		cout << endl << "Choice: ";
		cin >> choice;
			
		if(choice == "0"){
			(isCompressed) ? compressed.displayFilter(0) : image.displayFilter(0);
		}else if(choice == "1"){
			(isCompressed) ? compressed.displayFilter(1) : image.displayFilter(1);
		}else if(choice == "2"){
			(isCompressed) ? compressed.displayFilter(2) : image.displayFilter(2);
		}else if(choice == "3"){
			(isCompressed) ? compressed.displayFilter(3) : image.displayFilter(3);
		}else if(choice == "4"){
			(isCompressed) ? compressed.displayFilter(4) : image.displayFilter(4);
		}else if(choice == "5"){
			(isCompressed) ? compressed.displayFilter(5) : image.displayFilter(5);
		}else if(choice == "6"){
			(isCompressed) ? compressed.displayFilter(6) : image.displayFilter(6);
		}else{
			cout << "Invalid option!" << endl;
			done = false;
		}
				
	}
}

