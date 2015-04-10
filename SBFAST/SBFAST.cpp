#include "SBFAST.h"

// 1
SBFAST::SBFAST(fstream & incomingWriteSeqFile) : _Pages_Per_Block(Pages_Per_Block),_Pages_Per_Subblock(Pages_Per_Subblock),_RND_Log_Blocks_Num(RND_Log_Blocks_Num_SBFAST),_SEQ_Log_Blocks_Num(SEQ_Log_Blocks_Num_SBFAST),
_RND_Log_Blocks_Fisrt_LBN(RND_Log_Blocks_Fisrt_LBN), _SEQ_Log_Blocks_Fisrt_LBN(SEQ_Log_Blocks_Fisrt_LBN),
_RND_Log_Blocks(_Pages_Per_Block, _RND_Log_Blocks_Num, _RND_Log_Blocks_Fisrt_LBN),
_SEQ_Log_Blocks(_Pages_Per_Block, _SEQ_Log_Blocks_Num, _SEQ_Log_Blocks_Fisrt_LBN)
{

    
    // Print the initialized blocks
    //cout<<"=============================================================="<<endl;
    //cout<<"==========================SBFAST============================"<<endl;
    //cout<<"=============================================================="<<endl;
    //cout<<"The original log blocks are:"<<endl;
    // cout<<"RND_Log_Block:"<<endl;
    _RND_Log_Blocks.readLogBlocks();
    
    // cout<<"SEQ_Log_Block:"<<endl;
    _SEQ_Log_Blocks.readLogBlocks();
    //printConfig(2);
    //cout<<"Pages_Per_Subblock="<<Pages_Per_Subblock<<endl;
    
    //cout<<"_Pages_Per_Subblock="<<_Pages_Per_Subblock<<endl;
    
    
    
    // 02. Run FAST algorithm
    string currentIOLine;
    while( getline(incomingWriteSeqFile,currentIOLine) )
    {
        
        // Assume we need to use the log anyway, so we ignore the no-collision case and only write to the log blocks
        //cout<<"---------------------------------"<<endl<<"Incoming Write Page is "<<currentWritePage<<". Run SBFAST:"<<endl;
        
        // Run FAST
        writeToBlocks(currentIOLine);
        //cout<<"After running, the EraseCount is:"<<Erase_SBFAST<<endl;
        //cout<<"RND_Log_Block is:"<<endl;
        //_RND_Log_Blocks.readLogBlocks();
        //cout<<"SEQ_Log_Block is:"<<endl;
        //_SEQ_Log_Blocks.readLogBlocks();
        // printConfig(2);
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    }
    
    
    
    
    
    
}


