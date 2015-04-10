#include "FAST_Log_Blocks.h"
// 01
FAST_Log_Blocks::FAST_Log_Blocks(long Pages_Per_Block, long Log_Blocks_Num, long FirstLBN):_Pages_Per_Block(Pages_Per_Block),_Log_Blocks_Num(Log_Blocks_Num), _First_LBN(FirstLBN)
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        vector<long> currentBlock;
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            currentBlock.push_back(FREE);
        }
        _Log_Blocks.push_back(currentBlock);
    }
    
}


// 02
void FAST_Log_Blocks::eraseBlock(long LBN)
{
    //cout<<"Erasing a block now, and before erasing, the eraseCount is "<<Erase_FAST<<endl;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        _Log_Blocks[LBN][i]=FREE;

    }
    Erase_FAST++;
    //cout<<"After erasing, the eraseCount is "<<Erase_FAST<<endl;
}

// 03
vector<long> FAST_Log_Blocks::readBlock(long LBN)
{
    return _Log_Blocks[LBN];
}




// 04
long FAST_Log_Blocks::readPage(long LBN, long offset)
{
    return _Log_Blocks[LBN][offset];
}


// 05
bool FAST_Log_Blocks::writePage(long LBN,long offset, long pageValue)
{
    if (_Log_Blocks[LBN][offset]==FREE)
    {
        _Log_Blocks[LBN][offset]=pageValue;
        return true;
    }
    else
        return false;
}


// programming purpose
// 06
vector<vector<long>> FAST_Log_Blocks::readLogBlocks()
{
    
    for (long column=0; column<_Log_Blocks_Num; column++)
    {
        //cout<<right<<setfill(' ')<<setw(19)<<"LBN"<<_First_LBN+column;
        
    }
    //cout<<endl;
    
    
    for (long row=0; row<_Pages_Per_Block; row++)
    {
        for (long column=0; column<_Log_Blocks_Num; column++)
        {
            if(_Log_Blocks[column][row]==FREE)
            {
                //cout<<right<<setfill(' ')<<setw(20)<<"FREE";
            }
            else if(_Log_Blocks[column][row]==INVALID)
            {
                //cout<<right<<setfill(' ')<<setw(20)<<"INVALID";
            }
            ////cout<<"  |  "<<_Log_Blocks[column][row]<<"  |  ";
            else
            {
                
                //cout<<right<<setfill(' ')<<setw(20)<<_Log_Blocks[column][row];
            }
            
        }
        //cout<<endl;
    }
        return _Log_Blocks;
}



// 07
bool FAST_Log_Blocks::blockIsFree (long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]!=FREE)
            return false;
    }
    return true;
}


// 08
bool FAST_Log_Blocks::blocksAreFree ()
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            if(_Log_Blocks[i][j]!=FREE)
                return false;
        }
    }
    return true;

}


// 09
bool FAST_Log_Blocks::blockIsFull(long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]==FREE)
            return false;
    }
    return true;
}

// 10
bool FAST_Log_Blocks::blocksAreFull ()
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            if(_Log_Blocks[i][j]==FREE)
                return false;
        }
    }
    return true;
    
}

// 11
long FAST_Log_Blocks::getLastPageOfBlock(long LBN)
{
    for (int i=_Pages_Per_Block-1; i>=0; i--)
    {
        if (_Log_Blocks[LBN][i]!=FREE)
        {
            return _Log_Blocks[LBN][i];
        }
    }
    return FREE;
}



// 12
void  FAST_Log_Blocks::appendToBlock(long LBN, long pageValue)
{
    if (blockIsFull(LBN)==true)
        cerr<<"The block is full. Cannot append anything inside."<<endl;
    else
    {
        // append from last to first page sector
        for (long i=0; i<_Pages_Per_Block; i++)
        {
            if (_Log_Blocks[LBN][i]==FREE)
            {
                _Log_Blocks[LBN][i]=pageValue;
                break;
            }
        }
    }

}


// 13
bool  FAST_Log_Blocks::appendToBlocks(long pageValue)
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        if (blockIsFull(i)!=true)
        {
            //cout<<"A not full block is found and we append the page to it."<<endl;
            appendToBlock(i, pageValue);
            return true;
        }
    }
//    //cout<<"The whole log blocks are full and we cannot append anything inside. We need to kick out one of them by FIFO order. And the victim is "<<_currentVictimLBN<<endl;
//    eraseBlock(_currentVictimLBN);
//    appendToBlock(_currentVictimLBN, pageValue);
//    //cout<<"appendToBlocks calls updateVictim()."<<endl;
//    updateVictimLBN();
//    return true;
    
}


