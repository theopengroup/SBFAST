/*! 
 Name: Zheng-Yu YANG \n
 Major: PhD in Electrical Engineering\n
 NUID: 000505655 \n
 Date: Mar 2013 \n
 Programming Environment: Mac OS X Lion, Xcode \n
 */
#include <iostream>
#include <vector>
#include <fstream>
#include "BAST.h"
#include "FAST.h"
#include "SBFAST.h"
#include "math.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <queue>
#include "FTL_Scheme_Tester.h"

// Global variables
// 01. Log Block Size
double Pages_Per_Block=64;
double Pages_Per_Subblock;

// BAST
double Log_Blocks_Num_BAST=10;

// FAST
double RND_Log_Blocks_Num_FAST=9;
double SEQ_Log_Blocks_Num_FAST=1;

// SBFAST
double RND_Log_Blocks_Num_SBFAST=4;
double SEQ_Log_Blocks_Num_SBFAST=6;

long Log_Blocks_Fisrt_LBN=0;
long RND_Log_Blocks_Fisrt_LBN=0;
long SEQ_Log_Blocks_Fisrt_LBN=0;

// 02. counters
// BAST
long Erase_BAST=0;
long FM_BAST=0;
long SM_BAST=0;
long CapCrctStrm_BAST=0;

// FAST
long Erase_FAST=0;
long FMinRLB_FAST=0;
long FMinSLB_FAST=0;
long PM_FAST=0;
long SM_FAST=0;
long ZeroAssoFM_FAST=0;
long AssoDBlk_FAST=0;
long PMCpyPg_FAST=0;
long CapCrctBlkHdr_FAST=0;
long CapWrgBlkHdr_FAST=0;

// SBFAST
long Erase_SBFAST=0;
long FMinRLB_SBFAST=0;
long FMinSLB_SBFAST=0;
long PM_SBFAST=0;
long SM_SBFAST=0;
long ZeroAssoFM_SBFAST=0;
long AssoDBlk_SBFAST=0;
long PMCpyPg_SBFAST=0;
long FMinRLBCpyPg_SBFAST=0;
long FMinSLBCpyPg_SBFAST=0;
long CapCrctStrmSubHdr_SBFAST=0;
long CapWrgStrmSubHdr_SBFAST=0;


// output file
ofstream outputFile ("/users/Zheng-Yu_YANG/Dropbox/Workspace/incomingWrite/Exp08/results.txt");

void printConfig();
void printComparison();
void clearCount();

vector<vector<long>> BASTResults;
vector<vector<long>> FASTResults;
vector<vector<long>> SBFASTResults;

long avgPagesCapForEachSubSeqStrm=0;
vector<long> traceSeqStreamCapPagesCounter;
long traceSeqStreamNumber=22142;
long traceSeqSubstreamSize=64;


using namespace std;
int main(int argc, const char * argv[])
{
    
    

    
    
    printConfig();
    //128
    //for (int i=32; i<=320; i=i+32)
    for (int i=4; i<=64 ;i=i*2)
    {
        Pages_Per_Subblock=i;        // Print line
        cout<<"===== "<<i<<" ====="<<endl;
        outputFile<<"===== "<<i<<" ====="<<endl;
        // Clear Counters
        clearCount();
        
        
        
        // manually set

        for (int i=0; i<traceSeqStreamNumber; i++)
        {
            traceSeqStreamCapPagesCounter.push_back(0);
        }
        
        
        
        
        
        
        
        
        
        // Open incomingWrite File
        string currentFileStr;
        currentFileStr=("/Users/Zheng-Yu_YANG/Dropbox/Workspace/incomingWrite/Exp08/");
        string s = to_string(i);
        currentFileStr.append(s);
        currentFileStr.append(".txt");
        char * currentFileChar;
        currentFileChar = new char [currentFileStr.size()+1];
        strcpy (currentFileChar, currentFileStr.c_str());

        // Run Experiment
        FTL_Scheme_Tester FTL_Scheme_Tester_Running(currentFileChar);
        
        // Print Line
        cout<<"===== "<<i<<" ====="<<endl;
        outputFile<<"===== "<<i<<" ====="<<endl;
    }
    
    
    // After all, print results.
    printComparison();
}








