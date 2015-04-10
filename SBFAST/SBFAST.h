#ifndef _SBFAST_
#define _SBFAST_

#include <iostream>


#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "SBFAST_Log_Blocks.h"
#include "math.h"


#define INVALID -10
#define FREE -20
#define NOT_FIND -30
#define FIND -40
#define NO_VALID_DATA -50

extern double Pages_Per_Block;

extern long RND_Log_Blocks_Fisrt_LBN;
extern long SEQ_Log_Blocks_Fisrt_LBN;

extern double RND_Log_Blocks_Num_SBFAST;
extern double SEQ_Log_Blocks_Num_SBFAST;
extern double Pages_Per_Subblock;


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

extern vector<long> traceSeqStreamCapPagesCounter; 

using namespace std;
class SBFAST
{
public:
    // 1
    SBFAST(fstream & incomingWriteSeqFile);
    // 2
    void writeToBlocks (string currentIOLine);
    // 3
    bool partialMergeNewestSameLBNBlockInSEQ(long partialVictimNewestBlock, vector <long> blocksWithSameDataLBNInSEQ);
    // 4
    bool fullMergeNewestSameLBNBlockInSEQ(long partialVictimNewestBlock, vector <long> blocksWithSameDataLBNInSEQ, long currentLSN);
    // 5
    bool fullMergeInRND(long victimBlock, long currentLSN);
    // 6
    void printVector(vector<long> v);
    // 7
    void printSet(set<long> v);
    // 8
    void cleanupLog();  
    // 9
    bool kickoutBlockInSEQForCleanup(long victimBlock);
    // 10
    bool fullMergeInRNDForCleanup(long victimBlock);

private:
    double _Pages_Per_Block;
    double _Pages_Per_Subblock;
    double _RND_Log_Blocks_Num;
    double _SEQ_Log_Blocks_Num;
    long _RND_Log_Blocks_Fisrt_LBN;
    long _SEQ_Log_Blocks_Fisrt_LBN;
    
    SBFAST_Log_Blocks _RND_Log_Blocks;
    SBFAST_Log_Blocks _SEQ_Log_Blocks;

    
};




#endif /* defined(_SBFAST_) */
