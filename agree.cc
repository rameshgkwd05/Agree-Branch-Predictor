/*
 * agree.cc

 *
 *  Created on: 27-Nov-2014
 *      Author: gauri
 *	Modified by: Ramesh Gaikwad 27/11/14 v 1.0
 *								3/12/14 v 1.1  -- modified lookup and update code
 *												- dynamic modification of Biasing bit
 */
#include "base/bitfield.hh"
#include "base/intmath.hh"
#include "cpu/pred/agree.hh"

agreeBP::agreeBP(const Params *params)
    : BPredUnit(params), instShiftAmt(params->instShiftAmt),
      globalHistoryReg(0),
      globalHistoryBits(ceilLog2(params->globalPredictorSize)),
      BiasingBitArraySize(params->choicePredictorSize),
      PHTCtrBits(params->choiceCtrBits),
      PHTSize(params->globalPredictorSize),
      globalCtrBits(params->globalCtrBits)
{
    if (!isPowerOf2(BiasingBitArraySize))
        fatal("Invalid Biasing-Bits Array Size.\n");
    if (!isPowerOf2(PHTSize))
        fatal("Invalid PHT size.\n");

    /* Resizing all vectors and counters*/
    BiasingBits.resize(BiasingBitArraySize);
    BiasingBitsCounter.resize(BiasingBitArraySize);
    PHTCounters.resize(PHTSize);

    /*Initialization alternate true false*/
    for (int i = 0; i < BiasingBitArraySize; ++i) {
        if(i%2 == 0) BiasingBits[i]= true;
	else BiasingBits[i]= false;

        BiasingBitsCounter[i]=0;
    }
    for (int i = 0; i < PHTSize; ++i) {
        PHTCounters[i].setBits(globalCtrBits);
        }

    historyRegisterMask = mask(globalHistoryBits);
    BiasingHistoryMask = BiasingBitArraySize - 1;
    globalHistoryMask = PHTSize - 1;


    PHTThreshold = (ULL(1) << (PHTCtrBits - 1)) - 1;

}

void
agreeBP::btbUpdate(Addr branchAddr, void * &bpHistory)
{
    globalHistoryReg &= (historyRegisterMask & ~ULL(1));
}

void
agreeBP::uncondBranch(void * &bpHistory)
{
    BPHistory *history = new BPHistory;
    history->globalHistoryReg = globalHistoryReg;
    history->predBT = true;
    history->predPHT = true;
    history->finalPred = true;
    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(true);
}

void
agreeBP::squash(void *bpHistory)
{
    BPHistory *history = static_cast<BPHistory*>(bpHistory);
    globalHistoryReg = history->globalHistoryReg;

    delete history;
}
/*
 * We will lookup PHT and biasing bit
 * Later we will pass them to XNOR gate.
 * XNOR behavior: if PHTGHBPrediction is 1 then it will use biasing bit as it is
 * 				   else invert(biasing bit) will be used
 */

bool
agreeBP::lookup(Addr branchAddr, void * &bpHistory)
{
    unsigned BBTableIdx = ((branchAddr >> instShiftAmt)
                                & BiasingHistoryMask);
    unsigned PHTIdx = (((branchAddr >> instShiftAmt)
                                ^ globalHistoryReg)
                                & globalHistoryMask);

    assert(BBTableIdx < BiasingBitArraySize);
    assert(PHTIdx < PHTSize);

    bool biasingBit = BiasingBits[BBTableIdx];
    bool PHTPrediction = PHTCounters[PHTIdx].read()
                              > PHTThreshold;
    bool finalPrediction;


    BPHistory *history = new BPHistory;
    history->globalHistoryReg = globalHistoryReg;
    history->predBT = biasingBit;
    history->predPHT = PHTPrediction;


    /*
     * XNOR Operation
     */
    if (PHTPrediction) {
    	finalPrediction = biasingBit; // use biasing bit
    } else {
    	finalPrediction = !biasingBit;// use invert(biasing bit)
    }

    /*
     * Update Global history register
     */
    history->finalPred = finalPrediction;
    bpHistory = static_cast<void*>(history);
    updateGlobalHistReg(finalPrediction);

    return finalPrediction;
}

void
agreeBP::update(Addr branchAddr, bool taken, void *bpHistory, bool squashed)
{
	if (bpHistory) {
        BPHistory *history = static_cast<BPHistory*>(bpHistory);


        unsigned PHTIdx = (((branchAddr >> instShiftAmt)
                                    ^ history->globalHistoryReg)
                                    & globalHistoryMask);

        assert(PHTIdx < PHTSize);
        /*
         * Update PHT saturation counter 
	 * if direction "agrees" with biasing bit then increment else decrement
         */
        if (history->predPHT) {
                PHTCounters[PHTIdx].increment();
        } else {
                PHTCounters[PHTIdx].decrement();
        }
        

        /*
         * Updating Global History register of BPhistory
         */
        if (squashed) {
            if (taken) {
                globalHistoryReg = (history->globalHistoryReg << 1) | 1;
            } else {
                globalHistoryReg = (history->globalHistoryReg << 1);
            }
            globalHistoryReg &= historyRegisterMask;
        } else {
            delete history;
        }




	 unsigned BBTableIdx = ((branchAddr >> instShiftAmt) & BiasingHistoryMask);
       	 assert(BBTableIdx < BiasingBitArraySize);

       	//Updating the biasing bit based on the first branch's outcome
         if(BiasingBitsCounter[BBTableIdx] == 0){
        		BiasingBitsCounter[BBTableIdx] = 1;
        		BiasingBits[BBTableIdx]=taken;
         }//update if branch bias is changed
         /*else{
        	 if(BiasingBits[BBTableIdx] != taken){//if mis-predicted
        		 BiasingBits[BBTableIdx]= !(BiasingBits[BBTableIdx]); // change the bias
        	 }
         }
	*/

      }
}

void
agreeBP::retireSquashed(void *bp_history)
{
    BPHistory *history = static_cast<BPHistory*>(bp_history);
    delete history;
}

void
agreeBP::updateGlobalHistReg(bool taken)
{
    globalHistoryReg = taken ? (globalHistoryReg << 1) | 1 :
                               (globalHistoryReg << 1);
    globalHistoryReg &= historyRegisterMask;
}
