#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import matplotlib.pyplot as plt
import re
import numpy as np
from matplotlib.ticker import LogFormatterSciNotation

def parse_file(filename):
    series = []
    current_label = None
    current_x = []
    current_y = []
    
    data_pattern = re.compile(r'=\s*([+-]?\d+\.?\d*)')
    
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                
                matches = data_pattern.findall(line)
                
                if len(matches) >= 2:
                    try:
                        x_val = float(matches[0])
                        y_val = float(matches[1])
                        current_x.append(x_val)
                        current_y.append(y_val)
                    except ValueError:
                        pass
                else:
                    if current_label is not None and current_x:
                        series.append((current_label, current_x, current_y))
                    
                    current_label = line
                    current_x = []
                    current_y = []
        
        if current_label is not None and current_x:
            series.append((current_label, current_x, current_y))
            
    except FileNotFoundError:
        raise FileNotFoundError(f"Файл '{filename}' не найден")
    except Exception as e:
        raise Exception(f"Ошибка при чтении файла: {e}")
    
    return series

def plot_data(series, log_y=True, log_x=False, log_base=10, title=None, xlabel=None, ylabel=None):
    plt.figure(figsize=(12, 7))
    
    has_data = False
    
    for label, x_vals, y_vals in series:
        x_arr = np.array(x_vals)
        y_arr = np.array(y_vals)
        
        if log_y:
            y_plot = y_arr.copy()
            y_plot[y_arr <= 0] = np.nan
            
            if log_x:
                x_plot = x_arr.copy()
                x_plot[x_arr <= 0] = np.nan
                valid_mask = ~np.isnan(x_plot) & ~np.isnan(y_plot)
                x_plot = x_plot[valid_mask]
                y_plot = y_plot[valid_mask]
            else:
                valid_mask = ~np.isnan(y_plot)
                x_plot = x_arr[valid_mask]
                y_plot = y_plot[valid_mask]
                
            if len(x_plot) > 0:
                plt.plot(x_plot, y_plot, marker='o', markersize=4, label=label, linewidth=1.5, alpha=0.8)
                has_data = True
            else:
                print(f"Серия '{label}' пропущена: нет положительных значений для логарифмической шкалы.")
                
        else:
            plt.plot(x_arr, y_arr, marker='o', markersize=4, label=label, linewidth=1.5, alpha=0.8)
            has_data = True
    
    if not has_data:
        print("Ошибка: не удалось построить ни одной серии данных.")
        return

    if log_x:
        plt.xscale('log', base=log_base)
        plt.xlabel(xlabel if xlabel else 'Аргумент (логарифмическая шкала)', fontsize=12)
    else:
        plt.xlabel(xlabel if xlabel else 'Аргумент', fontsize=12)
    
    if log_y:
        plt.yscale('log', base=log_base)
        plt.ylabel(ylabel if ylabel else 'Значение (логарифмическая шкала)', fontsize=12)
    else:
        plt.ylabel(ylabel if ylabel else 'Значение', fontsize=12)
    
    plt.grid(True, which='both', alpha=0.4, linestyle='--', linewidth=0.5)
    plt.grid(True, which='major', alpha=0.7, linestyle='-', linewidth=0.8)
    
    if log_y:
        plt.gca().yaxis.set_major_formatter(LogFormatterSciNotation(labelOnlyBase=False))
    if log_x:
        plt.gca().xaxis.set_major_formatter(LogFormatterSciNotation(labelOnlyBase=False))
    
    plt.title(title if title else 'График данных (Логарифмический масштаб)', fontsize=14, fontweight='bold')
    plt.legend(loc='best', framealpha=0.9)
    plt.tight_layout()
    plt.show()

def main():
    parser = argparse.ArgumentParser(
        description='Построение графика по данным из файла (по умолчанию в логарифмическом масштабе)'
    )
    parser.add_argument(
        'filename',
        type=str,
        help='Путь к файлу с данными'
    )
    parser.add_argument(
        '--linear',
        action='store_true',
        help='Использовать линейный масштаб вместо логарифмического (по оси Y)'
    )
    parser.add_argument(
        '--log-x',
        action='store_true',
        help='Использовать логарифмический масштаб также для оси X'
    )
    parser.add_argument(
        '--base', '-b',
        type=float,
        default=10,
        help='Основание логарифма (по умолчанию: 10, доступно 2, 10, 2.718...)'
    )
    parser.add_argument(
        '--title', '-t',
        type=str,
        default=None,
        help='Название всего графика'
    )
    parser.add_argument(
        '--xlabel', '-x',
        type=str,
        default=None,
        help='Подпись для оси X'
    )
    parser.add_argument(
        '--ylabel', '-y',
        type=str,
        default=None,
        help='Подпись для оси Y'
    )
    
    args = parser.parse_args()
    
    try:
        series = parse_file(args.filename)
        
        if not series:
            print("Ошибка: не найдено данных для построения графика")
            return
        
        print(f"Найдено серий данных: {len(series)}")
        for label, x, y in series:
            print(f"  - {label}: {len(x)} точек")
        
        if not args.linear:
            zero_count = sum(1 for _, _, y in series for val in y if val <= 0)
        
        log_y = not args.linear
        plot_data(series, log_y=log_y, log_x=args.log_x, log_base=args.base,
                  title=args.title, xlabel=args.xlabel, ylabel=args.ylabel)
        
    except FileNotFoundError as e:
        print(f"Ошибка: {e}")
    except Exception as e:
        print(f"Критическая ошибка: {e}")

if __name__ == '__main__':
    main()