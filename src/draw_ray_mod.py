import re
import matplotlib.pyplot as plt
from collections import defaultdict

def parse_data(filename):
    """
    Парсит файл с данными в формате:
    Eb/N0 = 0
    qpsk = 0.188 qam16 = 0.332 qam64 = 0.388 qam256 = 0.475
    """
    data = defaultdict(lambda: defaultdict(list))
    
    with open(filename, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    eb_n0 = None
    for line in lines:
        line = line.strip()
        if not line:
            continue
            
        # Ищем строку с Eb/N0
        eb_match = re.match(r'Eb/N0\s*=\s*([\d.]+)', line)
        if eb_match:
            eb_n0 = float(eb_match.group(1))
            continue
            
        # Если есть Eb/N0 и строка с модуляциями
        if eb_n0 is not None and '=' in line:
            # Извлекаем все пары ключ=значение
            matches = re.findall(r'(qpsk|qam16|qam64|qam256)\s*=\s*([\d.]+)', line, re.IGNORECASE)
            for modulation, value in matches:
                val = float(value)
                # Пропускаем нулевые значения
                if val > 0:
                    data[modulation.lower()]['eb_n0'].append(eb_n0)
                    data[modulation.lower()]['ber'].append(val)
    
    return data

def plot_results(data, output_file='ber_plot.png'):
    """Строит график BER от Eb/N0 для разных схем модуляции"""
    
    # Названия для легенды и стилей
    config = {
        'qpsk':   {'label': 'QPSK',   'marker': 'o', 'color': 'blue'},
        'qam16':  {'label': '16-QAM', 'marker': 's', 'color': 'green'},
        'qam64':  {'label': '64-QAM', 'marker': '^', 'color': 'red'},
        'qam256': {'label': '256-QAM','marker': 'd', 'color': 'purple'}
    }
    
    plt.figure(figsize=(10, 6))
    
    for modulation, cfg in config.items():
        if modulation in data and data[modulation]['eb_n0']:
            plt.plot(
                data[modulation]['eb_n0'],
                data[modulation]['ber'],
                marker=cfg['marker'],
                color=cfg['color'],
                label=cfg['label'],
                linewidth=1.5,
                markersize=4
            )
    
    plt.xlabel('Отношение сигнал/шум (Eb/N0), дБ', fontsize=12)
    plt.ylabel('Вероятность ошибки на бит (BER)', fontsize=12)
    plt.title('Вероятность ошибки на бит для различных схем модуляции в Рэлеевском канале', fontsize=14, pad=20)
    plt.grid(True, which='both', linestyle='--', alpha=0.7)
    plt.legend(fontsize=10, loc='best')
    plt.yscale('log')  # Логарифмическая шкала по Y для лучшей наглядности
    plt.xlim(left=0)
    plt.tight_layout()
    
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"✅ График сохранён как '{output_file}'")
    plt.show()

if __name__ == "__main__":
    # Укажите имя вашего файла с данными
    input_file = 'res_ray_mod.txt'  # ← измените на путь к вашему файлу
    
    try:
        data = parse_data(input_file)
        if not data:
            print("⚠️ Не удалось найти данные в файле. Проверьте формат.")
        else:
            print(f"✅ Загружено данных:")
            for mod in data:
                print(f"   {mod.upper()}: {len(data[mod]['eb_n0'])} точек")
            plot_results(data)
    except FileNotFoundError:
        print(f"❌ Файл '{input_file}' не найден!")
    except Exception as e:
        print(f"❌ Ошибка: {e}")