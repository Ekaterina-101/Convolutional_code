#include "convolution-coding.hpp"
#include "crc.hpp"

std::vector<std::vector<int>> hard_parallel_lva(const Trellis &T, std::vector<int> &encoded, int L) {
    const int R = T.n;
    const int N = T.numStates;
    const size_t length = encoded.size() / R;

    // phi[state][k] — k-я наименьшая накопленная метрика для состояния state (φ_t(i,k) в статье)
    std::vector<std::vector<int>> phi(N, std::vector<int>(L, INT_MAX));
    phi[0][0] = 0; // старт в нулевом состоянии, лучший путь имеет нулевую метрику

    // tb_state[state][k][t] — предшествующее состояние j* для k-го пути в state на шаге t (ξ_t(i,k))
    std::vector<std::vector<std::vector<int>>> tb_state(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));

    // tb_rank[state][k][t] — ранг l* предшествующего пути (γ_t(i,k))
    std::vector<std::vector<std::vector<int>>> tb_rank(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));

    // tb_input[state][k][t] — входной бит, приведший в state k-м путём на шаге t
    std::vector<std::vector<std::vector<int>>> tb_input(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));

    for (int i = 0; i < length; i++) {
        int received_symbol = 0;
        for (int c = 0; c < R; c++) {
            received_symbol |= (encoded[R * i + c] << (R - c - 1));
        }

        // temp_phi[state][k] — кандидаты в k-е лучшие метрики для каждого состояния на текущем шаге
        std::vector<std::vector<int>> temp_phi(N, std::vector<int>(L, INT_MAX));
        std::vector<std::vector<int>> temp_tb_state(N, std::vector<int>(L, -1));
        std::vector<std::vector<int>> temp_tb_rank(N, std::vector<int>(L, -1));
        std::vector<std::vector<int>> temp_tb_input(N, std::vector<int>(L, -1));

        for (int curr_state = 0; curr_state < N; curr_state++) {
            for (int input = 0; input < 2; input++) {
                int next_state = T.nextStates[curr_state][input];
                int output_symbol = T.outputs[curr_state][input];
                int branch_metric = __builtin_popcount(received_symbol ^ output_symbol);

                // перебираем все L путей из curr_state — это реализует min^(k) по всем j,l из статьи
                for (int l = 0; l < L; l++) {
                    int curr_metric = phi[curr_state][l];

                    if (curr_metric == INT_MAX) {
                        continue; // путь недостижим
                    }
                    int new_metric = curr_metric + branch_metric;

                    // вставляем new_metric в отсортированный список temp_phi[next_state]
                    // ищем позицию куда вставить, сдвигая худшие метрики вправо
                    for (int k = 0; k < L; k++) {
                        if (new_metric < temp_phi[next_state][k]) {
                            // сдвигаем всё начиная с позиции k вправо на одну позицию
                            for (int m = L - 1; m > k; m--) {
                                temp_phi[next_state][m] = temp_phi[next_state][m - 1];
                                temp_tb_state[next_state][m] = temp_tb_state[next_state][m - 1];
                                temp_tb_rank[next_state][m] = temp_tb_rank[next_state][m - 1];
                                temp_tb_input[next_state][m] = temp_tb_input[next_state][m - 1];
                            }
                            // вставляем новый кандидат на позицию k (j* и l* из статьи)
                            temp_phi[next_state][k] = new_metric;
                            temp_tb_state[next_state][k] = curr_state;
                            temp_tb_rank[next_state][k] = l;
                            temp_tb_input[next_state][k] = input;
                            break;
                        }
                    }
                }
            }
        }

        phi = temp_phi;

        // сохраняем историю путей для обратного отслеживания на шаге i
        for (int s = 0; s < N; s++) {
            for (int k = 0; k < L; k++) {
                tb_state[s][k][i] = temp_tb_state[s][k];
                tb_rank[s][k][i] = temp_tb_rank[s][k];
                tb_input[s][k][i] = temp_tb_input[s][k];
            }
        }
    }

    // обратное отслеживание L путей из конечного состояния 0
    std::vector<std::vector<int>> result(L, std::vector<int>(length));

    for (int k = 0; k < L; k++) {
        int state = 0;
        int rank = k; // начинаем с k-го пути в конечном состоянии

        for (int i = length - 1; i >= 0; i--) {
            result[k][i] = tb_input[state][rank][i];

            int prev_state = tb_state[state][rank][i];
            int prev_rank = tb_rank[state][rank][i]; // переходим к рангу l* предыдущего шага (γ_t)
            state = prev_state;
            rank = prev_rank;
        }
    }

    return result;
}

