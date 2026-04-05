import numpy as np
import matplotlib.pyplot as plt
import os
import sys

def round_value(value, attempts):
    if value == 0:
        return None
    decimals = int(np.floor(np.log10(attempts))) - 1
    decimals = max(0, decimals)
    return round(value, decimals)

def read_data(filename):
    data = []
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('EbN0'):
                continue
            parts = line.split()
            if len(parts) >= 4:
                try:
                    row = [float(x) for x in parts[:4]]
                    data.append(row)
                except ValueError:
                    continue
    return np.array(data)

def main():
    if len(sys.argv) < 2:
        print("Использование: python script.py <имя_файла_с_данными>")
        return
    
    input_file = sys.argv[1]
    base_name = os.path.splitext(os.path.basename(input_file))[0]
    
    os.makedirs('graphics', exist_ok=True)
    
    data = read_data(input_file)
    
    if len(data) == 0:
        print(f"Не найдено валидных данных в файле {input_file}")
        return
    
    ebno = data[:, 0]
    ber = data[:, 1]
    fer = data[:, 2]
    attempts = data[:, 3]
    
    ber_processed = []
    ebno_ber = []
    for i, (b_val, att) in enumerate(zip(ber, attempts)):
        rounded = round_value(b_val, att)
        if rounded is not None and rounded > 0:
            ber_processed.append(rounded)
            ebno_ber.append(ebno[i])
    
    fer_processed = []
    ebno_fer = []
    for i, (f_val, att) in enumerate(zip(fer, attempts)):
        rounded = round_value(f_val, att)
        if rounded is not None and rounded > 0:
            fer_processed.append(rounded)
            ebno_fer.append(ebno[i])
    
    if len(ebno_ber) > 0:
        plt.figure(figsize=(8, 6))
        plt.semilogy(ebno_ber, ber_processed, 'o-', markersize=5, linewidth=1.5)
        plt.xlabel('Отношение сигнал/шум (дБ), Eb/N0')
        plt.ylabel('BER')
        plt.grid(True, which='both', ls='--', alpha=0.7)
        plt.title('Вероятность ошибки на бит (BER)')
        plt.tight_layout()
        plt.savefig(f'graphics/{base_name}_BER.png', dpi=300)
        plt.close()
    
    if len(ebno_fer) > 0:
        plt.figure(figsize=(8, 6))
        plt.semilogy(ebno_fer, fer_processed, 's-', markersize=5, linewidth=1.5, color='orange')
        plt.xlabel('Отношение сигнал/шум (дБ), Eb/N0')
        plt.ylabel('Вероятность ошибки на слово')
        plt.grid(True, which='both', ls='--', alpha=0.7)
        plt.title('Вероятность ошибки на слово (FER)')
        plt.tight_layout()
        plt.savefig(f'graphics/{base_name}_FER.png', dpi=300)
        plt.close()
    
if __name__ == "__main__":
    main()