// 2
void SBFAST::writeToBlocks (string currentIOLine)
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

    
    //cout<<"traceStreamID="<<traceStreamID<<", PAge="<<currentLSN<<endl;

    
    
    
    
    
    bool SEQ_Flag=true;
    if (traceStreamID==-1)
    {
        // cout<<"convert"<<endl;
        SEQ_Flag=false;
        currentLSN=-currentLSN;
    }
    
    
    /*!
     Calculate (sub)block number and (sub)block offset for the input currentLSN
     */
    // Block header number
    long LBN=currentLSN / _Pages_Per_Block;
    // Block offset
    long offset=fmod(currentLSN, _Pages_Per_Block);
    // Print header LBN and Offset
    //cout<<"Block number="<<LBN<<", Offset="<<offset<<endl;
    
    // Subblock Number
    long LSBN= currentLSN / _Pages_Per_Subblock;
    // Subblock offest
    long offsetSubblock = fmod(currentLSN, _Pages_Per_Subblock);
    // Print header LBN and Offset
    //cout<<"Subblock Number="<<LSBN<<", OffsetInSubblock="<<offsetSubblock<<endl;
    
    
    /*!  For later use. Find a free block in SEQ_Log_Blocks.      */
    long freeBlockInSEQ =_SEQ_Log_Blocks.findFreeLBN();
    
    vector<long> blocksWithSameDataLBNInSEQ;
    
    
    // Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page currentLSN. It returns the 'newest' block amomg them. Otherwise (no 'newest' block = not found), it will return NOT_FOUND.
    long newestBlockWithSameDataLBNInSEQ=_SEQ_Log_Blocks.findNewestBlockWithSameDataLBNInSEQ(LBN, blocksWithSameDataLBNInSEQ);
    
    //cout<<"_Pages_Per_Subblock="<<_Pages_Per_Subblock<<endl;
    
    
    //cout<<"=============Printing blocksWithSameDataLBNInSEQ vector================"<<endl;
    printVector(blocksWithSameDataLBNInSEQ);
    //cout<<"offsetSubblock="<<offsetSubblock<<endl;
    
    
    /*! Case 1, is block header or subblock header, do in FAST-like way      */
    if (offsetSubblock==0)
    {
        
        if (SEQ_Flag)
        {
            // seq page
            //cout<<"traceStreamID="<<traceStreamID<<endl;
            //cout<<"currentLSN="<<currentLSN<<endl;
            traceSeqStreamCapPagesCounter[traceStreamID]++;
        }
        
        
        if (SEQ_Flag==true)
        {
            CapCrctStrmSubHdr_SBFAST++;
        }
        else
        {
            CapWrgStrmSubHdr_SBFAST++;
        }
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
        //cout<<"Case 1. The input page currentLSN is a block header or a subblock header. We will try to insert it into the SEQ_Log_Blocks."<<endl;
        //cout<<"Checking the SEQ_Log_Blocks."<<endl;
        
        /*! Case 1.1 SEQ_Log_Blocks are free, then directly insert. */
        if (_SEQ_Log_Blocks.blocksAreFree())
        {
            //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
            //cout<<"Case 1.1 The SEQ_Log_Blocks are free, then directly write the input page currentLSN to the first block of SEQ_Log_Blocks."<<endl;
            _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
            _SEQ_Log_Blocks.writePageDef(0, offset, currentLSN);
        }
        
        
        /*! Case 1.2 SEQ_Log_Blocks are not all free, and input currentLSN's LBN is in the SEQ_Log_Blocks, we try to (1) directly insert or (2) partial merge and then insert. */
        else if (newestBlockWithSameDataLBNInSEQ!=NOT_FIND)
        {
            //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
            //cout<<"Case 1.2 SEQ_Log_Blocks are not all free, and input currentLSN's LBN (block header) is in the SEQ_Log_Blocks, we try to (1) directly insert or (2) partial merge and then insert."<<endl;
            //cout<<"Check whether the position in the newestBlockWithSameDataLBNInSEQ is available."<<endl;
            
            // Case 1.2.1 The position in the newest 'same LBN' block is available, then directly insert the currentLSN to the newest LBN block.
            if(_SEQ_Log_Blocks.readPage(newestBlockWithSameDataLBNInSEQ, offset)==FREE)
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 1.2.1 The position in the newestBlockWithSameDataLBNInSEQ is available, then directly insert the currentLSN to the newest LBN block."<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                _SEQ_Log_Blocks.writePageDef(newestBlockWithSameDataLBNInSEQ, offset, currentLSN);
            }
            
            // Case 1.2.2 The position in the newest 'same LBN' block is not available, then directly insert the currentLSN to the newest LBN block.
            else if(_SEQ_Log_Blocks.readPage(newestBlockWithSameDataLBNInSEQ, offset)!=FREE)
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 1.2.2 The position in the newestBlockWithSameDataLBNInSEQ is not available, then partial merge and insert."<<endl;
                //cout<<"Searching for a free block in SEQ_Log_Blocks."<<endl;
                if (freeBlockInSEQ!=NOT_FIND)
                {
                    //cout<<"Case 1.2.2.1 SEQ_Log_Blocks have a free block, then directly insert."<<endl;
                    _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                    _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                    _SEQ_Log_Blocks.writePageDef(freeBlockInSEQ, offset, currentLSN);
                    
                }
                else
                {
                    //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                    //cout<<"Case 1.2.2.2 SEQ_Log_Blocks do not have any free blocks, then partial merge the 'newest same LBN block' and insert."<<endl;
                    
                    // Partial merge the newestBlockWithSameDataLBNInSEQ
                    //cout<<"Partial merge the newestBlockWithSameDataLBNInSEQ whose LBN is "<<newestBlockWithSameDataLBNInSEQ<<endl;
                    partialMergeNewestSameLBNBlockInSEQ(newestBlockWithSameDataLBNInSEQ, blocksWithSameDataLBNInSEQ);
                    // We have a free block now
                    //cout<<"Now insert to the free block 'newestBlockWithSameDataLBNInSEQ'"<<endl;
                    _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                    _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN); //Just in case RND has same pages with currentLSN
                    _SEQ_Log_Blocks.writePage(newestBlockWithSameDataLBNInSEQ, offset, currentLSN); // insert to the free block "newestBlockWithSameDataLBNInSEQ"
                }
            }
        }
        
        
        /*! Case 1.3 SEQ_Log_Blocks are not free, and input currentLSN's LBN is not in the SEQ_Log_Blocks, we try to (1) directly insert or (2) partial merge and then insert. */
        else if (newestBlockWithSameDataLBNInSEQ==NOT_FIND)
        {
            //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
            //cout<<"Case 1.3 SEQ_Log_Blocks are not all free, and input currentLSN's LBN is not in the SEQ_Log_Blocks, we try to (1) directly insert or (2) partial merge and then insert."<<endl;
            //cout<<"Searching for a free block in SEQ_Log_Blocks."<<endl;
            if (freeBlockInSEQ!=NOT_FIND)
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 1.3.1 SEQ_Log_Blocks have a free block, then directly insert."<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                _SEQ_Log_Blocks.writePageDef(freeBlockInSEQ, offset, currentLSN);
            }
            else
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 1.3.2 SEQ_Log_Blocks do not have any free blocks, then partial merge the victim block and insert."<<endl;
                
                // Preparation
                
                // Get a victim by FIFO
                long victimBlock = _SEQ_Log_Blocks.getVictimLBN();
                //cout<<"The victimBlock we get is "<<victimBlock<<"th block."<<endl;
                //cout<<"After getting the current victimBlockLBN in SEQ_Log_Blocks, we immediately update the victimBlockLBN in SEQ_Log_Blocks."<<endl;
                _SEQ_Log_Blocks.updateVictimLBN();
                
                // Get the first valid page of it. If no valid page, it will return NOT_VALID_DATA.
                // Since there are not free blocks in the SEQ_Log_Blocks, so it returns a valid page or NOT_VALID_DATA (the block is useless).
                long victimBlockFirstPage=_SEQ_Log_Blocks.getFirstPageOfBlock(victimBlock);
                //cout<<"victimBlockFirstPage="<<victimBlockFirstPage<<endl;
                
                
                long victimBlockCorrespondingDataLBN=victimBlockFirstPage/_Pages_Per_Block;
                //cout<<"victimBlockCorrespondingDataLBN="<<victimBlockCorrespondingDataLBN<<endl;
                
                
                
                
                //cout<<"Checking whether the victimBlock is a newestBlock or not:"<<endl;
                // Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page victimLBN.
                vector<long> victimSameDataLBNBlocksInSED;
                long victimBlockCorrespondingDataLBNNewestBlockInSEQ = _SEQ_Log_Blocks.findNewestBlockWithSameDataLBNInSEQ(victimBlockCorrespondingDataLBN, victimSameDataLBNBlocksInSED);
                
                
                //cout<<"=============Printing victimSameDataLBNBlocksInSED vector================"<<endl;
                printVector(victimSameDataLBNBlocksInSED);
                
                
                //  Case 1.3.2.1 If the victim block is the newestBlock, then it cannot have any invalid data pages.
                if (_SEQ_Log_Blocks.checkValidBlock(victimBlock)==true)
                {
                    //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                    //cout<<"Case 1.3.2.1 The victim block is a newestBlock, then partial merge."<<endl;
                    //cout<<"The victim block has no invalid data which means its a newBlock. We partial merge it."<<endl;
                    //cout<<"victimBlock="<<victimBlock<<endl;
                    partialMergeNewestSameLBNBlockInSEQ(victimBlock, victimSameDataLBNBlocksInSED);
                    
                }
                
                
                // Case 1.3.2.2 if the victim block is not a newestLBN, then copy all valid pages in this victim block to its newestSameLBNBlock(if exists);
                //		Erase the victim block;
                else
                {   // Full merge (0) in SEQ - zero_Association - kickout a victim, before kicking out, copy valid pages to the newestblk
                    //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                    //cout<<"Case 1.3.2.2 The victim block is not a newestBlock, then (1) copy all valid pages in it to its newestBlock(if exists) or (2) directly erase the victim block."<<endl;
                    if (victimBlockCorrespondingDataLBNNewestBlockInSEQ!=NOT_FIND)
                    {
                        
                        // find victimLBN's newestLBN
                        //cout<<"The victim block's newestBlock exists, then copy victim block's valid pages to the newestBlock."<<endl;
                        for (int i=0; i<_Pages_Per_Block; i++)
                        {
                            long pageToCopy=_SEQ_Log_Blocks.readPage(victimBlock, i);
                            if ((pageToCopy!=FREE)&&(pageToCopy!=INVALID))
                            {
                                // copy it to the newest block
                                _SEQ_Log_Blocks.writePageDef(victimBlockCorrespondingDataLBNNewestBlockInSEQ, i, pageToCopy);
                                
                                FMinSLBCpyPg_SBFAST++;
                            }
                        }
                    }
                    // count
                    
                    ZeroAssoFM_SBFAST++;
                    FMinSLB_SBFAST++;
                    // erase
                    _SEQ_Log_Blocks.eraseBlock(victimBlock);
                }
                
                
                // we have a free block
                //cout<<"Now insert to the free block 'newestBlockWithSameDataLBNInSEQ'"<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN); //Just in case RND has same pages with currentLSN
                _SEQ_Log_Blocks.writePage(victimBlock, offset, currentLSN); // insert to the free block "newestBlockWithSameDataLBNInSEQ"
            }
        }
    }
    
    
    
    
    // Case 2, is not block header nor a subblock header
    else
    {
        // Case 2
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
        //cout<<"Case 2. The input page is not a block header nor a subblock header."<<endl;
        //cout<<"Checking whether it's block header or subblock header is in SEQ_Log_Block."<<endl;
        
        // Case 2.1 input page's block header or subblock header is in the SEQ_Log_Block
        if(newestBlockWithSameDataLBNInSEQ!=NOT_FIND)
        {
            //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
            //cout<<"Case 2.1 The input's block header or subblock header is in SEQ_Log_block, and then we try to insert it or full merge the corresponding SEQ_Log_Block."<<endl;
            
            // Case 2.1.1 available and insert
            if (_SEQ_Log_Blocks.readPage(newestBlockWithSameDataLBNInSEQ, offset)==FREE)  //
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 2.1.1 The input page's position is available, then we insert it to the newestBlockWithSameDataLBNInSEQ in SEQ_Log_Block."<<endl;
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                _SEQ_Log_Blocks.writePage(newestBlockWithSameDataLBNInSEQ, offset, currentLSN);
                
                
                if (SEQ_Flag)
                {
                    // seq page
                    traceSeqStreamCapPagesCounter[traceStreamID]++;
                }
                
                
                
            }
            
            // Case 2.1.2 not available, kick out and insert
            else
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 2.1.2 The input page's position is occupied by others, then we merge the SEQ_Log_Block and this input page to the data block."<<endl;
                
                
                // we don't need to write the currentLSN to the SEQ_Log_Blocks because we merge it out.
                //_SEQ_Log_Blocks.partialMergeForSEQ(newestBlockWithSameDataLBNInSEQ, blocksWithSameDataLBNInSEQ, currentLSN);
                //cout<<"We do full merge in SEQ_Log_Blocks now. Note that we merge the input page currentLSN together with its newestBlockWithSameDataLBNInSEQ, which means we are not writing the currentLSN to any log blocks this time."<<endl;
                fullMergeNewestSameLBNBlockInSEQ(newestBlockWithSameDataLBNInSEQ, blocksWithSameDataLBNInSEQ, currentLSN);
                
                // We've already written the currentLSN during the fullMerge. Just mark in SEQ and RND.
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                
            }
            
        }
        
        // Case 2.2
        else
        {
            //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
            //cout<<"Case 2.2 Input's block header and subblock are not in the SEQ_Log_Blocks, then insert to the RND_Log_Block"<<endl;
            //cout<<"Check whether the RND_Log_Blocks are full or not."<<endl;
            
            // Case 2.2.1
            if (_RND_Log_Blocks.blocksAreFull())
            {
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 2.2.1 RND_Log_Blocks are full. Kick out one RND_Log_Block."<<endl;
                long victimLBNInRND = _RND_Log_Blocks.getVictimLBN();
                //cout<<"We call getVictimLBN() to get the currentVictimBlock which is "<<victimLBNInRND<<"th block in RND_Log_Blocks."<<endl;
                //cout<<"After that, we immediately update the victim block number in RND_Log_Blocks."<<endl;
                _RND_Log_Blocks.updateVictimLBN();
                //cout<<"We do full merge in RND_Log_Blocks now:"<<endl;
                fullMergeInRND(victimLBNInRND, currentLSN);    // if the currentLSN is belong to the associated block, then we also merge it together. Thus we need to pass the currentLSN to the fullMergeInRND().
            }
            // Case 2.2.2
            else
            {
                // If the log blocks are not full
                //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
                //cout<<"Case 2.2.2 The log blocks are not full, then directly append it to the blocks."<<endl;
                _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
                _RND_Log_Blocks.appendToBlocks(currentLSN);
            }
            
        }
        
    }
}

