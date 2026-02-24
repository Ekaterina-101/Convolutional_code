#include "convolution-coding.hpp"
#include <iostream>
#include <climits>

const int hamming_distance[4][4] = {
    {0, 1, 1, 2},
    {1, 0, 2, 1},
    {1, 2, 0, 1},
    {2, 1, 1, 0}
};

std::vector<int> conv75_viterbi_decode(std::vector<int>& encoded) {
    size_t length = encoded.size() / 2;
    std::vector<int> output(length);
    
    trellis75 T;

    int state_metric[4] = {0, INT_MAX, INT_MAX, INT_MAX};

    std::vector<std::vector<int>> tb_state(T.numStates, std::vector<int>(length));
    std::vector<std::vector<int>> tb_input(T.numStates, std::vector<int>(length));

    for (int i = 0; i < length; i++) {
        int received_symbol = (encoded[2 * i] << 1) | encoded[2 * i + 1];

        int temp_metric[T.numStates] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};

        for (int curr_state = 0; curr_state < T.numStates; curr_state++) {
            int curr_metric = state_metric[curr_state];

            for (int input = 0; input < 2; input++) {
                int next_state = T.nextStates[curr_state][input];
                int output_symbol = T.outputs[curr_state][input];
                
                int new_metric = curr_metric + hamming_distance[received_symbol][output_symbol]; 
                
                if (new_metric < temp_metric[next_state]) {
                    temp_metric[next_state] = new_metric;
                    
                    tb_state[next_state][i] = curr_state;
                    tb_input[next_state][i] = input;
                }
            }
        }
        
        for (int s = 0; s < T.numStates; s++) {
            state_metric[s] = temp_metric[s];
        }
    }
    
    int final_state = 0;
    for (int i = length - 1; i >= 0; i--) {
        output[i] = tb_input[final_state][i];
        final_state = tb_state[final_state][i];
    }
    
    return output;
}

std::vector<int> conv75_soft_decode(std::vector<double>& encoded) {
    size_t length = encoded.size() / 2;
    std::vector<int> output(length);
    
    trellis75 T;

    double state_metric[4] = {0.0, INT_MAX ,INT_MAX, INT_MAX};

    std::vector<std::vector<int>> tb_state(T.numStates, std::vector<int>(length));
    std::vector<std::vector<int>> tb_input(T.numStates, std::vector<int>(length));

    for (int i = 0; i < length; i++) {
        double temp_metric[T.numStates] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};

        for (int curr_state = 0; curr_state < T.numStates; curr_state++) {
            double curr_metric = state_metric[curr_state];

            for (int input = 0; input < 2; input++) {
                int next_state = T.nextStates[curr_state][input];
                int output_symbol = T.outputs[curr_state][input];
                int b0 = output_symbol >> 1;
                int b1 = output_symbol & 1;
                
                double correltion = (1 - 2 * b0) * encoded[2 * i] + (1 - 2 * b1) * encoded[2 * i + 1];
                double new_metric = curr_metric - correltion;
                
                if (new_metric < temp_metric[next_state]) {
                    temp_metric[next_state] = new_metric;
                    
                    tb_state[next_state][i] = curr_state;
                    tb_input[next_state][i] = input;
                }
            }
        }
        
        for (int s = 0; s < T.numStates; s++) {
            state_metric[s] = temp_metric[s];
        }
    }
    
    int final_state = 0;
    for (int i = length - 1; i >= 0; i--) {
        output[i] = tb_input[final_state][i];
        final_state = tb_state[final_state][i];
    }
    
    return output;
}
