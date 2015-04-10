#ifndef _FAST_
#define _FAST_

#include <iostream>


#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "FAST_Log_Blocks.h"
#include "math.h"

#define FREE -20
#define INVALID -10

extern double Pages_Per_Block;

extern double RND_Log_Blocks_Num_FAST;
extern double SEQ_Log_Blocks_Num_FAST;

extern long RND_Log_Blocks_Fisrt_LBN;
extern long SEQ_Log_Blocks_Fisrt_LBN;

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
class FAST
{
public:
    FAST(fstream & incomingWriteSeqFile);
    void writeToBlocks (string currentIOLine);
    void cleanupLog();
    
private:
    double _Pages_Per_Block;
    double _RND_Log_Blocks_Num;
    double _SEQ_Log_Blocks_Num;
    long _RND_Log_Blocks_Fisrt_LBN;
    long _SEQ_Log_Blocks_Fisrt_LBN;
    
    FAST_Log_Blocks _RND_Log_Blocks;
    FAST_Log_Blocks _SEQ_Log_Blocks;
    

};

    
    
    
#endif /* defined(_FAST_) */