// 3
bool SBFAST::partialMergeNewestSameLBNBlockInSEQ(long partialVictimNewestBlock, vector <long> blocksWithSameDataLBNInSEQ)
{
    
    //cout<<"--------------------partialMergeNewestSameLBNBlockInSEQ()------------------------"<<endl;
    // Copy all valid pages in (1) SEQ_Log_Blocks (sameLBNBlocks vector), and (2) RND_Log_Blocks to this newestSameLBNBlock.
    
    // SEQ_Log_Blocks, sameLBNBlocks
    long currentBlk;
    
    //cout<<"partialVictimNewestBlock="<<partialVictimNewestBlock<<endl;
    //cout<<"blocksWithSameDataLBNInSEQ.size()="<<blocksWithSameDataLBNInSEQ.size()<<endl;
    
    
    
    // Scan the whole vector blocksWithSameDataLBNInSEQ
    for (int i=0; i<blocksWithSameDataLBNInSEQ.size();i++)
    {
        //cout<<"i="<<i<<endl;
        if (blocksWithSameDataLBNInSEQ[i]==partialVictimNewestBlock)
        {
            //cout<<"blocksWithSameDataLBNInSEQ[i]==partialVictimNewestBlock  -> continue"<<endl;
            // Skip the newest block (itself)
            continue;
        }
        
        // Scan the whole block
        currentBlk=blocksWithSameDataLBNInSEQ[i];
        //cout<<"currentBlk="<<currentBlk<<endl;
        long pageToCopy;
        for (int j=0; j<_Pages_Per_Block; j++)
        {
            pageToCopy=_SEQ_Log_Blocks.readPage(currentBlk, j);
            if((pageToCopy!=INVALID)&&(pageToCopy!=FREE))
            {
                //cout<<"The page #"<<j<<" is a valid page with value of "<<pageToCopy<<" and we are copying it to the newest block now."<<endl;
                //	Mark SEQ_Log_Block as INVALID
                //cout<<"Marking the old page as INVALID."<<endl;
                _SEQ_Log_Blocks.writePageDef(currentBlk, j, INVALID);
                
                
                //	Copy all valid pages in sameLBNBlocks to this newestSameLBNBlock
                //cout<<"Copy the old page to the newest block."<<endl;
                _SEQ_Log_Blocks.writePageDef(partialVictimNewestBlock, j, pageToCopy);
                PMCpyPg_SBFAST++;
                
            }
        }
    }
    
    
    // RND_Log_Blocks, sameLBNBlocks
    // victimNewBlock's firstPage / _Pages_Per_Block -> victimNewBlock's LBN
    long partialVictimNewestBlockCorrespondingDataLBN=_SEQ_Log_Blocks.getFirstPageOfBlock(partialVictimNewestBlock)/_Pages_Per_Block;
    
    long currentPage;
    long currentPageLBN;
    long currentPageOffset;
    
    
    for (int k=0; k<_RND_Log_Blocks_Num; k++)
    {
        for (int m=0; m<_Pages_Per_Block; m++)
        {
            
            currentPage         =   _RND_Log_Blocks.readPage(k, m);
            if ((currentPage==FREE) || (currentPage==INVALID))
            {continue;}
            
            currentPageLBN      =   currentPage/_Pages_Per_Block;
            currentPageOffset   =   fmod(currentPage, _Pages_Per_Block);
            //cout<<"currentPage="<<currentPage<<endl;
            //cout<<"currentPageLBN"<<currentPageLBN<<", partialVictimNewestBlockCorrespondingDataLBN="<<partialVictimNewestBlockCorrespondingDataLBN<<endl;
            
            
            if (currentPageLBN==partialVictimNewestBlockCorrespondingDataLBN)
            {
                // For same LBN, copy to SEQ. We assume no "sync issue" occurs.
                _SEQ_Log_Blocks.writePageDef(partialVictimNewestBlock, currentPageOffset, currentPage);
                // Mark INVALID
                //cout<<"Marking INVALID to RND["<<k<<"]["<<m<<"]"<<endl;
                _RND_Log_Blocks.writePageDef(k, m, INVALID);
                PMCpyPg_SBFAST++;
            }
        }
        
    }
    
    // Counter
    if (_SEQ_Log_Blocks.isSeqBlock(partialVictimNewestBlock)==true)
    {
        //cout<<"isSeqBlock is true."<<endl;
        //cout<<"switchMergeCountInSEQ++."<<endl;
        SM_SBFAST++;
    }
    else
    {
        //cout<<"isSeqBlock is false."<<endl;
        //cout<<"partialMergeCountInSEQ++."<<endl;
        PM_SBFAST++;
        
        //copy page counter
        
        for (long i=0; i<_Pages_Per_Block; i++)
        {
            ////cout<<"current page is "<<_Log_Blocks[LBN][i]<<endl;
            if(_SEQ_Log_Blocks.readPage(partialVictimNewestBlock, i)==FREE)
            {
                PMCpyPg_SBFAST++;
                
            }
        }
        
        
        
    }
    
    
    
    
    
    
    
    // 	Erase the LBN block
    _SEQ_Log_Blocks.eraseBlock(partialVictimNewestBlock);
    
    
    return true;
}