// 14
void  FAST_Log_Blocks::switchMerge (long Victim_Block_Num)
{
    //cout<<"Switch Merge = 1 Erase for Data."<<endl;
    eraseBlock(Victim_Block_Num);
    SM_FAST++;
}

// 15
void  FAST_Log_Blocks::partialMergeForSEQ (long Victim_Block_Num)
{

    
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        ////cout<<"current page is "<<_Log_Blocks[LBN][i]<<endl;
        if(_Log_Blocks[Victim_Block_Num][i]==FREE)
        {
            PMCpyPg_FAST++;
        }
    }
    
    //cout<<"Partial Merge = Erase = 1 = Data."<<endl;
    eraseBlock(Victim_Block_Num); // We do "Log erase", but in fact we are going to erase "data log"
    PM_FAST++;

    
    
    
}

// 16
long FAST_Log_Blocks::updateVictimLBN()
{
    if ((_currentVictimLBN+1)<=(_First_LBN+_Log_Blocks_Num-1))
        _currentVictimLBN++;
    else
        _currentVictimLBN=_First_LBN;
    
    //cout<<"We are updating the victimLBNnum now, and after that it is "<<_currentVictimLBN<<endl;
    return _currentVictimLBN;

}
// 17
void FAST_Log_Blocks::fullMergeForRND(long victimLBN)
{
    // Calculate the number of associated blocks
    long Associated_Blocks_Num=0;
    set<long> Associated_Blocks;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if((_Log_Blocks[victimLBN][i]!=FREE)&&(_Log_Blocks[victimLBN][i]!=INVALID))
        {
            Associated_Blocks.insert(_Log_Blocks[victimLBN][i]/_Pages_Per_Block);
        }
    }
    Associated_Blocks_Num=(long)Associated_Blocks.size();
    
    
    
    
    // markInvalidToSameAddressPages
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if((_Log_Blocks[victimLBN][i]!=FREE)&&(_Log_Blocks[victimLBN][i]!=INVALID))
        {
            markInvalidToSameAddressPages(_Log_Blocks[victimLBN][i]);
        }
    }

    
    
    
    // eraseCount
    //cout<<"Before, the eraseCount is "<<Erase_FAST<<endl;
    Erase_FAST=Erase_FAST+Associated_Blocks_Num; //data erase
    //cout<<"We have "<<Associated_Blocks_Num<<" associated blocks, and we need to do "<<Associated_Blocks_Num<<"(Data blocks)+1(Log block) erases."<<endl;
    //cout<<"We are erasing "<<Associated_Blocks_Num<<" log blocks now."<<endl;
    
    
    
    if (Associated_Blocks_Num==0)
    {
      ZeroAssoFM_FAST++;
    }
    
    AssoDBlk_FAST=AssoDBlk_FAST+Associated_Blocks_Num;

    
    
    
    FMinRLB_FAST++;
    //cout<<"We erase the 1 log block now."<<endl;
    eraseBlock(victimLBN);
    


}

// 18
void FAST_Log_Blocks::markInvalidToSameAddressPages(long pageValue)
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            if(_Log_Blocks[i][j]==pageValue)
                _Log_Blocks[i][j]=INVALID;
        }
    }

}

// 19
long FAST_Log_Blocks::getVictimLBN()
{
    return _currentVictimLBN;
}




// 20
bool FAST_Log_Blocks::isSeqBlock(long LBN)
{
    ////cout<<"Check whether the victimLBN is a seqBlock."<<endl;
    long firstPage=_Log_Blocks[LBN][0];
    
    
    if(   (firstPage<0) ||  (fmod(firstPage,_Pages_Per_Block)!=0)   )
    {
        ////cout<<"Quit[1]."<<endl;
        return false;
    }
    
    for (long i=1; i<_Pages_Per_Block; i++)
    {
        ////cout<<"current page is "<<_Log_Blocks[LBN][i]<<endl;
        if((_Log_Blocks[LBN][i]!=(firstPage+i)))
        {
            ////cout<<"Quit[2]."<<endl;
            return false;
        }
    }
    
    return true;
    
    
}

// 21
bool FAST_Log_Blocks::fullMergeInSEQ(long currentLSN)
{
    eraseBlock(0); // Erasing log block
    Erase_FAST++; // Erasing data block
    FMinSLB_FAST++;
    return true;
}