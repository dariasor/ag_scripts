//rnd_tv_dta executable: converts csv or dta files with or without header to randomly split dta files without header
//if all flag is set, everything goes into train file, in which case its name is just stem
//automatically identifies the empty line after the header
//extra option: change -1 to 0 in the first column
//extra option: first column is group column, data point with the same group go to the same split
//
//(c) Daria Sorokina

#pragma warning(disable : 4996)
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <algorithm>


#include <string>
#include <map>

using namespace std;

typedef map<string, int> simap;

const int lineLen = 500000; //max size that I so far needed
const int shLineLen = 16;

int main(int argc, char* argv[])
{
	try{

	if(argc != 13)	//12 input args (command name excluded) - max, once I reach 13, redo the args properly
		throw string("Usage: rnd_tv_dta data_file out_stem rand_seed mtoz|no csv|dta header|no") +
			string(" test|no|all target_file|no|q group|no group_column valid_proportion train_proportion");

	string mtozstr(argv[4]);
	bool mtoz = (mtozstr.compare("mtoz") == 0);
	string csvstr(argv[5]);
	char delch = '\t';
	if(csvstr.compare("csv") == 0)
		delch = ',';
	string headerstr(argv[6]);
	bool header = (headerstr.compare("header") == 0);
	string teststr(argv[7]);
	bool test = (teststr.compare("test") == 0);
	bool all = (teststr.compare("all") == 0);
	string tarstr(argv[8]);
	bool target = (tarstr.compare("no") != 0) && (tarstr.compare("q") != 0);
	bool tarq = (tarstr.compare("q") == 0);
	string groupstr(argv[9]);
	bool doGroup = (groupstr.compare("group") == 0);
	int groupNo = atoi(argv[10]) - 1;
	double propVal = atof(argv[11]);
	double propTrain = atof(argv[12]);

	//open files, check that they are there
	fstream fdata(argv[1], ios_base::in);
	if(!fdata)
		throw string("Error: failed to open file ") + string(argv[1]);

	string trainFName(argv[2]);
	if(!all)
		trainFName += ".train.dta";
	fstream ftrain(trainFName.c_str(), ios_base::out);
	if(!ftrain)
		throw string("Error: failed to open file ") + trainFName;

	string validFName(string(argv[2]) + ".valid.dta");
	fstream fvalid(validFName.c_str(), ios_base::out);
	if(!fvalid)
		throw string("Error: failed to open file ") + validFName;

	string testFName(string(argv[2]) + ".test.dta");
	fstream ftest;
	if(test)
	{
		ftest.open(testFName.c_str(), ios_base::out);
		if(!ftest)
			throw string("Error: failed to open file ") + testFName;
	}

	fstream ftar;
	if(target)
	{
		ftar.open(tarstr.c_str(), ios_base::in);
		if(!ftar)
			throw string("Error: failed to open file ") + tarstr;
	}

	srand(atoi(argv[3]));
	
	char buf[lineLen];
	char tarBuf[shLineLen];
	if(header)
	{
		fdata.getline(buf, lineLen); //header, skip it
		if(target)
			ftar.getline(tarBuf, shLineLen);
	}
	fdata.getline(buf, lineLen);
	if(target)
		ftar.getline(tarBuf, shLineLen);
	if(header && (fdata.gcount() == 0))
	{
		fdata.getline(buf, lineLen); //skip empty string
		if(target)
			ftar.getline(tarBuf, shLineLen);
	}
	
	simap groups; //needed if grouping is on
	string groupID;
	simap::iterator groupIt = groups.end();

	for(int i = 1; !fdata.fail(); i++)
	{
          if(i % 100000 == 0)
            cout << "read " << i << " lines" << endl;

		string str(buf);
		//trim delimiters at the end
		while(!str.empty() && (str[str.size() - 1] == delch))
			str = str.substr(0, str.size() - 1);

		string tarstr;
		if(target)
			tarstr = string(tarBuf);

		if(doGroup)
		{
			string::size_type prevDelim = -1;
			for(int colNo = 0; colNo < groupNo; colNo++)
				prevDelim = str.find(delch, prevDelim + 1);
			string::size_type delim = str.find(delch, prevDelim + 1);
			groupID = str.substr(prevDelim + 1, delim - prevDelim - 1);
			groupIt = groups.find(groupID);
		}

		double randCoef = (double) rand() / RAND_MAX;
		if(all || 
		   (groupIt != groups.end()) && (groupIt->second == 1) ||
		   (groupIt == groups.end()) && (randCoef < propTrain)
		  )
		{//output to train
			//if mtoz flag is on, and the first value is -1, replace it with 0
			int len = str.size();
			if((len > 1) && mtoz && (str[0] == '-') && (str[1] == '1'))
			{
				ftrain << '0';
				if(len == 2)
				{
					ftrain << endl;
					continue;
				}
				ftrain << '\t';
				str = str.substr(3);
			}

			//output the rest of the line, replacing commas with tabs
			while(str.size() != 0)
			{
				string::size_type delim = str.find(delch);
				string field = str.substr(0, delim);
				if (field.empty())
					field = "?";
				ftrain << field;

				if(delim == string::npos)
					break;

				ftrain << '\t';
				str = str.substr(delim + 1);
			}

			//attach target as the last column, if it is separate
			if(target || tarq)
			{
				ftrain << '\t';
				if(tarq)
					ftrain << '?';
				else if(target)
					ftrain << tarstr;
			}
			ftrain << endl;

			if(doGroup && (groupIt == groups.end()))
				groups.insert(simap::value_type(groupID, 1));
		}
		else if((groupIt != groups.end()) && (groupIt->second == 2) ||
				(groupIt == groups.end() && (randCoef >= (1 - propVal)))
				)
		{//output to validation
			//if mtoz flag is on, and the first value is -1, replace it with 0
			int len = str.size();
			if((len > 1) && mtoz && (str[0] == '-') && (str[1] == '1'))
			{
				fvalid << '0';
				if(len == 2)
				{
					fvalid << endl;
					continue;
				}
				fvalid << '\t';
				str = str.substr(3);
			}

			//output the rest of the line, replacing commas with tabs
			while(str.size() != 0)
			{
				string::size_type delim = str.find(delch);
				string field = str.substr(0, delim);
				if(field.empty())
					field = "?";
				fvalid << field;

				if(delim == string::npos)
					break;

				fvalid << '\t';
				str = str.substr(delim + 1);
			}

			//attach target as the last column, if it is separate
			if(target || tarq)
			{
				fvalid << '\t';
				if(tarq)
					fvalid << '?';
				else if(target)
					fvalid << tarstr;
			}
			fvalid << endl;

			if(doGroup && (groupIt == groups.end()))
				groups.insert(simap::value_type(groupID, 2));
		} else {
            if(test)
			{//output to test set
				//if mtoz flag is on, and the first value is -1, replace it with 0
				int len = str.size();
				if((len > 1) && mtoz && (str[0] == '-') && (str[1] == '1'))
				{
					ftest << '0';
					if(len == 2)
					{
						ftest << endl;
						continue;
					}
					ftest << '\t';
					str = str.substr(3);
				}

				//output the rest of the line, replacing commas with tabs
				while(str.size() != 0)
				{
					string::size_type delim = str.find(delch);
					string field = str.substr(0, delim);
					if (field.empty())
						field = "?";
					ftest << field;

					if (delim == string::npos)
						break;

					ftest << '\t';
					str = str.substr(delim + 1);
				}

				//attach target as the last column, if it is separate
				if(target || tarq)
				{
					ftest << '\t';
					if(tarq)
						ftest << '?';
					else if(target)
						ftest << tarstr;
				}
				ftest << endl;
			}
			if(doGroup && (groupIt == groups.end()))
				groups.insert(simap::value_type(groupID, 3));
		}

		fdata.getline(buf, lineLen); 
		if(target)
			ftar.getline(tarBuf, shLineLen);
	}

	fdata.close();
	ftrain.close();
	fvalid.close();
	if(test)
		ftest.close();
	if(target)
		ftar.close();

	}catch(string err){
		cerr << err << endl;
		return 1;
	}catch(...){
		string errstr = strerror(errno);
		cerr << "Error: " << errstr << endl;
		return 1;
	}
	return 0;
}