// 4
bool SBFAST::fullMergeNewestSameLBNBlockInSEQ(long partialVictimNewestBlock, vector <long> blocksWithSameDataLBNInSEQ, long currentLSN)
{
    // actually, we don't need to pass "currentLSN" to this function from the programming view. The reason is we will write it togeter with the partialVictimBlock pages. No counter will be affected.
    
    // Copy all valid pages in (1) SEQ_Log_Blocks (sameLBNBlocks vector), and (2) RND_Log_Blocks to this newestSameLBNBlock.
    
    // SEQ_Log_Blocks, sameLBNBlocks
    long currentBlk;
    // Scan the whole vector blocksWithSameDataLBNInSEQ
    for (int i=0; i<blocksWithSameDataLBNInSEQ.size();i++)
    {
        if (i==partialVictimNewestBlock)
        {
            // Skip the newest block (itself)
            continue;
        }
        
        // Scan the whole block
        currentBlk=blocksWithSameDataLBNInSEQ[i];
        long pageToCopy;
        for (int j=0; j<_Pages_Per_Block; j++)
        {
            pageToCopy=_SEQ_Log_Blocks.readPage(currentBlk, j);
            if((pageToCopy!=INVALID)&&(pageToCopy!=FREE))
            {
                //	Mark SEQ_Log_Block as INVALID
                _SEQ_Log_Blocks.writePageDef(currentBlk, j, INVALID);
                //	Copy the valid page belongs to the same data block lbn as the partialVictimNewestBlock to merge later
                //cout<<"Copy the valid page belongs to the same data block lbn as the partialVictimNewestBlock to merge later."<<endl;
                //_SEQ_Log_Blocks.writePageDef(partialVictimNewestBlock, j, pageToCopy);
                FMinSLBCpyPg_SBFAST++;
            }
        }
    }
    
    
    // RND_Log_Blocks, sameLBNBlocks
    // victimNewBlock's firstPage / _Pages_Per_Block -> victimNewBlock's LBN
    long partialVictimNewestBlockCorrespondingDataLBN=_SEQ_Log_Blocks.getFirstPageOfBlock(partialVictimNewestBlock)/_Pages_Per_Block;
    
    long currentPage;
    long currentPageLBN;
    long currentPageOffset;
    
    
    for (int k=0; k<_RND_Log_Blocks_Num; k++)
    {
        for (int m=0; m<_Pages_Per_Block; m++)
        {
            currentPage         =   _RND_Log_Blocks.readPage(k, m);
            
            if ((currentPage==FREE) || (currentPage==INVALID))
            {continue;}
            
            
            currentPageLBN      =   currentPage/_Pages_Per_Block;
            currentPageOffset   =   fmod(currentPage, _Pages_Per_Block);
            
            if (currentPageLBN==partialVictimNewestBlockCorrespondingDataLBN)
            {
                // For same LBN, copy to SEQ. We assume no "sync issue" occurs.
                _SEQ_Log_Blocks.writePageDef(partialVictimNewestBlock, currentPageOffset, currentPage);
                // Mark INVALID
                //cout<<"Marking INVALID to RND["<<k<<"]["<<m<<"]."<<endl;
                _RND_Log_Blocks.writePageDef(k, m, INVALID);
                FMinSLBCpyPg_SBFAST++;
            }
        }
        
    }
    
    
    
    //
    
    //cout<<"Merge the input page out together with its same data LBN page(s) in this victim block."<<endl;
    
    
    
    //
    // 	Erase the LBN block
    
    _SEQ_Log_Blocks.eraseBlock(partialVictimNewestBlock);   // 1 for log block
    Erase_SBFAST++;  // 1 for data block
    
    //cout<<"We erased 2 blocks = 1 * Log + 1* Data"<<endl;
    FMinSLB_SBFAST++;
    
    return true;
}


