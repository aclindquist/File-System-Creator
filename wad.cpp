#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include<string>
#include<vector>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "wad.h"
using namespace std;


Wad::Wad()
{
	//Descriptor root;
	//root.name = "/";
	///directory.push_back(root);    
}
Wad* Wad::loadWad(const std::string& path) 
{
	int num;
	int descriptorOff;
	int numOfLevels = 0;
	bool newLevel = false;
	string start = "_START";
	string end = "_END";
	string currDirec;          //current directory
	vector<string> files;      //files that belong to the current directory;
	bool active = false;
	bool Root = false;
	fstream test2;

	test2.open(path, ios::in | ios::binary);
	test2.seekg(ios::beg);
	unsigned int size = test2.tellg();

	if (test2.is_open()) 
	{
		char magic[5];
		test2.read(magic, 4);
		magic[4] = '\0';

		//cout << "magic: " << string(magic) << endl;

		test2.read((char*)&num, 4);
		//cout << "Number of Descriptors " << num << endl;
		test2.read((char*)&descriptorOff, 4);
		//cout << "Descriptor Offset " << descriptorOff << endl;

		static Wad* wad = new Wad();
		wad->magic = magic;
		wad->numOfDescriptors = num;
		test2.seekg(descriptorOff);                        //go to descriptors' location in file
		//int j = 0;

/**************************** Put elements in map object *********************************************/
		for (int i = 0; i < num; i++)
		{
			int Off;
			test2.read((char*)&Off, 4);
			int Len;
			test2.read((char*)&Len, 4);
			char nm[9];
			test2.read(nm, 8);
			nm[8] = '\0';
			string check = nm;

			if (nm[0] == 'E' && isdigit(check.at(1)) && nm[2] == 'M' && isdigit(check.at(3)) && check.length() == 4) {           //check for E#M#
				if (numOfLevels == 0){
					newLevel = true;
				}
				Descriptor descriptor;
				descriptor.name = nm;
				descriptor.length = Len;
				descriptor.offset = Off;
				string EM = nm;
 				wad->translator[nm] = descriptor;
				vector<string> contents;
				
				for (int j = 0; j < 10; j++)                 //insert 10 files into directory
				{
					int Off;
					int Len;
					char nm[9];
					test2.read((char*)&Off, 4);
					
					test2.read((char*)&Len, 4);
					
					test2.read(nm, 8);
					nm[8] = '\0';
    				Descriptor descriptor;
					descriptor.name = nm;
					descriptor.length = Len;
					descriptor.offset = Off;
					wad->translator[nm + ('_' + EM)] = descriptor;
					contents.push_back(nm);

				}
				wad->directories[nm] = contents;
				if (newLevel) {
					wad->levels[numOfLevels].push_back(nm);
				}
				if (numOfLevels == 0) {
					wad->directories["/"].push_back(EM);
				}
				else if (currDirec != "null") {
					wad->directories[currDirec].push_back(EM);
				}
				i += 10;
				continue;
			}

/********************************* Start *******************************************/

			else if (check.find(start) != string::npos)       //https://stackoverflow.com/questions/2340281/check-if-a-string-contains-a-string-in-c
			{
				string direcName = "";
				for (int k = 0; k < check.size(); k++) {
					if (check.at(k) != '_') {
						direcName += check.at(k);
					}
					else {
						break;
					}
				}

				Descriptor descriptor;
				descriptor.name = direcName;
				descriptor.length = Len;
				descriptor.offset = Off;
				//j++;

				if (numOfLevels == 0 || !active) {                      //added !active to check for files belonging to root, might mess things up
					newLevel = true;
					wad->directories["/"].push_back(direcName);

				}
				if (newLevel) {
					wad->levels[numOfLevels].push_back(direcName);
					newLevel = false;
					numOfLevels += 1;                                           //added
				}

				wad->translator[direcName] = descriptor;
				if (currDirec != "null") {
					if (active) {
						vector<string>complete;                    //vector to copy files 
						for (int l = 0; l < files.size(); l++) {
							complete[l] = files[l];
						}
						complete.push_back(direcName);
						wad->directories[currDirec] = complete;
						files.clear();
						

					}
					active = false;
					
				}
				currDirec = direcName;
					active = true;

			}
/********************************* End *******************************************/

			else if (check.find(end) != string::npos && check.find(end)+4==check.size())
			{
				newLevel = true;
				numOfLevels += 1;
				//j--;
				if (currDirec != "null") {
					vector<string>complete;                    //vector to copy files 
					for (int l = 0; l < files.size(); l++) {
						complete.push_back(files[l]);
					}
					for (int i = 0; i < complete.size(); i++) {
						wad->directories[currDirec].push_back(complete[i]);
					}
					complete.clear();
					files.clear();
					wad->translator[currDirec].isActive = false;
					currDirec = "null";
					active = false;                                                        //added this, might be cause of error
					
				}
			}

/********************************* File *******************************************/

			else 
			{
				Descriptor descriptor;
				descriptor.name = nm;
				descriptor.length = Len;
				descriptor.offset = Off;
				if (currDirec != "null" && active) {
					wad->translator[nm + ('_' + currDirec)] = descriptor;
					
				}
				else {
					string root = "_/";
					string newName = nm + root;
					wad->translator[newName] = descriptor;
					wad->directories["/"].push_back(nm);
					Root = true;
				}
				if (!Root) {
					files.push_back(nm);
				}
				Root = false;
			}

		}

		test2.close();
		return wad;
	}
		return nullptr;
}

