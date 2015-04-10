#include "FAST.h"

FAST::FAST(fstream & incomingWriteSeqFile) : _Pages_Per_Block(Pages_Per_Block),_RND_Log_Blocks_Num(RND_Log_Blocks_Num_FAST),_SEQ_Log_Blocks_Num(SEQ_Log_Blocks_Num_FAST),
                _RND_Log_Blocks_Fisrt_LBN(RND_Log_Blocks_Fisrt_LBN), _SEQ_Log_Blocks_Fisrt_LBN(SEQ_Log_Blocks_Fisrt_LBN),
                _RND_Log_Blocks(_Pages_Per_Block, _RND_Log_Blocks_Num, _RND_Log_Blocks_Fisrt_LBN),
                _SEQ_Log_Blocks(_Pages_Per_Block, _SEQ_Log_Blocks_Num, _SEQ_Log_Blocks_Fisrt_LBN)
{
     // 01.Initialize the log blocks
       
    // Print the initialized blocks
    //cout<<"=============================================================="<<endl;
    //cout<<"============================FAST=============================="<<endl;
    //cout<<"=============================================================="<<endl;
    //cout<<"The original log blocks are:"<<endl;
    //cout<<"RND_Log_Block:"<<endl;
    _RND_Log_Blocks.readLogBlocks();
    
    //cout<<"SEQ_Log_Block:"<<endl;
    _SEQ_Log_Blocks.readLogBlocks();
   // printConfig(1);
    
    // 02. Run FAST algorithm
    
    //long currentWritePage;
    string currentIOLine;
    while( getline(incomingWriteSeqFile,currentIOLine) )
    {
        // Assume we need to use the log anyway, so we ignore the no-collision case and only write to the log blocks
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl<<"Incoming Write Page is "<<currentWritePage
        //<<". Run FAST:"<<endl;
        // Run FAST
        writeToBlocks(currentIOLine);
        //cout<<"After running, the EraseCount is:"<<Erase_FAST<<endl;
        //cout<<"RND_Log_Block is:"<<endl;
        _RND_Log_Blocks.readLogBlocks();
        //cout<<"SEQ_Log_Block is:"<<endl;
        _SEQ_Log_Blocks.readLogBlocks();
       // printConfig(1);
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    }


    
        



}

void FAST::writeToBlocks (string currentIOLine)
{
    
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

    //
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
    // Case 1, is header
    if (offset==0)
    {
        if (SEQ_Flag==true)
        { CapCrctBlkHdr_FAST++; }
        else
        { CapWrgBlkHdr_FAST++; }
        
        
        //cout<<"Case 1. The input page is a header."<<endl;
        if (_SEQ_Log_Blocks.blockIsFull(_SEQ_Log_Blocks_Fisrt_LBN))  // isFull then switch
        {    // Kick out seq and switch
            //cout<<"Case 1.1 SEQ_Log_Block is full. Switch Merge."<<endl;
            _SEQ_Log_Blocks.switchMerge(0);
        }
        else if (_SEQ_Log_Blocks.blockIsFree(_SEQ_Log_Blocks_Fisrt_LBN)!=true)    // is not full and not free, do merge
        {
            //cout<<"Case 1.2 SEQ_Log_Block is not full and not free. Full Merge."<<endl;
            // Merge the SW log with its data block
            _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
            _SEQ_Log_Blocks.partialMergeForSEQ(0);
        }
        // Write the page to seq log
        //cout<<"Now the SEQ_Log_Block is free. Write input page to SEQ_Log_Block"<<endl;
        _SEQ_Log_Blocks.writePage(0, 0, currentLSN);
        //_SEQ_Log_Blocks.readLogBlocks();
    }
    else
    {
        //cout<<"Case 2. The input page is not a header."<<endl;
        if (_SEQ_Log_Blocks.readPage(0, 0)/_Pages_Per_Block == LBN)
        {
            // input's header is in sw
            //cout<<"Case 2.1 Input's header is in SEQ_Log_block, and then we check whether it is continous to the existing pages the block or not."<<endl;
            long Last_LSN=_SEQ_Log_Blocks.getLastPageOfBlock(0);
            
            // Case 2.1 Perfect case
            if (currentLSN==Last_LSN+1)
            {
                //cout<<"Case 2.1.1 The input page is continous to the existing pages in the block, then append it to the SEQ_Log_Block."<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.appendToBlock(0,currentLSN);
            }
            // Case 2.2 and 2.3
            else
            {
                //cout<<"Case 2.1.2 The input page is not continous to the existing pages in the block, then we merge the SEQ_Log_Block and this input page to the data block."<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.fullMergeInSEQ(currentLSN);
                // also merge the input page
            }
            
        }
        else
        {
            // If the log blocks are full
            //cout<<"Case 2.2 Input's header is not in the SEQ_Log_Block, then insert to the RND_Log_Block"<<endl;
            if (_RND_Log_Blocks.blocksAreFull())
            {
                //cout<<"Kick out one RND block."<<endl;
                long victimLBN = _RND_Log_Blocks.getVictimLBN();
                _RND_Log_Blocks.updateVictimLBN();
                _RND_Log_Blocks.fullMergeForRND(victimLBN); // do not need to check the SEQ_Log_Blocks
                
            }
            // If the log blocks are not full
            _RND_Log_Blocks.appendToBlocks(currentLSN);
            
            
        }
        
        
        
    }
}



void FAST::cleanupLog()
{
    //cout<<"Now kicking out all pages in the log blocks in FAST."<<endl;
    //cout<<"==SEQ_Log_Blocks:=="<<endl;
    // Kick out SEQ_Log_Blocks
    for (long i=0; i<_SEQ_Log_Blocks_Num; i++)
    {
        if (_SEQ_Log_Blocks.blockIsFree(i))
        {
            //cout<<"Block "<<i<<" is free."<<endl;
            continue;
        }
        else if(_SEQ_Log_Blocks.isSeqBlock(i))
        {
            //cout<<"SM_FAST, erase = 1 = data block"<<endl;
            _SEQ_Log_Blocks.switchMerge(i);
                
        }
        else
        {
            //cout<<"PM_FAST, erase = 1 = log block"<<endl;
            _SEQ_Log_Blocks.partialMergeForSEQ(i);
        }
    }
    
    // Kick out RND_Log_Blocks
    //cout<<"==RND_Log_Blocks:=="<<endl;
    for (long i=0; i<_RND_Log_Blocks_Num; i++)
    {
        if (_RND_Log_Blocks.blockIsFree(i))
        {
            //cout<<"Block "<<i<<" is free."<<endl;
            continue;
        }
        else
        {
            //cout<<"fullMerge a log block."<<endl;
            _RND_Log_Blocks.fullMergeForRND(i);
        }
    }
    
    //cout<<"After cleanning up log blocks in FAST:"<<endl;
    //cout<<"RND_Log_Block is:"<<endl;
    _RND_Log_Blocks.readLogBlocks();
    //cout<<"SEQ_Log_Block is:"<<endl;
    _SEQ_Log_Blocks.readLogBlocks();
    
}