// 5
bool SBFAST::fullMergeInRND(long victimBlock, long currentLSN)
{
    
    // Calculate the number of associated blocks
    long inputPageCorrespondingDataLBN=currentLSN/_Pages_Per_Block;
    
    long currentVictimPage;
    long currentVictimPageCorrespondingDataLBN;
    long currentVictimPageOffset;
    long newestBlockInSEQWithSameCorrespondingLBN;
    long Associated_Blocks_To_Erase_Num;
    
    
    
    set<long> Associated_Blocks_To_Erase;
    
    
    
    // scan the whole pages in the victim RND_Log_Block
    for (int i=0; i<_Pages_Per_Block; i++)
    {
        currentVictimPage=_RND_Log_Blocks.readPage(victimBlock, i);
        if((currentVictimPage==FREE) || (currentVictimPage==INVALID))
        {
            //cout<<"Not useful page, go to next."<<endl;
            continue;
        }
        else
        {
            // Calculate
            currentVictimPageCorrespondingDataLBN=currentVictimPage/_Pages_Per_Block;
            currentVictimPageOffset=fmod(currentVictimPage, _Pages_Per_Block);
            newestBlockInSEQWithSameCorrespondingLBN=_SEQ_Log_Blocks.findNewestBlockWithSameDataLBNInSEQ(currentVictimPageCorrespondingDataLBN);
            
            
            if (newestBlockInSEQWithSameCorrespondingLBN==NOT_FIND)
            {
                //cout<<"currentVictimPage's corresponding data LBN has not the newest log block in SEQ_Lob_Blocks. We save currentVictimPageCorrespondingDataLBN to a set which keeps all the data blocks to be erased later."<<endl;
                Associated_Blocks_To_Erase.insert(currentVictimPageCorrespondingDataLBN);
            }
            else
            {
                //cout<<"currentVictimPage's corresponding data LBN is in SEQ_Lob_Blocks. Then copy the currentVictimPage to the newestBlockInSEQWithSameCorrespondingLBN"<<endl;
                _SEQ_Log_Blocks.writePageDef(newestBlockInSEQWithSameCorrespondingLBN, currentVictimPageOffset, currentVictimPage);
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentVictimPage);
                FMinRLBCpyPg_SBFAST++;
            }
            
        }
    }
    
    
    
    //Erase_SBFAST++;
    
    Associated_Blocks_To_Erase_Num=(long)Associated_Blocks_To_Erase.size();
    
    //cout<<"We are erasing the 1 victim RND_Log_Block now."<<endl;
    _RND_Log_Blocks.eraseBlock(victimBlock);
    // Erase_SBFAST has already been ++ inside the eraseBlock() function.
    printSet(Associated_Blocks_To_Erase);
    
    //cout<<"==========inputPageCorrespondingDataLBN="<<inputPageCorrespondingDataLBN<<endl;
    //cout<<"Associated_Blocks_To_Erase.count(currentVictimPageCorrespondingDataLBN)="<<Associated_Blocks_To_Erase.count(currentVictimPageCorrespondingDataLBN)<<endl;
    
    if (Associated_Blocks_To_Erase.count(inputPageCorrespondingDataLBN)==0)
    {
        //cout<<"We still need to write the input page after erasing the victim block since no pages in the victim block has the same data LBN as it."<<endl;
        _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
        _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
        
        _RND_Log_Blocks.writePageDef(victimBlock,0,currentLSN);
    }
    else
    {
        _SEQ_Log_Blocks.markInvalidToSameAddressPagesAndLockBlock(currentLSN);
        _RND_Log_Blocks.markInvalidToSameAddressPages(currentLSN);
        
        //cout<<"Merge the input page out together with its same data LBN page(s) in this victim block."<<endl;
    }
    
    Erase_SBFAST=Erase_SBFAST+Associated_Blocks_To_Erase_Num;
    
    // eraseCount
    //cout<<"Erase = 1 * Victim RND_Log_Block + "<< Associated_Blocks_To_Erase_Num <<" * Data Blocks = "<<(Associated_Blocks_To_Erase_Num+1)<<"."<<endl;
    //cout<<"FMinRLB_SBFAST++ in fullMergeInRND()."<<endl;
    
    
    FMinRLB_SBFAST++;
    if (Associated_Blocks_To_Erase_Num<=0)
    {
        ZeroAssoFM_SBFAST++;
    }
    
    AssoDBlk_SBFAST=AssoDBlk_SBFAST+Associated_Blocks_To_Erase_Num;
    
    return true;
    
}


