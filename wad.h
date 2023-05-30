#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include<string>
#include<vector>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

struct Descriptor {
public:
	int offset = 0;
	int length = 0;
	string name;
	bool isActive = false;
};



class Wad {
public:
	string magic;
	int numOfDescriptors = 0;
	//Directory F <F1>
	//Directory F1 <E1M1, LolWut>
	//Directory E1M1
	//MAPS HERE
	
	map<string, Descriptor> translator;
	map<string, vector<string>> directories;
	map<int, vector<string>> levels;           //can delete, doesnt do anything anymore

	//if (is duplicate) string.substring (string.end , / /)
	Wad();
	static Wad* loadWad(const string& path);
	string getMagic();
	bool isContent(const string& path);
	bool isDirectory(const string& path);
	int getSize(const string& path);
	int getContents(const string& path, char* buffer, int length, int offset = 0);
	int getDirectory(const string& path, vector<string>* directory);
};
