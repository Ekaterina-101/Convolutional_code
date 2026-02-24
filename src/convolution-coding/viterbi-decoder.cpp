#include "convolution-coding.hpp"

const int hamming_distance[8][8] = {
    {0, 1, 1, 2, 1, 2, 2, 3},
    {1, 0, 2, 1, 2, 1, 3, 2},
    {1, 2, 0, 1, 2, 3, 1, 2},
    {2, 1, 1, 0, 3, 2, 2, 1},
    {1, 2, 2, 3, 0, 1, 1, 2},
    {2, 1, 3, 2, 1, 0, 2, 1},
    {2, 3, 1, 2, 1, 2, 0, 1},
    {3, 2, 2, 1, 2, 1, 1, 0},
};

std::vector<int> hard_viterbi_decode(std::vector<int>& encoded, trellis75& T) {
    const int R = std::log2(T.numOutputSymbols);
    const int N = T.numStates;
    size_t length = encoded.size() / R;
    std::vector<int> output(length);
    
    int state_metric[N] {};
    std::vector<std::vector<int>> tb_state(N, std::vector<int>(length));
    std::vector<std::vector<int>> tb_input(N, std::vector<int>(length));

    for (int i = 0; i < length; i++) {
        int received_symbol = 0;

        for (int c = 0; c < R; c++) {
            received_symbol |= (encoded[R * i + c] << (R - c - 1));
        }
        std::vector<int> temp_metric(N, INT_MAX);

        for (int curr_state = 0; curr_state < N; curr_state++) {
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

        for (int s = 0; s < N; s++) {
            state_metric[s] = temp_metric[s];
        }
    }

    int state = 0;
    for (int i = length - 1; i >= 0; i--) {
        output[i] = tb_input[state][i];
        state = tb_state[state][i];
    }
    
    return output;
}

std::vector<int> soft_viterbi_decode(std::vector<double>& encoded, trellis75& T) {
    const int R = std::log2(T.numOutputSymbols);
    const int N = T.numStates;
    size_t length = encoded.size() / R;
    std::vector<int> output(length);

    double state_metric[N] {};
    std::vector<std::vector<int>> tb_state(N, std::vector<int>(length));
    std::vector<std::vector<int>> tb_input(N, std::vector<int>(length));

    for (int i = 0; i < length; i++) {
        std::vector<double> temp_metric(N, INT_MAX);

        for (int curr_state = 0; curr_state < N; curr_state++) {
            double curr_metric = state_metric[curr_state];

            for (int input = 0; input < 2; input++) {
                int next_state = T.nextStates[curr_state][input];
                int output_symbol = T.outputs[curr_state][input];

                double correlation = 0;
                for (int c = 0; c < R; c++) {
                    int bit = (output_symbol >> (R - c - 1)) & 1;
                    correlation += (1 - 2 * bit) * encoded[R * i + c];
                }
                double new_metric = curr_metric - correlation;
                
                if (new_metric < temp_metric[next_state]) {
                    temp_metric[next_state] = new_metric;
                    
                    tb_state[next_state][i] = curr_state;
                    tb_input[next_state][i] = input;
                }
            }
        }
        
        for (int s = 0; s < N; s++) {
            state_metric[s] = temp_metric[s];
        }
    }
    
    int state = 0;
    for (int i = length - 1; i >= 0; i--) {
        output[i] = tb_input[state][i];
        state = tb_state[state][i];
    }
    
    return output;
}
