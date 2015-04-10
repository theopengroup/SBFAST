#include "BAST_Log_Blocks.h"
// 01
BAST_Log_Blocks::BAST_Log_Blocks(long Pages_Per_Block, long Log_Blocks_Num, long FirstLBN):_Pages_Per_Block(Pages_Per_Block),_Log_Blocks_Num(Log_Blocks_Num), _First_LBN(FirstLBN)
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
void BAST_Log_Blocks::eraseBlock(long LBN)
{
    //cout<<"Erasing a block now, and before erasing, the eraseCount is "<<Erase_BAST<<endl;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        _Log_Blocks[LBN][i]=FREE;
        
    }
    Erase_BAST++;
    //cout<<"After erasing, the eraseCount is "<<Erase_BAST<<endl;
}

// 03
vector<long> BAST_Log_Blocks::readBlock(long LBN)
{
    return _Log_Blocks[LBN];
}




// 04
long BAST_Log_Blocks::readPage(long LBN, long offset)
{
    return _Log_Blocks[LBN][offset];
}


// 05
bool BAST_Log_Blocks::writePage(long LBN,long offset, long pageValue)
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
vector<vector<long>> BAST_Log_Blocks::readLogBlocks()
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
bool BAST_Log_Blocks::blockIsFree (long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]!=FREE)
            return false;
    }
    return true;
}


// 08
bool BAST_Log_Blocks::blocksAreFree ()
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
bool BAST_Log_Blocks::blockIsFull(long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]==FREE)
            return false;
    }
    return true;
}

// 10
bool BAST_Log_Blocks::blocksAreFull ()
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
long BAST_Log_Blocks::getFirstPageOfBlock(long LBN)
{
    ////cout<<"======getFirstPageOfBlock======="<<endl;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        
        ////cout<<"======current="<<_Log_Blocks[LBN][i]<<endl;
        if ((_Log_Blocks[LBN][i]!=FREE)&&(_Log_Blocks[LBN][i]!=INVALID))
        {
            return _Log_Blocks[LBN][i];
        }
    }
    
    return NO_VALID_DATA;
}



// 12
void  BAST_Log_Blocks::appendToBlock(long LBN, long pageValue)
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

                writePage(LBN, i, pageValue);
                //_Log_Blocks[LBN][i]=pageValue;
                break;
            }
        }
    }
    
}





// 13
bool  BAST_Log_Blocks::appendToBlocks(long pageValue)
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
void  BAST_Log_Blocks::switchMerge (long Victim_Block_Num)
{
    //cout<<"Switch Merge = 1 Erase for Data."<<endl;
    eraseBlock(Victim_Block_Num);
    SM_BAST++;
    //cout<<"switchMerge calls updateVictim()."<<endl;
}



// 15
void  BAST_Log_Blocks::merge (long Victim_Block_Num)
{
    //cout<<"Merge: Erase = 2 = 1 Data + 1 Log."<<endl;
    Erase_BAST++;
    FM_BAST++;
    eraseBlock(Victim_Block_Num);
}



// 16
long BAST_Log_Blocks::updateVictimLBN()
{
    if ((_currentVictimLBN+1)<=(_First_LBN+_Log_Blocks_Num-1))
        _currentVictimLBN++;
    else
        _currentVictimLBN=_First_LBN;
    
    //cout<<"We are updating the victimLBNnum now, and after that it is "<<_currentVictimLBN<<endl;
    return _currentVictimLBN;
    
}
// 17
void BAST_Log_Blocks::fullMergeForRND(long victimLBN)
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
    //cout<<"Before, the eraseCount is "<<Erase_BAST<<endl;
    Erase_BAST=Erase_BAST+Associated_Blocks_Num; //data erase
    //cout<<"We have "<<Associated_Blocks_Num<<" associated blocks, and we need to do "<<Associated_Blocks_Num<<"(Data blocks)+1(Log block) erases."<<endl;
    //cout<<"We are erasing "<<Associated_Blocks_Num<<" log blocks now."<<endl;
    FM_BAST+=Associated_Blocks_Num;
    //cout<<"We erase the 1 log block now."<<endl;
    eraseBlock(victimLBN);
    
    //cout<<"fullMergeForRND calls updateVictim()."<<endl;
    updateVictimLBN();
    
    
    
}

// 18
void BAST_Log_Blocks::markInvalidToSameAddressPages(long pageValue)
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
long BAST_Log_Blocks::getVictimLBN()
{
    return _currentVictimLBN;
}






// 20
// Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page currentLSN. It returns the 'newest' block amomg them. Otherwise (no 'newest' block = not found), it will return NOT_FOUND.
long BAST_Log_Blocks::findBlockWithSameDataLBN(long LBN)
{
    //cout<<"--------In side of the findBlockWithSameDataLBN()-------"<<endl;
    //cout<<"The input LBN is "<<LBN<<"."<<endl;
    
    long newestBlock=NOT_FIND;
    
    
    
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        //cout<<"Now searching in the "<<i<<"th block in the Log_Blocks."<<endl;
        long firstPageOfBlock=getFirstPageOfBlock(i);
        //cout<<"firstPageOfBlock="<<firstPageOfBlock<<endl;
        if (firstPageOfBlock==NO_VALID_DATA)
        {
            //cout<<"The "<<i<<"th block has no valid data. Check next block."<<endl;
            continue;
        }
        long currentBlockDataLBN=firstPageOfBlock/_Pages_Per_Block;
        //cout<<"The "<<i<<"th Log_Block's data LBN is "<<currentBlockDataLBN<<endl;
        
        // Calculate LBN of ith Log_Block
        if(currentBlockDataLBN==LBN)
        {
            return i;
        }
        
    }
    //cout<<"--------End of the findBlockWithSameDataLBN()-------"<<endl;
    //cout<<"Returns: "<<newestBlock<<endl;
    return newestBlock;
    
}







// 21
long BAST_Log_Blocks::findFreeLBN()
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        if(blockIsFree(i)==true)
            return i;
    }
    
    return NOT_FIND;
    
}

// 22
bool BAST_Log_Blocks::isSeqBlock(long LBN)
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
        if(_Log_Blocks[LBN][i]==FREE)
        {

            return false;
        }
        else if((_Log_Blocks[LBN][i]!=(firstPage+i)))
        {
            ////cout<<"Quit[2]."<<endl;
            return false;
        }
    }
    
    return true;


}

