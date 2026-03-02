import matplotlib.pyplot as plt
import re

def parse_data_file(filename):
    sections = {}
    current_section = None

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                # Определяем заголовок секции
                if line.startswith('#-----'):
                    current_section = line.replace('#-----', '').strip().split('-----')[0].strip()
                    sections[current_section] = {'SNR': [], 'pb': []}
                continue

            # Извлекаем SNR и pb
            match = re.match(r'SNR\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)\s+pb\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)', line)
            if match and current_section:
                snr = float(match.group(1))
                pb = float(match.group(2))
                # Пропускаем некорректные значения (например, 0.0e+00 в первом блоке может быть placeholder'ом)
                if pb == 0.0 and 'soft FER' in current_section:
                    continue  # или можно оставить, если нужно
                sections[current_section]['SNR'].append(snr)
                sections[current_section]['pb'].append(pb)

    return sections

# Чтение и парсинг данных
data = parse_data_file('data_p_soft.txt')

# Построение графика
plt.figure(figsize=(10, 6))

colors = ['red', 'blue', 'green', 'purple']
styles = ['o-', 's--', 'd-.', '*:']

for idx, (section, values) in enumerate(data.items()):
    label = section
    color = colors[idx % len(colors)]
    style = styles[idx % len(styles)]
    plt.plot(values['SNR'], values['pb'], style, color=color, label=label, markersize=6)

plt.yscale('log')
plt.xlabel('Отношение сигнал/шум ($E_b/N_0$), дБ')
plt.ylabel('Вероятность $P_b$ ошибки на бит')
plt.title('Вероятность $P_b$ ошибки на бит в канале c АБГШ при декодировании по алгоритму Витерби')
plt.grid(True, which="both", ls="--")
plt.legend()
plt.tight_layout()
plt.show()