string Wad::getMagic() { //Returns the magic for this WAD data.
//return wad->magic;
	return this->magic;
}

bool Wad::isContent(const string& path) {
	//Returns true if path represents content(data), and false otherwise.
	//if not null is content
	//node for each 
	//node class vector of node pointers
	//if vector has node pointers 
	//only directories have no content
	//files have content
	std::size_t found = path.find_last_of("/\\");
	string Path = path.substr(0, found);
	string File = path.substr(found + 1);
	found = Path.find_last_of("/\\");
    string Direc = Path.substr(found + 1);
	if (Direc == "") {
		Direc = "/";
	}
	if (translator[File+("_"+Direc)].length != 0) {
		return true;
	}
	return false;
}

bool Wad::isDirectory(const std::string& path) {
	//Returns true if path represents a directory, and false otherwise.
	switch (getSize(path)) {
	case -1:
		return true;

	default:
		return false;
	}

}

int Wad::getSize(const std::string& path) {
	//If path represents content, returns the number of bytes in its data; otherwise, returns - 1.
	if (isContent(path)) {
		std::size_t found = path.find_last_of("/\\");
		string Path = path.substr(0, found);
		string File = path.substr(found + 1);
		found = Path.find_last_of("/\\");
		string Direc = Path.substr(found + 1);
		if (Direc == "") {
			Direc = "/";
		}
		return translator[File + '_' + Direc].length;
		

	}
	return -1;
}

int Wad::getContents(const std::string& path, char* buffer, int length, int offset) {
	/*
	If path represents content, copies as many bytes as are available, up to length, of content's data into the pre-
	existing buffer.If offset is provided, data should be copied starting from that byte in the content.Returns
	number of bytes copied into buffer, or -1 if path does not represent content(e.g., if it represents a directory).
	*/
	fstream test2;

	test2.open(path, ios::in | ios::out);
	test2.seekg(offset, ios::beg);
	test2.read(buffer, length);
	return length;

	return -1;
}

int Wad::getDirectory(const std::string& path, std::vector<std::string> *directory) {

	if (path == "/") {
		*directory = directories["/"];
		return directories["/"].size();
	}
	string name = path.substr(0, path.length() - 1);
	std::size_t found = name.find_last_of("/\\");

    name = name.substr(found+1, name.length()-found);
	
	if (directories.find(name) == directories.end()) {
		return -1;
	}
	
 	*directory = directories.at(name);
	return directories.at(name).size();

}




