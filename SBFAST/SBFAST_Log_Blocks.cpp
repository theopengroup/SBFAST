#include "SBFAST_Log_Blocks.h"
// 01
SBFAST_Log_Blocks::SBFAST_Log_Blocks(long Pages_Per_Block, long Log_Blocks_Num, long FirstLBN):_Pages_Per_Block(Pages_Per_Block),_Log_Blocks_Num(Log_Blocks_Num), _First_LBN(FirstLBN)
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
void SBFAST_Log_Blocks::eraseBlock(long LBN)
{
    //cout<<"Erasing a block now, before that the eraseCount is "<<Erase_SBFAST<<endl;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        _Log_Blocks[LBN][i]=FREE;
        
    }
    Erase_SBFAST++;
    //cout<<"After erasing, the eraseCount is "<<Erase_SBFAST<<endl;
}

// 03
vector<long> SBFAST_Log_Blocks::readBlock(long LBN)
{
    return _Log_Blocks[LBN];
}




// 04
long SBFAST_Log_Blocks::readPage(long LBN, long offset)
{
    return _Log_Blocks[LBN][offset];
}


// 05-A
bool SBFAST_Log_Blocks::writePage(long LBN,long offset, long pageValue)
{
    if (_Log_Blocks[LBN][offset]==FREE)
    {
        //cout<<"The position is free, we check whether the pageValue has already been inputted before."<<endl;
        markInvalidToSameAddressPages(pageValue);
        //cout<<""<<endl;
        _Log_Blocks[LBN][offset]=pageValue;
        return true;
    }
    else
        return false;
}

// 05-B
bool SBFAST_Log_Blocks::writePageDef(long LBN,long offset, long pageValue)
{
    _Log_Blocks[LBN][offset]=pageValue;
    return false;
}




// programming purpose
// 06
vector<vector<long>> SBFAST_Log_Blocks::readLogBlocks()
{
    
    for (long column=0; column<_Log_Blocks_Num; column++)
    {
        //cout<<right<<setfill(' ')<<setw(19)<<"LBN"<<_First_LBN+column;
        
    }
  //  cout<<endl;
    
    
    for (long row=0; row<_Pages_Per_Block; row++)
    {
        for (long column=0; column<_Log_Blocks_Num; column++)
        {
            if(_Log_Blocks[column][row]==FREE)
            {
            //    cout<<right<<setfill(' ')<<setw(20)<<"FREE";
            }
            else if(_Log_Blocks[column][row]==INVALID)
            {
             //   cout<<right<<setfill(' ')<<setw(20)<<"INVALID";
            }
            ////cout<<"  |  "<<_Log_Blocks[column][row]<<"  |  ";
            else
            {// cout<<right<<setfill(' ')<<setw(20)<<_Log_Blocks[column][row];
            }
            
        }
        //cout<<endl;
    }
    return _Log_Blocks;
}



// 07
bool SBFAST_Log_Blocks::blockIsFree (long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]!=FREE)
            return false;
    }
    return true;
}


// 08
bool SBFAST_Log_Blocks::blocksAreFree ()
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
bool SBFAST_Log_Blocks::blockIsFull(long LBN)
{
    
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]==FREE)
            return false;
    }
    return true;
}

// 10
bool SBFAST_Log_Blocks::blocksAreFull ()
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
long SBFAST_Log_Blocks::getFirstPageOfBlock(long LBN)
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
void  SBFAST_Log_Blocks::appendToBlock(long LBN, long pageValue)
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
bool SBFAST_Log_Blocks::appendToBlocks(long pageValue)
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
    //cout<<"The whole log blocks are full and we cannot append anything inside. We need to kick out one of them by FIFO order. And the victim is "<<_currentVictimLBN<<endl;

    
}




// 15 modify here========<modify
void SBFAST_Log_Blocks::partialMergeForSEQ (long newestBlockWithSameDataLBNInSEQ, vector<long> blocksWithSameDataLBNInSEQ)
{
    //cout<<"Partial Merge For SEQ Log, Erase = 1 For Data."<<endl;
    eraseBlock(newestBlockWithSameDataLBNInSEQ); // We do "Log erase", but in fact we are going to erase "data log"
    PM_SBFAST++;
    updateVictimLBN();
}

