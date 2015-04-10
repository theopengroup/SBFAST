#ifndef _BAST_LOG_BLOCKS_
#define _BAST_Log_BLOCKS_
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

extern double Pages_Per_Block;

extern double RND_Log_Blocks_Num_BAST;
extern double SEQ_Log_Blocks_Num_BAST;

extern long Log_Blocks_Fisrt_LBN;


extern long Erase_BAST;
extern long FM_BAST;
extern long SM_BAST;
extern long CapCrctStrm_BAST;


using namespace std;
class BAST_Log_Blocks
{
    
public:
    // 01
 	BAST_Log_Blocks(long _Pages_Per_Block, long Log_Blocks_Num, long First_LBN);
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
    long getFirstPageOfBlock(long LBN);
    // 12
    void appendToBlock(long LBN, long pageValue);
    // 13
    bool appendToBlocks(long pageValue);
    // 14
    void  switchMerge (long Victim_Block_Num);
    // 15
    void  merge(long Victim_Block_Num);
    // 16
    long updateVictimLBN();
    // 17
    void fullMergeForRND(long victimLBN);
    // 18
    void markInvalidToSameAddressPages(long pageValue);
    // 19
    long getVictimLBN();
    // 20
    long findBlockWithSameDataLBN(long LBN);
    // 21
    long findFreeLBN();
    // 22
    bool isSeqBlock(long LBN);


    
    
private:
    vector<vector<long>> _Log_Blocks;
    long _Pages_Per_Block;
    long _Log_Blocks_Num;
    long _First_LBN;
    long _currentVictimLBN=0;
};

#endif /* defined(_BAST_LOG_BLOCKS_) */
