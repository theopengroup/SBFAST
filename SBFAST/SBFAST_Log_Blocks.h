#ifndef _SBFAST_Log_Blocks_
#define _SBFAST_Log_Blocks_
#include <iomanip>
#include <iostream>
#include <vector>
#include <set>
#include "math.h"

#define INVALID -10
#define FREE -20
#define NOT_FIND -30
#define FIND -40
#define NO_VALID_DATA -50

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
using namespace std;
extern vector<long> traceSeqStreamCapPagesCounter;


class SBFAST_Log_Blocks
{
    
public:
    // 01
 	SBFAST_Log_Blocks(long _Pages_Per_Block, long Log_Blocks_Num, long First_LBN);
    // 02
    void eraseBlock(long LBN);
    // 03
    vector<long> readBlock(long LBN);
    // 04
    long readPage(long LBN, long offset);
    // 05-A
    bool writePage(long LBN,long offset, long pageValue);
    // 05-B
    bool writePageDef(long LBN,long offset, long pageValue);
    // programming purpose
    // 06
    vector<vector<long>> readLogBlocks();
    // 07
    bool blockIsFree (long LBN);
    // 08
    bool blocksAreFree ();
    // 09
    bool blockIsFull (long LBN);
    // 10
    bool blocksAreFull ();
    // 11
    long getFirstPageOfBlock(long LBN);
    // 12
    void appendToBlock(long LBN, long pageValue);
    // 13
    bool appendToBlocks(long pageValue);

    // 15
    void partialMergeForSEQ (long newestBlockWithSameDataLBNInSEQ, vector<long> blocksWithSameDataLBNInSEQ);
    // 16
    long updateVictimLBN();
    // 17
    void fullMergeForRND(long victimLBN, long pageValue);
    // 18
    bool markInvalidToSameAddressPages(long pageValue);
    // 19
    long getVictimLBN();
    // 20-A
    long findNewestBlockWithSameDataLBNInSEQ(long LBN, vector<long> &BlocksWithSameLBN);
    // 20-B
    long findNewestBlockWithSameDataLBNInSEQ(long LBN);
    // 21
    long findFreeLBN();
    // 22
    bool markInvalidToSameAddressPagesAndLockBlock(long pageValue);
    // 23
    bool checkValidBlock(long LBN);
    // 24
    bool isSeqBlock(long LBN);
    // 25
    bool blockHasInvalidPage(long LBN);
    
    
private:
    vector<vector<long>> _Log_Blocks;
    long _Pages_Per_Block;
    long _Log_Blocks_Num;
    long _First_LBN;
    long _currentVictimLBN=0;
};

#endif /* defined(_SBFAST_Log_Blocks_) */
