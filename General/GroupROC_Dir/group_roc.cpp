#pragma warning(disable : 4996)

#include <fstream>
#include <iostream>
#include <errno.h>
#include <vector>
#include <string>
#include <algorithm>
#include <string.h>
#include <limits>
#include <cmath>

using namespace std;

typedef vector<double> doublev;
typedef pair<double, double> ddpair;
typedef pair<double, ddpair> dddtriple;
typedef vector<dddtriple> dddtriplev;
typedef vector<string> stringv;
typedef pair<string, int> sipair;
typedef vector<sipair> sipairv;

#define QNAN numeric_limits<double>::quiet_NaN()
#define LINE_LEN 20000	//maximum length of line in the input file

//extends fstream::getline with check on exceeding the buffer size
void getLineExt(fstream& fin, char* buf)
{
	fin.getline(buf, LINE_LEN);
	if(fin.gcount() == LINE_LEN - 1)
		throw string("Erros: lines in an input file exceed current limit.");
}

double atofExt(string str)
{
	char *end;
	double value = strtod(str.c_str(), &end);
	if((end == str) || (*end != '\0'))
		throw string("Error: non-numeric value \"") + string(str) + string("\"");
	return value;
}

double roc(doublev::iterator preds, doublev::iterator tars, doublev::iterator weights, int itemN)
{
	double volume = 0; //sum of all weights

	dddtriplev data(itemN);
	bool onlyOnes = true;
	bool onlyZeros = true;
	for(int i = 0; i < itemN; i++)
	{
		data[i].first = preds[i];
		data[i].second.first = tars[i];
		data[i].second.second = weights[i];
		volume += weights[i];
		if((tars[i] < 0) || (tars[i] > 1))
			throw "Error: can't calculate ROC with response values out of [0,1] range.\n";
		if(tars[i] != 0)
			onlyZeros = false;
		if(tars[i] != 1)
			onlyOnes = false;
	}
	if(onlyZeros || onlyOnes)
		return QNAN;

	sort(data.begin(), data.end());

	/* get the fractional weights. If there are ties we count the number
	of cases tied and how many positives there are, and we assign  
	each case to be #pos/#cases positive */

	doublev fraction(itemN);
	int item = 0;
	while(item < itemN)
	{
		int begin = item;
		double posV = 0;
		double tieV = 0;
		for(;(item < itemN) && (data[item].first == data[begin].first); item++)
		{
			posV += data[item].second.first * data[item].second.second; 
			tieV += data[item].second.second;
		}

		double curFrac = posV / tieV;
		for(int i = begin; i < item; i++)
			fraction[i] = curFrac;
	}

	double pos = 0.0; //number of positives in the data
	double neg = 0.0; //number of negatives in the data
	for(int i = 0; i < itemN; i++)
	{
		pos += data[i].second.first * data[i].second.second;
		neg += (1 - data[i].second.first) * data[i].second.second;
	}

	double tpos = 0.0; //number of true positives
	double fpos = 0.0; //number of false positives

	double roc_area = 0.0;
	double tpr_prev = 0.0; //tpr = true positives rate
	double fpr_prev = 0.0; //fpr = false positives rate

						   //calculate auc incrementally
	for(item = itemN - 1; item > -1; item--)
	{
		tpos += fraction[item] * data[item].second.second;
		fpos += (1 - fraction[item]) * data[item].second.second;
		double tpr  = tpos / pos;
		double fpr  = fpos / neg;
		roc_area += 0.5 * (tpr + tpr_prev) * (fpr - fpr_prev);
		tpr_prev = tpr;
		fpr_prev = fpr;
	}

	return roc_area;
}

double roc(doublev::iterator preds, doublev::iterator tars, int itemN)
{
	doublev weights(itemN,1);
	return roc(preds, tars, weights.begin(), itemN);
}

//roc averaged across groups
double group_roc(doublev& preds_in, doublev& tars_in, stringv& groups_in, bool sorted)
{
	//if the data is not sorted by group, create sorted copies of all input arrays
	int itemN = (int)preds_in.size();
	doublev preds_s, tars_s; 
	stringv groups_s;
	if(!sorted)
	{
		sipairv groupids;
		groupids.resize(itemN);
		for(int itemNo = 0; itemNo < itemN; itemNo++)
		{
			groupids[itemNo].first = groups_in[itemNo];
			groupids[itemNo].second = itemNo;
		}
		sort(groupids.begin(), groupids.end());

		preds_s.resize(itemN);
		tars_s.resize(itemN);
		groups_s.resize(itemN);
		for(int itemNo = 0; itemNo < itemN; itemNo++)
		{
			preds_s[itemNo] = preds_in[groupids[itemNo].second];
			tars_s[itemNo] = tars_in[groupids[itemNo].second];
			groups_s[itemNo] = groups_in[groupids[itemNo].second];
		}
	}
	doublev& preds = sorted ? preds_in : preds_s;
	doublev& tars = sorted ? tars_in : tars_s;
	stringv& groups = sorted ? groups_in : groups_s;

	//now calculate roc for every group
	double meanVal = 0;
	int groupN = 0;
	int curBegins = 0;
	string curGr = groups[0];
	for(int itemNo = 0; itemNo < itemN; itemNo++)
	{
		if((itemNo == itemN - 1) || (groups[itemNo + 1].compare(curGr) != 0))
		{
			double rocVal = roc(preds.begin() + curBegins, tars.begin() + curBegins, itemNo + 1 - curBegins);
			cout << rocVal << endl;
			if(!isnan(rocVal))
			{
				meanVal += rocVal;
				groupN++;
			}
			if(itemNo != itemN - 1)
			{
				curGr = groups[itemNo + 1];
				curBegins = itemNo + 1;
			}
		}
	}
	return meanVal / groupN;
}

//group_roc _targets_ _predictions_ _groups_
int main(int argc, char* argv[])
{
	try{

		//check presence of input arguments
		if(argc != 4)
			throw string("Usage: group_roc _targets_ _predictions_ _groups_");

		fstream ftar(argv[1], ios_base::in);
		if(!ftar)
			throw string("Error: failed to open file ") + string(argv[1]);

		fstream fpred(argv[2], ios_base::in);
		if(!fpred)
			throw string("Error: failed to open file ") + string(argv[2]);

		fstream fgroup(argv[3], ios_base::in);
		if(!fgroup)
			throw string("Error: failed to open file ") + string(argv[3]);

		//load target and prediction values;
		doublev tars, preds; 
		stringv groups;
		char buf[LINE_LEN];	//buffer for reading strings from input files
		double hold_d;
		string hold_s;
		fpred >> hold_s;
		try
		{
			hold_d = atofExt(hold_s);
		}
		catch(...) {
			//header present, remove it form all columns
			ftar >> hold_s;
			getLineExt(fgroup, buf);
			fpred >> hold_s;
			hold_d = atofExt(hold_s);
		}

		while(!fpred.fail())
		{
			preds.push_back(hold_d);
			fpred >> hold_s;
			hold_d = atofExt(hold_s);
		}
		ftar >> hold_d;
		while(!ftar.fail())
		{
			tars.push_back(hold_d);
			ftar >> hold_d;
		}
		getLineExt(fgroup, buf);
		while(fgroup.gcount())
		{
			groups.push_back((string)buf);
			getLineExt(fgroup, buf);
		}
		if(tars.size() != preds.size())
			throw string("Error: different number of values in targets and predictions files");
		if(tars.size() != groups.size())
			throw string("Error: different number of values in targets and groups files");

		double retVal = group_roc(preds, tars, groups, true);
		cout << "Group AUC: " << retVal << endl;

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
