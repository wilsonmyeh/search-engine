#include <iostream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <ctype.h>
#include <stdlib.h>
#include "mergesortkevin.h"

using namespace std;

//strut to hold the company and bid details
struct comp_bid {
    string keyword;
    double bid;
    string company;
};

//compares two strings

int main(int argc, char *argv[]){
    if(argc < 3){
        cerr << "Need two filenames" << endl;
        return 1;
    }

    //input file
    ifstream ifile(argv[1]);
    if(ifile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    //output file
    ofstream ofile(argv[2]);
    if(ofile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    string key;
    cout<<"Enter keyword"<<endl;
    cin>>key;

    vector<comp_bid> comp;
    comp_bid cb;  
    vector<double> bidVec;
    vector<string> compTempVec, compVec; 
    IntComp c;
    AlphaStrComp c2;


    int count=0;
    int numLines;
    int j=0;
    string line;

    //reads the file line by line, extracts every word and stores it in the struct object. Then push the struct obj into the vector comp
    while (getline(ifile, line))  //while their are lines on the page
    {
        stringstream ss;
        ss << line;
        if(count==0) {
            ss>>numLines;
        }
        else if(count>0 && count <=numLines) { 
            ss>>cb.keyword;
            ss>>cb.bid;
            ss>>cb.company;
            comp.push_back(cb);
        }

        count ++;
    }


     ifile.close();


     //if the keyword in the struct matches the user entered keyword store the bid value in bidvec

    for(int i=0;i<numLines;i++) {
        cb = comp[i];
        if(cb.keyword == key) {
            bidVec[i]=cb.bid;
        }
    }   
      

    //sort all the bidvalues
     for(int i=0;i<numLines;i++) {
     mergesort(bidVec, c);
    }

     //if the keyword in the struct matches the user entered keyword and bid value equals the one in the vector store the company name for that bid value in a temp vactor    
    for(int j=0; j<bidVec.size(); j++) {
     for(int i=0;i<numLines;i++) {
        cb = comp[i];
        if(cb.keyword == key && cb.bid == bidArr[j]) {
            compTempVec.push_back(cb.company);
        }
    }  

    //sort the temp vector alphabetically (company names) 
    mergesort(compTempVec, c2);
    //push all the company names into a vector
    compVec.push_back(cb.company);
    }

    //print these company names
    for(int i=0;i<compVec.size();i++) {
       cout<<compVec[i];
    }
}