void printConfig()
{

            // Print the configurations at the beginning
            outputFile<<"=============================BAST============================="<<endl;
            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
            outputFile<<"Log_Blocks Number is:"<<Log_Blocks_Num_BAST<<endl;
            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;

            // Print the configurations at the beginning
            outputFile<<"=============================FAST============================="<<endl;
            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
            outputFile<<"SEQ_Log_Blocks Number is:"<<SEQ_Log_Blocks_Num_FAST<<endl;
            outputFile<<"RND_Log_Blocks Number is:"<<RND_Log_Blocks_Num_FAST<<endl;
            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;

            // Print the configuratins at the beginning
            outputFile<<"==============================SBFAST=========================="<<endl;
            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
            outputFile<<"SEQ_Log_Blocks Number is:"<<SEQ_Log_Blocks_Num_SBFAST<<endl;
            outputFile<<"RND_Log_Blocks Number is:"<<RND_Log_Blocks_Num_SBFAST<<endl;
            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
            outputFile<<"Pages_Per_Subblock is:"<<Pages_Per_Subblock<<endl;
            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;

    
}

void printComparison()
{
    outputFile<<"=============================BAST============================="<<endl;
    outputFile<<"Erase_BAST \t FM_BAST \t SM_BAST \t CapCrctStrm_BAST"<<endl;
        
    for (int i=0; i<BASTResults.size(); i++)
    {
        for (int j=0; j<BASTResults[i].size(); j++)
        {
            outputFile<<BASTResults[i][j]<<"\t";
        }
        outputFile<<endl;
        
    }
    //outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;

    
    
    
    outputFile<<"=============================FAST============================="<<endl;
    outputFile<<"Erase_FAST \t FMinRLB_FAST \t FMinSLB_FAST \t PM_FAST \t SM_FAST \t ZeroAssoFM_FAST \t AssoDBlk_FAST \t PMCpyPg_FAST \t CapCrctBlkHdr_FAST \t CapWrgBlkHdr_FAST"<<endl;
    
    for (int i=0; i<FASTResults.size(); i++)
    {
        for (int j=0; j<FASTResults[i].size(); j++)
        {
            outputFile<<FASTResults[i][j]<<"\t";
        }
        outputFile<<endl;
        
    }
    //outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;
    
    
    
    outputFile<<"==============================SBFAST=========================="<<endl;
    outputFile<<"Erase_SBFAST \t FMinRLB_SBFAST \t FMinSLB_SBFAST \t PM_SBFAST \t SM_SBFAST \t ZeroAssoFM_SBFAST \t AssoDBlk_SBFAST \t PMCpyPg_SBFAST \t FMinRLBCpyPg_SBFAST \t FMinSLBCpyPg_SBFAST \t CapCrctStrmSubHdr_SBFAST \t CapWrgStrmSubHdr_SBFAST \t totalCapSeqPages"<<endl;
    
    for (int i=0; i<SBFASTResults.size(); i++)
    {
        for (int j=0; j<SBFASTResults[i].size(); j++)
        {
            outputFile<<SBFASTResults[i][j]<<"\t";
        }
        outputFile<<endl;
        
    }
    //outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;
    
    


}

void clearCount()
{
    // Clear counters
    // BAST
    Erase_BAST=0;
    FM_BAST=0;
    SM_BAST=0;
    CapCrctStrm_BAST=0;
    
    //FAST
    Erase_FAST=0;
    FMinRLB_FAST=0;
    FMinSLB_FAST=0;
    PM_FAST=0;
    SM_FAST=0;
    ZeroAssoFM_FAST=0;
    AssoDBlk_FAST=0;
    PMCpyPg_FAST=0;
    CapCrctBlkHdr_FAST=0;
    CapWrgBlkHdr_FAST=0;
    
    
    //SBFAST
    Erase_SBFAST=0;
    FMinRLB_SBFAST=0;
    FMinSLB_SBFAST=0;
    PM_SBFAST=0;
    SM_SBFAST=0;
    ZeroAssoFM_SBFAST=0;
    AssoDBlk_SBFAST=0;
    PMCpyPg_SBFAST=0;
    FMinRLBCpyPg_SBFAST=0;
    FMinSLBCpyPg_SBFAST=0;
    CapCrctStrmSubHdr_SBFAST=0;
    CapWrgStrmSubHdr_SBFAST=0;

    
    // traceSeqStreamCapPagesCounter
    traceSeqStreamCapPagesCounter.clear();
    avgPagesCapForEachSubSeqStrm=0;
  
    
}
