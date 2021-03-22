//attrnobool.cpp: the single source file for the attrnobool.exe tool
//takes 
	//- either an "all cont" attribute file, or a list of attributes, one name per line in the right order 
	//- the name of the target attribute
	//- (optional) the name of the weight attribute 
//produces a new attribute file with all features marked as cont, target and weight set.
//important note: it loses all information about original types of attributes (in particular, nominals), 
//and inactive attributes. All this information has to be restored in the new file manually.

#pragma warning(disable : 4996) //complaints about strerror function

#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <string.h>

using namespace std;

typedef vector<float> floatv;
typedef vector<int> intv;
typedef vector<string> stringv;
typedef numeric_limits<float> flim;

#define LINE_LEN 500000	//maximum length of line in the input file
#define QNAN flim::quiet_NaN()

#if defined(__VISUALC__)
    #define wxisNaN(n) _isnan(n)
#elif defined(__GNUC__)
    #define wxisNaN(n) isnan(n)
#else
    #define wxisNaN(n) ((n) != (n))
#endif

void getLineExt(fstream& fin, char* buf);
bool mv(string& str);
string trimSpace(const string& str);

//attrnobool _attr_file_ _new_attr_file_  _target_name_ [weight_name]
int main(int argc, char* argv[])
{
	try{
	//check presence of input arguments
	if((argc < 4) || (argc > 5))
		throw string("Usage: attrnobool _attr_file_ _new_attr_file_ _target_name_ [weight_name]");

	//open files, check that they are there
	fstream fattr(argv[1], ios_base::in);
	if(!fattr)
		throw string("Error: failed to open attribute file ") + string(argv[1]);

	fstream fnew(argv[2], ios_base::out);
	if(!fnew)
		throw string("Error: failed to open data file ") + string(argv[2]);

	string tarName(argv[3]);
	string weightName((argc == 5) ? argv[4] : "");

	char buf[LINE_LEN];	//buffer for reading from input files
 	getLineExt(fattr, buf);

	//read names of attributes, figure out their number
	stringv attrNames;
	int attrN; // counter
	bool tarFound = false;
	for(attrN = 0; fattr.gcount(); attrN++)
	{
		string attrStr(buf);	//a line of an attr file (corresponds to 1 attribute)
		attrStr = trimSpace(attrStr);
		if((attrStr.find("contexts") != -1) || (attrStr.size() == 0)) 
			break; //end of listed attributes
		
		//parse attr name
		string::size_type colonPos = attrStr.find(":");
		string::size_type nameLen = (colonPos == string::npos) ? attrStr.size() : colonPos;
		string attrName = trimSpace(attrStr.substr(0, nameLen));
		attrNames.push_back(attrName);
		if(tarName.compare(attrName) == 0)
			tarFound = true;

		getLineExt(fattr, buf);
	}
	fattr.close();
	if (!tarFound)
		throw string("Error: Could not find the target attribute ") + tarName;


//Output new attribute file
	for(int attrNo = 0; attrNo < attrN; attrNo++)
	{
		fnew << attrNames[attrNo] << ": cont";
		if(tarName.compare(attrNames[attrNo]) == 0)
			fnew << "(class)";
		if((argc == 5) && (weightName.compare(attrNames[attrNo]) == 0))
			fnew << "(weight)";
		fnew << "." << endl;
	}
	fnew << "contexts:" << endl;

	}catch(string err){
	  cerr << err << endl;
		return 1;
	}catch(...){
		string errstr = strerror(errno);
		cerr << errstr << endl;
		return 1;
	}
	return 0;
}

//extends fstream::getline with check on exceeding the buffer size
void getLineExt(fstream& fin, char* buf)
{
	fin.getline(buf, LINE_LEN);
	if(fin.gcount() == LINE_LEN - 1)
		throw string("Error: lines in an input file exceed current limit.");
}

bool mv(string& str)
{
	string trimstr = string(str.c_str());
	return !trimstr.compare("?");
}

//Not in-place, returns a new string. Deletes spaces from the beginning and from the end of the string
//By "spaces" I mean spaces only, not white spaces
string trimSpace(const string& str)
{
	int n = (int)str.size();
	
	int b; 
	for(b = 0; (b < n) && (str[b] == ' '); b++);
	
	if(b == n)
		return string();
	
	int e;
	for(e = n - 1; (str[e] == ' ') || (str[e] == '\r'); e--);
	
	return str.substr(b, e - b + 1);
}