// 6
void SBFAST::printVector(vector<long> v)
{
    //cout<<"========Printing a vector======="<<endl;
    for (int i=0; i<v.size(); i++)
    {
        //cout<<v[i]<<"   ";
    }
    
    //cout<<endl<<"========Finish printing======="<<endl;
    
}

// 7
void SBFAST::printSet(set<long> v)
{
    //cout<<"========Printing a vector======="<<endl;
    set<long>::iterator iter;
    for(iter=v.begin(); iter!=v.end();iter++)
    {
        //cout<<*iter<<"   ";
    }
    
    //cout<<endl<<"========Finish printing======="<<endl;
    
}



// 8
void SBFAST::cleanupLog()
{
    //cout<<"Now kicking out all pages in the log blocks in SBFAST."<<endl;
    //cout<<"=======================================SEQ_Log_Blocks:======================================="<<endl;
    // Kick out SEQ_Log_Blocks
    for (long i=0; i<_SEQ_Log_Blocks_Num; i++)
    {
        if (_SEQ_Log_Blocks.blockIsFree(i))
        {
            //cout<<"Block "<<i<<" is free."<<endl;
            continue;
        }
        else
        {
            //cout<<"partial or switch merge in SEQ"<<endl;
            kickoutBlockInSEQForCleanup(i);
        }
        //cout<<"RND_Log_Block is:"<<endl;
        _RND_Log_Blocks.readLogBlocks();
        //cout<<"SEQ_Log_Block is:"<<endl;
        _SEQ_Log_Blocks.readLogBlocks();
    }
    
    // Kick out RND_Log_Blocks
    //cout<<"=======================================RND_Log_Blocks:======================================="<<endl;
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
            fullMergeInRNDForCleanup(i);
        }
        //cout<<"RND_Log_Block is:"<<endl;
        _RND_Log_Blocks.readLogBlocks();
        //cout<<"SEQ_Log_Block is:"<<endl;
        _SEQ_Log_Blocks.readLogBlocks();
    }
    
    //cout<<"After cleanning up log blocks in FAST:"<<endl;
    //cout<<"RND_Log_Block is:"<<endl;
    _RND_Log_Blocks.readLogBlocks();
    //cout<<"SEQ_Log_Block is:"<<endl;
    _SEQ_Log_Blocks.readLogBlocks();
    
}



