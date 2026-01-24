import matplotlib.pyplot as plt
import re
from matplotlib.lines import Line2D

def read_data(fname):
    data = {}
    current_key = None

    with open(fname, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            if line.startswith('v ='):
                current_key = int(line.split('=')[1].strip())
                data[current_key] = []
            else:
                match = re.match(r'SNR\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)\s+pb\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)', line)
                if match and current_key is not None:
                    snr = float(match.group(1))
                    pb = float(match.group(2))
                    data[current_key].append((snr, pb))
    return data


# экспериментальные данные
data_exp = read_data('data_uncoded.txt')

labels_map = {
    0: 'QAM-4',
    1: 'QAM-16',
    2: 'QAM-64',
    3: 'QAM-256'
}

plt.figure(figsize=(10, 6))

colors = {}
exp_lines = []
teor_lines = []

for key in sorted(data_exp.keys()):
    snrs = [x for x, y in data_exp[key]]
    pbs  = [y for x, y in data_exp[key]]
    line, = plt.semilogy(snrs, pbs, marker='o', label=labels_map[key],)
    colors[key] = line.get_color()
    exp_lines.append(line)


plt.xlabel('Отношение сигнал/шум, дБ')
plt.ylabel('Вероятность ошибки на бит $P_b$')
plt.title('Вероятность ошибки на бит (BER) для QAM-сигналов в некодированной передаче')
plt.grid(True, which="both", ls="--")

leg1 = plt.legend(exp_lines,
                  [labels_map[k] for k in sorted(data_exp.keys())],
                  title='Моделирование',
                  loc='lower left',
                  bbox_to_anchor=(0, 0),
                  frameon=True)


plt.tight_layout()
plt.show()
