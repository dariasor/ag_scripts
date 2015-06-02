//prob_roc.cpp: main function of executable prob_roc
//Calculates probabilistic AUC from predictions and target value. 
//Targets can take on any values between 0 and 1, predictions can take on any continuous values
//
//(c) Daria Sorokina, Carnegie Mellon University, 2009

#pragma warning(disable : 4996)

#include <fstream>
#include <iostream>
#include <errno.h>
#include <vector>
#include <string>
#include <algorithm>
#include <string.h>

using namespace std;

typedef vector<double> doublev;
typedef pair<double, double> ddpair;
typedef vector<ddpair> ddpairv;

//prob_roc _targets_ _predictions_
int main(int argc, char* argv[])
{
	try{

cout << "test0\n";
	//check presence of input arguments
	if(argc != 3)
		throw string("Usage: prob_roc _targets_ _predictions_");

	fstream ftar(argv[1], ios_base::in);
	if(!ftar)
		throw string("Error: failed to open file ") + string(argv[1]);

	fstream fpred(argv[2], ios_base::in);
	if(!fpred)
		throw string("Error: failed to open file ") + string(argv[2]);

	//load target and prediction values;
	doublev tars, preds;
	double hold;
	ftar >> hold;
	while(!ftar.fail())
	{
		tars.push_back(hold);
		ftar >> hold;
	}
	fpred >> hold;
	while(!fpred.fail())
	{
		preds.push_back(hold);
		fpred >> hold;
	}
	if(tars.size() != preds.size())
		throw string("Error: different number of values in targets and predictions files");

	int itemN = tars.size();

cout << "test1\n";	
//************

	ddpairv data(itemN);
	for(int i = 0; i < itemN; i++)
	{
		data[i].first = preds[i];
		data[i].second = tars[i];
	}
      
	sort(data.begin(), data.end());
	
	/* get the fractional weights. If there are ties we count the number
    of cases tied and how many positives there are, and we assign  
    each case to be #poz/#cases positive and the rest negative */

	doublev fraction(itemN);
	int item = 0;
cout << "test2\n";
	while(item < itemN)
	{
		int begin = item;
		double posV = 0;
		for(;(item < itemN) && (data[item].first == data[begin].first); item++) 
			posV += data[item].second;

		double curFrac = posV / (item - begin);
		for(int i = begin; i < item; i++)
			fraction[i] = curFrac;
    }
cout << "test3\n";
	double tt = 0; 
	double tf = 0; 
	double ft = 0; 
	double ff = 0;

	//initialize tf and ff with ground truth
	for(int i = 0; i < itemN; i++)
	{
		tf += data[i].second;
		ff += 1 - data[i].second;
	}
    
	double roc_area = 0.0;
	double tpf_prev = 0;
	double fpf_prev = 0;
cout << "test4\n";
	for(item = itemN - 1; item > -1; item--)
    {
		tt += fraction[item];
		tf -= fraction[item];
		ft += 1 - fraction[item];
		ff -= 1 - fraction[item];
		double tpf  = tt / (tt + tf);
		double fpf  = 1.0 - ff / (ft + ff);
		roc_area += 0.5 * (tpf + tpf_prev) * (fpf - fpf_prev);
		tpf_prev = tpf;
		fpf_prev = fpf;
    }
cout << "test5\n";
	cout << "AUC: " << roc_area << endl;
	cout.flush();

	}catch(string err){
		cerr << "Error: " << err << endl;
		return 1;
	}catch(...){
		string errstr = strerror(errno);
		cerr << "Error: " << errstr << endl;
		return 1;
	}
	return 0;
}