// 9
bool SBFAST::kickoutBlockInSEQForCleanup(long victimBlock)
{
    //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
    //cout<<"Kicking out a block in SEQ."<<endl;
    
    // Preparation
    // Get the first valid page of it. If no valid page, it will return NOT_VALID_DATA.
    // Since there are not free blocks in the SEQ_Log_Blocks, so it returns a valid page or NOT_VALID_DATA (the block is useless).
    long victimBlockFirstPage=_SEQ_Log_Blocks.getFirstPageOfBlock(victimBlock);
    //cout<<"victimBlockFirstPage="<<victimBlockFirstPage<<endl;
    
    
    long victimBlockCorrespondingDataLBN=victimBlockFirstPage/_Pages_Per_Block;
    //cout<<"victimBlockCorrespondingDataLBN="<<victimBlockCorrespondingDataLBN<<endl;
    
    
    
    
    //cout<<"Checking whether the victimBlock is a newestBlock or not:"<<endl;
    // Search for those blocks in the SEQ_Log_Blocks that have the same data LBN with the input page victimLBN.
    vector<long> victimSameDataLBNBlocksInSED;
    long victimBlockCorrespondingDataLBNNewestBlockInSEQ = _SEQ_Log_Blocks.findNewestBlockWithSameDataLBNInSEQ(victimBlockCorrespondingDataLBN, victimSameDataLBNBlocksInSED);
    
    
    //cout<<"=============Printing victimSameDataLBNBlocksInSED vector================"<<endl;
    printVector(victimSameDataLBNBlocksInSED);
    
    
    //  If the victim block is the newestBlock, then it cannot have any invalid data pages.
    if (_SEQ_Log_Blocks.checkValidBlock(victimBlock)==true)
    {
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
        //cout<<"The victim block is a newestBlock, then partial merge."<<endl;
        //cout<<"The victim block has no invalid data which means its a newBlock. We partial merge it."<<endl;
        //cout<<"victimBlock="<<victimBlock<<endl;
        partialMergeNewestSameLBNBlockInSEQ(victimBlock, victimSameDataLBNBlocksInSED);
        
    }
    
    
    // If the victim block is not a newestLBN, then copy all valid pages in this victim block to its newestSameLBNBlock(if exists);
    //		Erase the victim block;
    else
    {
        //cout<<"--------------------------------------------------------------------------------------------------------"<<endl;
        //cout<<"The victim block is not a newestBlock, then (1) copy all valid pages in it to its newestBlock(if exists) or (2) directly erase the victim block."<<endl;
        if (victimBlockCorrespondingDataLBNNewestBlockInSEQ!=NOT_FIND)
        {
            
            // find victimLBN's newestLBN
            //cout<<"The victim block's newestBlock exists, then copy victim block's valid pages to the newestBlock."<<endl;
            for (int i=0; i<_Pages_Per_Block; i++)
            {
                long pageToCopy=_SEQ_Log_Blocks.readPage(victimBlock, i);
                if ((pageToCopy!=FREE)&&(pageToCopy!=INVALID))
                {
                    // copy it to the newest block
                    _SEQ_Log_Blocks.writePageDef(victimBlockCorrespondingDataLBNNewestBlockInSEQ, i, pageToCopy);
                }
            }
        }
        // count
        
        ZeroAssoFM_SBFAST++;
        FMinSLB_SBFAST++;
        // erase
        _SEQ_Log_Blocks.eraseBlock(victimBlock);
    }
    
    
    return true;
}

