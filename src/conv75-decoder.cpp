#include "convolution-coding.hpp"
#include <iostream>
#include <climits>

const int numStates = 4;

std::vector<int> conv75_viterbi_decode(std::vector<int>& demodulated_signal) {
    size_t length = demodulated_signal.size();
    int size = length / 2;
    std::vector<int> output(size);

    trellis75 T;

    const int MAX_METRIC = INT_MAX;
    int stateMetric[4] = {0, MAX_METRIC, MAX_METRIC, MAX_METRIC};

    std::vector<std::vector<int>> tbInput(numStates, std::vector<int>(size, 0));
    std::vector<std::vector<int>> tbState(numStates, std::vector<int>(size, 0));

    for (int ib = 0; ib < size; ib++) {

        int inputOffset = ib * 2;
        int codeWord[2] = {
            demodulated_signal[inputOffset],
            demodulated_signal[inputOffset + 1]
        };

        int branchMetric[4];
        
        for (int branch = 0; branch < 4; branch++) {
            int temp = branch;
            int metric = 0;

            for (int q = 0; q < 2; q++) {
                int expectedBit = temp & 1;
                int receivedBit = codeWord[1 - q];
                
                if (expectedBit == 1) {
                    metric += (1 - receivedBit);
                } else {
                    metric += receivedBit;
                }
                temp >>= 1;
            }
            branchMetric[branch] = metric;
        }

        int tempMetric[4];

        for (int i = 0; i < numStates; i++) {
            tempMetric[i] = MAX_METRIC;
        }

        for (int currState = 0; currState < numStates; currState++) {
            int currMetric = stateMetric[currState];
            
            for (int input = 0; input < 2; input++) {
                int nextState = T.nextStates[currState][input];
                int outputBits = T.outputs[currState][input];
                
                int newMetric = currMetric + branchMetric[outputBits];
                
                if (newMetric < tempMetric[nextState]) {
                    tempMetric[nextState] = newMetric;
                    
                    tbState[nextState][ib] = currState;
                    tbInput[nextState][ib] = input;
                }
            }
        }
        int minState = 0;
        int minMetric = tempMetric[0];
        
        for (int s = 0; s < numStates; s++) {
            stateMetric[s] = tempMetric[s];
        }
    }
    int minState = 0;

    for (int ib = size - 1; ib >= 0; ib--) {
        output[ib] = tbInput[minState][ib];
        minState = tbState[minState][ib];
    }

    return output;
}