// 16
long SBFAST_Log_Blocks::updateVictimLBN()
{
    if ((_currentVictimLBN+1)<=(_First_LBN+_Log_Blocks_Num-1))
        _currentVictimLBN++;
    else
        _currentVictimLBN=_First_LBN;
    
    //cout<<"============We are updating the victimLBNnum now, and after that it is "<<_currentVictimLBN<<endl;
    return _currentVictimLBN;
    
}
// 17
void SBFAST_Log_Blocks::fullMergeForRND(long victimLBN, long pageValue)
{
    // Calculate the number of associated blocks
    long inputPageNeedToBeMerge=0;
    long Associated_Blocks_Num=0;
    set<long> Associated_Blocks;
    for (long i=0; i<_Pages_Per_Block; i++)
    {
        if((_Log_Blocks[victimLBN][i]!=FREE)&&(_Log_Blocks[victimLBN][i]!=INVALID))
        {
            Associated_Blocks.insert(_Log_Blocks[victimLBN][i]/_Pages_Per_Block);
        }
        if (_Log_Blocks[victimLBN][i]==pageValue)
        {
            inputPageNeedToBeMerge=1;
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
    //cout<<"Before, the eraseCount is "<<Erase_SBFAST<<endl;
    Erase_SBFAST=Erase_SBFAST+Associated_Blocks_Num; //data erase
    //cout<<"We have "<<Associated_Blocks_Num<<" associated blocks, and we need to do "<<Associated_Blocks_Num<<"(Data blocks)+1(Log block) erases."<<endl;
    //cout<<"We are erasing "<<Associated_Blocks_Num<<" log blocks now."<<endl;

    FMinRLB_SBFAST+=Associated_Blocks_Num;
    //cout<<"We erase the 1 log block now."<<endl;
    eraseBlock(victimLBN);
    //cout<<"fullMergeForRND calls updateVictim()."<<endl;
    updateVictimLBN();
    
    // Append the input page to the new block if it is not in the erased victim block
    if (inputPageNeedToBeMerge!=1)
    {
        writePage(victimLBN, 0, pageValue);
        //_Log_Blocks[victimLBN][0]=pageValue;
    }
}





// 18
bool SBFAST_Log_Blocks::markInvalidToSameAddressPages(long pageValue)
{
   // cout<<"PageValue="<<pageValue<<endl;
    bool dupFlag=false;
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            if(_Log_Blocks[i][j]==pageValue)
            {
                dupFlag=true;
                //cout<<"Marking INVALID to _Log_Blocks["<<i<<"]["<<j<<"]"<<endl;
                _Log_Blocks[i][j]=INVALID;
            }
        }
    }
    return dupFlag;
}

// 19
long SBFAST_Log_Blocks::getVictimLBN()
{
    return _currentVictimLBN;
}

// 20-A
// Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page currentLSN. It returns the 'newest' block amomg them. Otherwise (no 'newest' block = not found), it will return NOT_FOUND.
long SBFAST_Log_Blocks::findNewestBlockWithSameDataLBNInSEQ(long LBN, vector<long> &BlocksWithSameLBN)
{
    //cout<<"--------In side of the findNewestBlockWithSameDataLBNInSEQ()-A-------"<<endl;
    //cout<<"The input LBN is "<<LBN<<"."<<endl;
    
    long newestBlock=NOT_FIND;
    


    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        //cout<<"Now searching in the "<<i<<"th block in the SEQ_Log_Blocks."<<endl;
        
        
        if (getFirstPageOfBlock(i)==NO_VALID_DATA)
        {
            //cout<<"The "<<i<<"th block has no valid data. Check next block."<<endl;
            continue;
        }
        
        long currentBlockDataLBN=getFirstPageOfBlock(i)/_Pages_Per_Block;
        //cout<<"The "<<i<<"th SEQ_Log_Block's data LBN is "<<currentBlockDataLBN<<endl;
        
        // Calculate LBN of ith Log_Block
        if(currentBlockDataLBN==LBN)
        {
            //cout<<"This block has the same LBN with the input LBN."<<endl;
            // Save this found "same LBN" block to the vector.
            BlocksWithSameLBN.push_back(i);
            
            //cout<<"Now searching for any 'INVALID' labels in this block to see whether it is the newest one:"<<endl;
            bool isNewestBlock=true;
            for (int j=0; j<_Pages_Per_Block; j++)
            {
                if(_Log_Blocks[i][j]==INVALID)
                {
                    //cout<<"An INVALID page is found in this block, so this block is not the newest one for the input LBN. Check the next block."<<endl;
                    isNewestBlock=false;
                    break;
                }
                
            }
            // No way to be more than one newest blocks. So when we find the "first" newest block, we can save it to the "newestBlock" flag and don't need to worry about it will be rewritten later.
            // We still need to scan all the blocks in order to save all "same-LBN" blocks.
            if (isNewestBlock==true)
            {
                //cout<<"This block does not contain any invalid pages, thus it is the newestBlock."<<endl;
                newestBlock=i;
            }
        }
        
    }
    //cout<<"--------End of the findNewestBlockWithSameDataLBNInSEQ()-A-------"<<endl;
    return newestBlock;
    
}