// 10
bool SBFAST::fullMergeInRNDForCleanup(long victimBlock)
{
    // Calculate the number of associated blocks
    long currentVictimPage;
    long currentVictimPageCorrespondingDataLBN;
    long currentVictimPageOffset;
    long newestBlockInSEQWithSameCorrespondingLBN;
    long Associated_Blocks_To_Erase_Num;
    
    
    
    set<long> Associated_Blocks_To_Erase;
    
    
    
    // scan the whole pages in the victim RND_Log_Block
    for (int i=0; i<_Pages_Per_Block; i++)
    {
        currentVictimPage=_RND_Log_Blocks.readPage(victimBlock, i);
        if((currentVictimPage==FREE) || (currentVictimPage==INVALID))
        {
            // cout<<"Not useful page, go to next."<<endl;
            continue;
        }
        else
        {
            // Calculate
            currentVictimPageCorrespondingDataLBN=currentVictimPage/_Pages_Per_Block;
            currentVictimPageOffset=fmod(currentVictimPage, _Pages_Per_Block);
            newestBlockInSEQWithSameCorrespondingLBN=_SEQ_Log_Blocks.findNewestBlockWithSameDataLBNInSEQ(currentVictimPageCorrespondingDataLBN);
            
            
            if (newestBlockInSEQWithSameCorrespondingLBN==NOT_FIND)
            {
                //  cout<<"currentVictimPage's corresponding data LBN has not the newest log block in SEQ_Lob_Blocks. We save currentVictimPageCorrespondingDataLBN to a set which keeps all the data blocks to be erased later."<<endl;
                Associated_Blocks_To_Erase.insert(currentVictimPageCorrespondingDataLBN);
            }
            else
            {
                // cout<<"currentVictimPage's corresponding data LBN is in SEQ_Lob_Blocks. Then copy the currentVictimPage to the newestBlockInSEQWithSameCorrespondingLBN"<<endl;
                _SEQ_Log_Blocks.writePageDef(newestBlockInSEQWithSameCorrespondingLBN, currentVictimPageOffset, currentVictimPage);
                _RND_Log_Blocks.markInvalidToSameAddressPages(currentVictimPage);
            }
            
        }
    }
    
    
    
    //Erase_SBFAST++;
    
    Associated_Blocks_To_Erase_Num=(long)Associated_Blocks_To_Erase.size();
    
    //cout<<"We are erasing the 1 victim RND_Log_Block now."<<endl;
    _RND_Log_Blocks.eraseBlock(victimBlock);
    // Erase_SBFAST has already been ++ inside the eraseBlock() function.
    printSet(Associated_Blocks_To_Erase);
    
    //cout<<"Associated_Blocks_To_Erase.count(currentVictimPageCorrespondingDataLBN)="<<Associated_Blocks_To_Erase.count(currentVictimPageCorrespondingDataLBN)<<endl;
    
    
    
    Erase_SBFAST=Erase_SBFAST+Associated_Blocks_To_Erase_Num;
    
    // eraseCount
    //cout<<"Erase = 1 * Victim RND_Log_Block + "<< Associated_Blocks_To_Erase_Num <<" * Data Blocks = "<<(Associated_Blocks_To_Erase_Num+1)<<"."<<endl;
    //cout<<"FMinRLB_SBFAST++ in fullMergeInRNDForCleanup()."<<endl;
    FMinRLB_SBFAST++;
    if (Associated_Blocks_To_Erase_Num<=0)
    {     
        ZeroAssoFM_SBFAST++;
    }
    return true;
    
}



