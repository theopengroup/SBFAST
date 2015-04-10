#include "BAST.h"


BAST::BAST(fstream & incomingWriteSeqFile) : _Pages_Per_Block(Pages_Per_Block), _Log_Blocks_Num(Log_Blocks_Num_BAST), _Log_Blocks_Fisrt_LBN(Log_Blocks_Fisrt_LBN), _Log_Blocks(_Pages_Per_Block, _Log_Blocks_Num, _Log_Blocks_Fisrt_LBN)
{
    // 01.Initialize the log blocks
    /*_SEQ_Log_Blocks.writePage(0, 0, 24);
     _SEQ_Log_Blocks.writePage(0, 1, 25);
     _SEQ_Log_Blocks.writePage(0, 2, 26);
     
     _RND_Log_Blocks.writePage(0, 0, 1);
     _RND_Log_Blocks.writePage(0, 1, 9);
     _RND_Log_Blocks.writePage(0, 2, 1);
     _RND_Log_Blocks.writePage(0, 3, 9);
     
     _RND_Log_Blocks.writePage(1, 0, 1);
     _RND_Log_Blocks.writePage(1, 1, 9);*/
    //  _RND_Log_Blocks.writePage(1, 2, 1);
    
    /*
     _RND_Log_Blocks.writePage(2, 0, 1);
     _RND_Log_Blocks.writePage(2, 1, 9);
     _RND_Log_Blocks.writePage(2, 2, 1);
     
     _RND_Log_Blocks.writePage(3, 0, 1);
     _RND_Log_Blocks.writePage(3, 1, 9);
     _RND_Log_Blocks.writePage(3, 2, 1);*/
    
    // Print the initialized blocks
    //cout<<"=============================================================="<<endl;
    //cout<<"============================BAST=============================="<<endl;
    //cout<<"=============================================================="<<endl;
    //cout<<"The original log blocks are:"<<endl;
    //cout<<"Log_Block:"<<endl;
    //_Log_Blocks.readLogBlocks();
    

    //printConfig(0);
    
    // 02. Run BAST algorithm
    //long currentWritePage;
    string currentIOLine;
        
    //while (incomingWriteSeqFile>>currentIOLine)
    while( getline(incomingWriteSeqFile,currentIOLine) )
    {
        //cout<<"==currentIOLine="<<currentIOLine<<endl;
        // Assume we need to use the log anyway, so we ignore the no-collision case and only write to the log blocks
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl<<"Incoming Write Page is "<<currentWritePage
       // <<". Run BAST:"<<endl;
        // Run BAST
        writeToBlocks(currentIOLine);
        //cout<<"After running, the EraseCount is:"<<Erase_BAST<<endl;
        //cout<<"Log_Block is:"<<endl;
        _Log_Blocks.readLogBlocks();
       // printConfig(0);
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    }
    
    
    
    
    
    
    
}

