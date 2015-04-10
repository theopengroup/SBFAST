#include "FTL_Scheme_Tester.h"
FTL_Scheme_Tester::FTL_Scheme_Tester(char * incomingWriteSeqFileChar):incomingWriteSeqFileForPrint(incomingWriteSeqFileChar), incomingWriteSeqFileForBAST(incomingWriteSeqFileChar),incomingWriteSeqFileForFAST(incomingWriteSeqFileChar), incomingWriteSeqFileForSBFAST(incomingWriteSeqFileChar)
{
    BAST BAST_Runner(incomingWriteSeqFileForBAST);
    FAST FAST_Runner(incomingWriteSeqFileForFAST);
    SBFAST SBFAST_Runner(incomingWriteSeqFileForSBFAST);
    
    // Print Results
    //cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<"Final results:"<<endl;
    //printResults();
    
    BAST_Runner.cleanupLog();
    FAST_Runner.cleanupLog();
    SBFAST_Runner.cleanupLog();
    // Print Results
    //cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<"After cleanning up all pages in the log blocks:"<<endl;
    //printConfig(0);
    //printConfig(1);
    //printConfig(2);
    recordResults();
    
    
   /* {;
        //void FTL_Scheme_Tester::printConfig(int algorithmType)
        //{
        //    switch (algorithmType) {
        //        case 0:
        //        {
        //            // Print the configurations at the beginning
        //            outputFile<<"=============================BAST============================="<<endl;
        //            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
        //            outputFile<<"Log_Blocks Number is:"<<Log_Blocks_Num_BAST<<endl;
        //            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
        //            outputFile<<"Pages_Per_Subblock is:"<<Pages_Per_Subblock<<endl;
        //            outputFile<<"----------------------------Counters--------------------------"<<endl;
        //            outputFile<<"Erase_BAST is:"<<Erase_BAST<<endl;
        //            outputFile<<"FM_BAST is:"<<FM_BAST<<endl;
        //            outputFile<<"SM_BAST is:"<<SM_BAST<<endl;
        //            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;
        //            break;
        //        }
        //
        //        case 1:
        //        {
        //            // Print the configurations at the beginning
        //            outputFile<<"=============================FAST============================="<<endl;
        //            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
        //            outputFile<<"SEQ_Log_Blocks Number is:"<<SEQ_Log_Blocks_Num_FAST<<endl;
        //            outputFile<<"RND_Log_Blocks Number is:"<<RND_Log_Blocks_Num_FAST<<endl;
        //            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
        //            outputFile<<"Pages_Per_Subblock is:"<<Pages_Per_Subblock<<endl;
        //            outputFile<<"----------------------------Counters--------------------------"<<endl;
        //            outputFile<<"Erase_FAST is:"<<Erase_FAST<<endl;
        //            outputFile<<"FMinRLB_FAST is:"<<FMinRLB_FAST<<endl;
        //            outputFile<<"PM_FAST is:"<<PM_FAST<<endl;
        //            outputFile<<"SM_FAST is:"<<SM_FAST<<endl;
        //            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;
        //            break;
        //        }
        //        case 2:
        //        {
        //            // Print the configuratins at the beginning
        //            outputFile<<"==============================SBFAST=========================="<<endl;
        //            outputFile<<"---------------------Log Block Configurations-----------------"<<endl;
        //            outputFile<<"SEQ_Log_Blocks Number is:"<<SEQ_Log_Blocks_Num_SBFAST<<endl;
        //            outputFile<<"RND_Log_Blocks Number is:"<<RND_Log_Blocks_Num_SBFAST<<endl;
        //            outputFile<<"Pages_Per_Block is:"<<Pages_Per_Block<<endl;
        //            outputFile<<"Pages_Per_Subblock is:"<<Pages_Per_Subblock<<endl;
        //            outputFile<<"----------------------------Counters--------------------------"<<endl;
        //            outputFile<<"Erase_SBFAST is:"<<Erase_SBFAST<<endl;
        //            outputFile<<"FMinSLB_SBFAST is:"<<FMinSLB_SBFAST<<endl;
        //            outputFile<<"FMinRLB_SBFAST is:"<<FMinRLB_SBFAST<<endl;
        //            outputFile<<"PM_SBFAST is:"<<PM_SBFAST<<endl;
        //            outputFile<<"SM_SBFAST is:"<<SM_SBFAST<<endl;
        //            outputFile<<"ZeroAssoFM_SBFAST is:"<<ZeroAssoFM_SBFAST<<endl;
        //            outputFile<<"=============================================================="<<endl<<endl<<endl<<endl;
        //            break;
        //        }
        //        default:
        //            break;
        //    }
        //    
        //    
        //}
        
    }*/
}







void FTL_Scheme_Tester::recordResults()
{        
    // BAST    
    vector<long> currentBAST;
    currentBAST.push_back(Erase_BAST);
    currentBAST.push_back(FM_BAST);
    currentBAST.push_back(SM_BAST);
    currentBAST.push_back(CapCrctStrm_BAST);

    // FAST
    vector<long> currentFAST;
    currentFAST.push_back(Erase_FAST);
    currentFAST.push_back(FMinRLB_FAST);
    currentFAST.push_back(FMinSLB_FAST);
    currentFAST.push_back(PM_FAST);
    currentFAST.push_back(SM_FAST);
    currentFAST.push_back(ZeroAssoFM_FAST);
    currentFAST.push_back(AssoDBlk_FAST);
    currentFAST.push_back(PMCpyPg_FAST);
    currentFAST.push_back(CapCrctBlkHdr_FAST);
    currentFAST.push_back(CapWrgBlkHdr_FAST);
        
    // SBFAST    
    vector<long> currentSBFAST;
    currentSBFAST.push_back(Erase_SBFAST);
    currentSBFAST.push_back(FMinRLB_SBFAST);
    currentSBFAST.push_back(FMinSLB_SBFAST);
    currentSBFAST.push_back(PM_SBFAST);
    currentSBFAST.push_back(SM_SBFAST);
    currentSBFAST.push_back(ZeroAssoFM_SBFAST);
    currentSBFAST.push_back(AssoDBlk_SBFAST);
    currentSBFAST.push_back(PMCpyPg_SBFAST);
    currentSBFAST.push_back(FMinRLBCpyPg_SBFAST);
    currentSBFAST.push_back(FMinSLBCpyPg_SBFAST);
    currentSBFAST.push_back(CapCrctStrmSubHdr_SBFAST);
    currentSBFAST.push_back(CapWrgStrmSubHdr_SBFAST);    
    
    
    // Calculate avgPagesCapForEachSubSeqStrm
    long totalCapSeqPages=0;
    for (int i=0; i<traceSeqStreamCapPagesCounter.size(); i++)
    {
        totalCapSeqPages=totalCapSeqPages+traceSeqStreamCapPagesCounter[i];
    }
    //cout<<"totalCapSeqPages="<<totalCapSeqPages<<endl;
    
    //cout<<"traceSeqStreamCapPagesCounterSize="<<traceSeqStreamCapPagesCounter.size()<<endl;
    //cout<<(float)totalCapSeqPages/traceSeqStreamCapPagesCounter.size();
    
    //avgPagesCapForEachSubSeqStrm=(float)totalCapSeqPages/traceSeqStreamCapPagesCounter.size();
    //cout<<avgPagesCapForEachSubSeqStrm;
    currentSBFAST.push_back(totalCapSeqPages);
    
    
    
    BASTResults.push_back(currentBAST);
    FASTResults.push_back(currentFAST);
    SBFASTResults.push_back(currentSBFAST);
    
    
    
    
}
