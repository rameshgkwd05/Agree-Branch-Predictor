/*
 * agree.hh
 *
 *  Created on: 27-Nov-2014
 *      Author: gauri
 * Modified by: Ramesh 27/11/2014  v 0.1
*/

#ifndef __CPU_PRED_AGREE_PRED_HH_
#define __CPU_PRED_AGREE_PRED_HH_

#include "cpu/pred/bpred_unit.hh"
#include "cpu/pred/sat_counter.hh"

class agreeBP : public BPredUnit
{
public:
	agreeBP (const Params *params);
	void uncondBranch(void * &bp_history);
	void squash(void *bp_history);
	bool lookup(Addr branch_addr, void * &bp_history);
	void btbUpdate(Addr branch_addr, void * &bp_history);
	void update(Addr branch_addr, bool taken, void *bp_history, bool squashed);
	void retireSquashed(void *bp_history);

private:
    void updateGlobalHistReg(bool taken);

    struct BPHistory {
            unsigned globalHistoryReg;
            // Prediction of Biasing bit
            bool predBT;
            // prediction(MSB) of the PHT
            bool predPHT;
            // the final prediction (XNOR(predBT,predPHT)
            bool finalPred;
        };

        // Biasing bit's counter predictors
        std::vector<bool> BiasingBits;
        //Biasing bit counter for keeping track of branch instance referring to particular biasing bit
        std::vector<int> BiasingBitsCounter;
        //PHT direction predictors containing saturation counters
        std::vector<SatCounter> PHTCounters;


        unsigned instShiftAmt;

        unsigned globalHistoryReg;
        unsigned globalHistoryBits;
        unsigned historyRegisterMask;

        unsigned BiasingBitArraySize;
        unsigned PHTCtrBits;
        unsigned BiasingHistoryMask;
        unsigned PHTSize;
        unsigned globalCtrBits;
        unsigned globalHistoryMask;


        unsigned PHTThreshold;

    };

#endif /* __CPU_PRED_AGREE_PRED_HH_ */