void BAST::writeToBlocks (string currentIOLine)
{
    //cout<<"currentIOLine="<<currentIOLine<<endl;
    string traceStreamIDStr;
    string currentLSNStr;
    for (int i=0;i<currentIOLine.length();i++)
    {
        if (currentIOLine[i]==' ')
        {
            // rand page
            traceStreamIDStr=currentIOLine.substr(0,i);
            currentLSNStr=currentIOLine.substr(i+1);
            break;
        }

    }

    const char* traceStreamIDChar = traceStreamIDStr.c_str();
    long traceStreamID = atol(traceStreamIDChar);
    
    const char* currentLSNChar = currentLSNStr.c_str();
    long currentLSN = atol(currentLSNChar);
    
    //cout<<"===TraceID==="<<traceStreamID<<"===pageAddr==="<<currentLSN<<endl;
    
    
    
    bool SEQ_Flag=true;
    if (traceStreamID==-1)
    {
        SEQ_Flag=false;
        currentLSN=-currentLSN;
    }
    long LBN=(currentLSN / (int)_Pages_Per_Block);
    //long offset=currentLSN % _Pages_Per_Block;
    double offset=fmod(currentLSN, _Pages_Per_Block);
    //cout<<"Offset="<<offset<<", LBN="<<LBN<<endl;
    
    long blockWithSameDataLBNInLogBlock = _Log_Blocks.findBlockWithSameDataLBN(LBN);
    // Case 1. if its corresponding data block is in the Log_Blocks, try to insert to that block if possible or merge together with that block
    if (_Log_Blocks.blocksAreFree())
    {
        //cout<<"Case 0. blocks are all free, then directly insert the input page to the log blocks"<<endl;
        _Log_Blocks.writePage(0, 0, currentLSN);
    }
    else if (blockWithSameDataLBNInLogBlock!=NOT_FIND)
    {
        //cout<<"Case 1. Corresponding data block is in the Log_Blocks."<<endl;
        if (_Log_Blocks.blockIsFull(blockWithSameDataLBNInLogBlock)==true)
        {
            //cout<<"Case 1.1 Corresponding data block is full, we are now merging it with the data block and the input page."<<endl;
            if(_Log_Blocks.isSeqBlock(blockWithSameDataLBNInLogBlock))
            {
                //cout<<"SM_BAST, erase = 1 = data block"<<endl;
                _Log_Blocks.switchMerge(blockWithSameDataLBNInLogBlock);

            }
            else
            {
                //cout<<"FM_BAST, erase = 2 = data block + log block"<<endl;
                _Log_Blocks.merge(blockWithSameDataLBNInLogBlock);
            }

            


        }
        else
        {
            //cout<<"Case 1.2 Corresponding data block is not full, we just append the input page to this block."<<endl;
            _Log_Blocks.appendToBlock(blockWithSameDataLBNInLogBlock, currentLSN);
        }
    }
    // Case 2. if its corresponding data block is not in the Log_Blocks, try to insert is find a free block or merge a victim block and then insert
    else
    {
        //cout<<"Case 2. Corresponding data block is not in the Log_Blocks."<<endl;
        long freeBlock = _Log_Blocks.findFreeLBN();
        //cout<<"=========find free block is "<<freeBlock<<endl;
        if (freeBlock==NOT_FIND)
        {
            //cout<<"Case 2.1 No free block, then merge out a victim block in the log block."<<endl;
            long victimBlock=_Log_Blocks.getVictimLBN();
            _Log_Blocks.updateVictimLBN();
            //cout<<"============victimBlock is "<<victimBlock<<endl;
            if(_Log_Blocks.isSeqBlock(victimBlock))
            {
                //cout<<"SM_BAST, erase = 1 = data block"<<endl;
                _Log_Blocks.switchMerge(victimBlock);

            }
            else
            {
                //cout<<"FM_BAST, erase = 2 = data block + log block"<<endl;
                _Log_Blocks.merge(victimBlock);

            }


            _Log_Blocks.appendToBlock(victimBlock, currentLSN);
            
        }
        else
        {
            //cout<<"Case 2.2 Find a free block, then insert the input page to that block."<<endl;
            _Log_Blocks.appendToBlock(freeBlock, currentLSN);
        }
    }

}


void BAST::cleanupLog()
{
    //cout<<"Now kicking out all pages in the log blocks in BAST."<<endl;
    for (long i=0; i<_Log_Blocks_Num; i++)
    {
        if (_Log_Blocks.blockIsFree(i))
        {
            //cout<<"Block "<<i<<" is free."<<endl;
            continue;
        }
        else if(_Log_Blocks.isSeqBlock(i))
        {
            //cout<<"SM_BAST, erase = 1 = data block"<<endl;
            _Log_Blocks.switchMerge(i);
            CapCrctStrm_BAST++;
        }
        else
        {
            //cout<<"FM_BAST, erase = 2 = data block + log block"<<endl;
            _Log_Blocks.merge(i);
        }


    }
    //cout<<"After cleanning up log blocks in BAST:"<<endl;
    _Log_Blocks.readLogBlocks();
}
