#ifndef _BAST_
#define _BAST_

#include <iostream>


#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "BAST_Log_Blocks.h"
#include "math.h"

#define INVALID -10
#define FREE -20
#define NOT_FIND -30
#define FIND -40
#define NO_VALID_DATA -50

extern double Pages_Per_Block;

extern double Log_Blocks_Num_BAST;

extern long Log_Blocks_Fisrt_LBN;


extern long Erase_BAST;
extern long FM_BAST;
extern long SM_BAST;
extern long CapCrctStrm_BAST;



using namespace std;
class BAST
{
public:
    BAST(fstream & incomingWriteSeqFile);
    void writeToBlocks (string currentIOLine);
    void cleanupLog();
    
private:
    double _Pages_Per_Block;
    double _Log_Blocks_Num;
    long _Log_Blocks_Fisrt_LBN;
    
    BAST_Log_Blocks _Log_Blocks;
    
    
};




#endif /* defined(_BAST_) */
