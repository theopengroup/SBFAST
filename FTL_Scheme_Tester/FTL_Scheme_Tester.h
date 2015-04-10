#ifndef _FTL_Scheme_Tester_
#define _FTL_Scheme_Tester_

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



// Global variables
// 01. Log Block Size
extern double Pages_Per_Block;
extern double Pages_Per_Subblock;

// BAST
extern double Log_Blocks_Num_BAST;

// FAST
extern double RND_Log_Blocks_Num_FAST;
extern double SEQ_Log_Blocks_Num_FAST;

// SBFAST
extern double RND_Log_Blocks_Num_SBFAST;
extern double SEQ_Log_Blocks_Num_SBFAST;

extern long Log_Blocks_Fisrt_LBN;
extern long RND_Log_Blocks_Fisrt_LBN;
extern long SEQ_Log_Blocks_Fisrt_LBN;

// 02. counters
// BAST
extern long Erase_BAST;
extern long FM_BAST;
extern long SM_BAST;
extern long CapCrctStrm_BAST;

// FAST
extern long Erase_FAST;
extern long FMinRLB_FAST;
extern long FMinSLB_FAST;
extern long PM_FAST;
extern long SM_FAST;
extern long ZeroAssoFM_FAST;
extern long AssoDBlk_FAST;
extern long PMCpyPg_FAST;
extern long CapCrctBlkHdr_FAST;
extern long CapWrgBlkHdr_FAST;

// SBFAST
extern long Erase_SBFAST;
extern long FMinRLB_SBFAST;
extern long FMinSLB_SBFAST;
extern long PM_SBFAST;
extern long SM_SBFAST;
extern long ZeroAssoFM_SBFAST;
extern long AssoDBlk_SBFAST;
extern long PMCpyPg_SBFAST;
extern long FMinRLBCpyPg_SBFAST;
extern long FMinSLBCpyPg_SBFAST;
extern long CapCrctStrmSubHdr_SBFAST;
extern long CapWrgStrmSubHdr_SBFAST;


extern ofstream outputFile;
extern vector<vector<long>> BASTResults;
extern vector<vector<long>> FASTResults;
extern vector<vector<long>> SBFASTResults;



extern long avgPagesCapForEachSubSeqStrm;
extern vector<long> traceSeqStreamCapPagesCounter;


using namespace std;
class FTL_Scheme_Tester
{
public:
    FTL_Scheme_Tester(char * incomingWriteSeqFileChar);
    void recordResults();
    
private:
    fstream incomingWriteSeqFileForPrint;
    fstream incomingWriteSeqFileForBAST;
    fstream incomingWriteSeqFileForFAST;
    fstream incomingWriteSeqFileForSBFAST;
    
    
    
};







    
    
#endif /* defined(_FTL_Scheme_Tester_) */