std::vector<std::vector<int>> soft_parallel_lva(const Trellis &T, std::vector<double> &encoded, int L) {
    const int R = T.n;
    const int N = T.numStates;
    size_t length = encoded.size() / R;

    std::vector<std::vector<double>> phi(N, std::vector<double>(L, std::numeric_limits<double>::infinity()));
    phi[0][0] = 0.0;

    std::vector<std::vector<std::vector<int>>> tb_state(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));
    std::vector<std::vector<std::vector<int>>> tb_rank(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));
    std::vector<std::vector<std::vector<int>>> tb_input(N, std::vector<std::vector<int>>(L, std::vector<int>(length, -1)));

    for (int i = 0; i < length; i++) {
        std::vector<std::vector<double>> temp_phi(N, std::vector<double>(L, std::numeric_limits<double>::infinity()));
        std::vector<std::vector<int>> temp_tb_state(N, std::vector<int>(L, -1));
        std::vector<std::vector<int>> temp_tb_rank(N, std::vector<int>(L, -1));
        std::vector<std::vector<int>> temp_tb_input(N, std::vector<int>(L, -1));

        for (int curr_state = 0; curr_state < N; curr_state++) {
            for (int input = 0; input < 2; input++) {
                int next_state = T.nextStates[curr_state][input];
                int output_symbol = T.outputs[curr_state][input];

                double correlation = 0.0;
                for (int c = 0; c < R; c++) {
                    int bit = (output_symbol >> (R - c - 1)) & 1;
                    correlation += (1 - 2 * bit) * encoded[R * i + c];
                }
                double branch_metric = -correlation;

                for (int l = 0; l < L; l++) {
                    double curr_metric = phi[curr_state][l];

                    if (curr_metric == std::numeric_limits<double>::infinity()) {
                        continue;
                    }
                    double new_metric = curr_metric + branch_metric;

                    for (int k = 0; k < L; k++) {
                        if (new_metric < temp_phi[next_state][k]) {
                            for (int m = L - 1; m > k; m--) {
                                temp_phi[next_state][m] = temp_phi[next_state][m - 1];
                                temp_tb_state[next_state][m] = temp_tb_state[next_state][m - 1];
                                temp_tb_rank[next_state][m] = temp_tb_rank[next_state][m - 1];
                                temp_tb_input[next_state][m] = temp_tb_input[next_state][m - 1];
                            }

                            temp_phi[next_state][k] = new_metric;
                            temp_tb_state[next_state][k] = curr_state;
                            temp_tb_rank[next_state][k] = l;
                            temp_tb_input[next_state][k] = input;

                            break;
                        }
                    }
                }
            }
        }

        phi = temp_phi;

        for (int s = 0; s < N; s++) {
            for (int k = 0; k < L; k++) {
                tb_state[s][k][i] = temp_tb_state[s][k];
                tb_rank[s][k][i] = temp_tb_rank[s][k];
                tb_input[s][k][i] = temp_tb_input[s][k];
            }
        }
    }

    std::vector<std::vector<int>> result(L, std::vector<int>(length));

    for (int k = 0; k < L; k++) {
        int state = 0;
        int rank = k;

        for (int i = length - 1; i >= 0; i--) {
            result[k][i] = tb_input[state][rank][i];

            int prev_state = tb_state[state][rank][i];
            int prev_rank = tb_rank[state][rank][i];

            state = prev_state;
            rank = prev_rank;
        }
    }

    return result;
}

std::vector<int> soft_parallel_lva_final(const Trellis &T, std::vector<double> &encoded, int L, int CRC, int size_block_CRC) {
    auto res = soft_parallel_lva(T, encoded, L);
    std::vector<bool> CRC_status;
    std::vector<int> decode_CRC;
    for (std::vector<int> word : res) {
        if (size_block_CRC != 0) {
            switch (CRC) {
            case 8: {
                CRC8 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            case 16: {
                CRC16 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            case 24: {
                CRC24 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            }
        } else {
            std::vector<int> CRC_decode;

            switch (CRC) {
            case 8: {
                CRC8 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            case 16: {
                CRC16 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            case 24: {
                CRC24 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            }
        }
    }
    return res[0];
}

std::vector<int> hard_parallel_lva_final(const Trellis &T, std::vector<int> &encoded, int L, int CRC, int size_block_CRC) {
    auto res = hard_parallel_lva(T, encoded, L);
    std::vector<bool> CRC_status;
    std::vector<int> decode_CRC;
    for (std::vector<int> word : res) {
        if (size_block_CRC != 0) {
            switch (CRC) {
            case 8: {
                CRC8 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            case 16: {
                CRC16 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            case 24: {
                CRC24 crc;
                if (crc.decodeBlocks(word, size_block_CRC, decode_CRC, CRC_status)) {
                    return word;
                }
                break;
            }
            }
        } else {
            std::vector<int> CRC_decode;

            switch (CRC) {
            case 8: {
                CRC8 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            case 16: {
                CRC16 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            case 24: {
                CRC24 crc;
                if (crc.decodeCRC(word)) {
                    return word;
                }
                break;
            }
            }
        }
    }
    return res[0];
}