// 20-B
// Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page currentLSN. It returns the 'newest' block amomg them. Otherwise (no 'newest' block = not found), it will return NOT_FOUND.
long SBFAST_Log_Blocks::findNewestBlockWithSameDataLBNInSEQ(long LBN)
{
    //cout<<"--------In side of the findNewestBlockWithSameDataLBNInSEQ()-B-------"<<endl;
    //cout<<"The input LBN is "<<LBN<<"."<<endl;
    
    long newestBlock=NOT_FIND;
    
    
    
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        //cout<<"Now searching in the "<<i<<"th block in the SEQ_Log_Blocks."<<endl;
        long currentBlockDataLBN=getFirstPageOfBlock(i)/_Pages_Per_Block;
        //cout<<"The "<<i<<"th SEQ_Log_Block's data LBN is "<<currentBlockDataLBN<<endl;
        
        // Calculate LBN of ith Log_Block
        if(currentBlockDataLBN==LBN)
        {
            //cout<<"This block has the same LBN with the input LBN."<<endl;
            
            //cout<<"Now searching for any 'INVALID' labels in this block to see whether it is the newest one:"<<endl;
            bool isNewestBlock=true;
            for (int j=0; j<_Pages_Per_Block; j++)
            {
                if(_Log_Blocks[i][j]==INVALID)
                {
                    //cout<<"An INVALID page is found in this block, so this block is not the newest one for the input LBN. Check the next block."<<endl;
                    isNewestBlock=false;
                    break;
                }
                
            }
            // No way to be more than one newest blocks. So when we find the "first" newest block, we can save it to the "newestBlock" flag and don't need to worry about it will be rewritten later.
            // We still need to scan all the blocks in order to save all "same-LBN" blocks.
            if (isNewestBlock==true)
            {
                newestBlock=i;
            }
        }
        
    }
    //cout<<"--------End of the findNewestBlockWithSameDataLBNInSEQ()-B-------"<<endl;
    return newestBlock;
    
}







// 21
long SBFAST_Log_Blocks::findFreeLBN()
{
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        if(blockIsFree(i)==true)
            return i;
    }
    
    return NOT_FIND;
    
}

// 22
bool SBFAST_Log_Blocks::markInvalidToSameAddressPagesAndLockBlock(long pageValue)
{
    bool lockFlag=false;
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        for (long j=0; j<_Pages_Per_Block; j++)
        {
            if(_Log_Blocks[i][j]==pageValue)
            {
                lockFlag=true;
                _Log_Blocks[i][j]=INVALID;
            }
        }
    }
    return lockFlag;
}


// 23
bool SBFAST_Log_Blocks::checkValidBlock(long LBN)
{
    for (int i=0; i<_Pages_Per_Block; i++)
    {
        if (_Log_Blocks[LBN][i]==INVALID)
        {
                return false;
    
        }
    }
    return true;
}


// 24
bool SBFAST_Log_Blocks::isSeqBlock(long LBN)
{
    //cout<<"================================================:"<<endl;
   //cout<<"Check whether the LBN is a seqBlock."<<endl;
    long firstPage=_Log_Blocks[LBN][0];
    //cout<<firstPage<<endl;
    if(   (firstPage<0) ||  (fmod(firstPage,_Pages_Per_Block)!=0)   )
    {
        
        //cout<<"Quit[1]."<<endl;
        return false;
    }
    
    for (long i=1; i<_Pages_Per_Block; i++)
    {
        //cout<<"current page is "<<_Log_Blocks[LBN][i]<<endl;
        if((_Log_Blocks[LBN][i]!=(firstPage+i)))
        {
            //cout<<"================================================:"<<_Log_Blocks[LBN][i]<<endl;
            //cout<<"Quit[2]."<<endl;
            return false;
        }
    }
    
    return true;
    
    
}

// 25
bool SBFAST_Log_Blocks::blockHasInvalidPage(long LBN)
{
    
    ////cout<<"Check whether the LBN has a invalid page."<<endl;
    long firstPage=_Log_Blocks[LBN][0];
    for (long i=1; i<_Pages_Per_Block; i++)
    {
        if(_Log_Blocks[LBN][i]==INVALID)
        {
            return true;
        }
    }
    return false;
}