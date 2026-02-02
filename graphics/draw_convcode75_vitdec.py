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

# теоретические данные
data_teor = read_data('teor.txt')

# экспериментальные данные
data_exp = read_data('data_convcode75_vitdec.txt')

labels_map = {
    0: 'BER (ДСК)',
    1: 'FER (ДСК)',
    2: 'BER (АБГШ)',
    3: 'FER (АБГШ)'
}

plt.figure(figsize=(10, 6))

colors = {}
exp_lines = []
teor_lines = []

# сначала рисуем эксперимент (сплошные)
for key in sorted(data_exp.keys()):
    snrs = [x for x, y in data_exp[key]]
    pbs  = [y for x, y in data_exp[key]]
    line, = plt.semilogy(snrs, pbs, marker='o', label=labels_map[key])
    colors[key] = line.get_color()
    exp_lines.append(line)

# потом теорию (пунктиром, тем же цветом)
for key in sorted(data_teor.keys()):
    snrs = [x for x, y in data_teor[key]]
    pbs  = [y for x, y in data_teor[key]]
    line, = plt.semilogy(snrs, pbs, linestyle='--', color=colors[key],
                         label=labels_map[key])
    teor_lines.append(line)

plt.xlabel('Отношение сигнал/шум, дБ')
plt.ylabel('Вероятности ошибок $P_b$ и $P_w$')
plt.title('Вероятность ошибки на бит (BER) и на слово (FER) в канале связи')
plt.grid(True, which="both", ls="--")

# Левая легенда — Практика
leg1 = plt.legend(exp_lines,
                  [labels_map[k] for k in sorted(data_exp.keys())],
                  title='Моделирование',
                  loc='lower left',
                  bbox_to_anchor=(0, 0),
                  frameon=True)

# Правая легенда — Теория
leg2 = plt.legend(teor_lines,
                  [labels_map[k] for k in sorted(data_teor.keys())],
                  title='Теория',
                  loc='lower left',
                  bbox_to_anchor=(0.15, 0),  # сдвиг вправо
                  frameon=True)

plt.gca().add_artist(leg1)  # чтобы обе легенды отображались


plt.tight_layout()
plt.show()