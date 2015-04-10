#ifndef _FAST_LOG_BLOCKS_
#define _FAST_Log_BLOCKS_
#include <iomanip>
#include <iostream>
#include <vector>
#include <set>
#include "math.h"

#define FREE -20
#define INVALID -10

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



using namespace std;
class FAST_Log_Blocks
{
    
public:
    // 01
 	FAST_Log_Blocks(long _Pages_Per_Block, long Log_Blocks_Num, long First_LBN);
    // 02
    void eraseBlock(long LBN);
    // 03
    vector<long> readBlock(long LBN);
    // 04
    long readPage(long LBN, long offset);
    // 05
    bool writePage(long LBN,long offset, long pageValue);

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
    long getLastPageOfBlock(long LBN);
    // 12
    void appendToBlock(long LBN, long pageValue);
    // 13
    bool appendToBlocks(long pageValue);
    // 14
    void switchMerge (long Victim_Block_Num);
    // 15
    void partialMergeForSEQ (long Victim_Block_Num);
    // 16
    long updateVictimLBN();
    // 17
    void fullMergeForRND(long victimLBN);
    // 18
    void markInvalidToSameAddressPages(long pageValue);
    // 19
    long getVictimLBN();
    // 20
    bool isSeqBlock(long LBN);
    // 21
    bool fullMergeInSEQ(long currentLSN);

    
private:
    vector<vector<long>> _Log_Blocks;
    long _Pages_Per_Block;
    long _Log_Blocks_Num;
    long _First_LBN;
    long _currentVictimLBN=0;
};

#endif /* defined(_FAST_LOG_BLOCKS_) */
