import matplotlib.pyplot as plt
import re

data = {}
current_key = None

with open('pb_teor_my.txt', 'r') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue

        if line.startswith('v ='):
            current_key = int(line.split('=')[1].strip())
            data[current_key] = []
        elif line == 'old':
            current_key = 'old'
            data[current_key] = []
        elif line == 'w + D0':
            current_key = 'w + D0'
            data[current_key] = []
        else:
            match = re.match(r'SNR\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)\s+pb\s*=\s*([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?)', line)
            if match and current_key is not None:
                snr = float(match.group(1))
                pb = float(match.group(2))
                data[current_key].append((snr, pb))

plt.figure(figsize=(10, 6))

for key in sorted(k for k in data.keys() if isinstance(k, int)):
    snrs = [item[0] for item in data[key]]
    pbs = [item[1] for item in data[key]]
    plt.semilogy(snrs, pbs, marker='o', label=f'v = {key}')

if 'old' in data:
    snrs_old = [item[0] for item in data['old']]
    pbs_old = [item[1] for item in data['old']]
    plt.semilogy(snrs_old, pbs_old, marker='s', linestyle='--', color='gray', label='old')

if 'w + D0' in data:
    snrs_old = [item[0] for item in data['w + D0']]
    pbs_old = [item[1] for item in data['w + D0']]
    plt.semilogy(snrs_old, pbs_old, marker='s', linestyle='--', color='black', label='w + D0')

plt.xlabel('SNR (dB)')
plt.ylabel('Bit Error Probability $p_b$')
plt.title('Theoretical Bit Error Probability vs SNR')
plt.grid(True, which="both", ls="--")
plt.legend()

all_snrs = sorted(set(item[0] for sublist in data.values() for item in sublist))
plt.xticks(all_snrs)

plt.tight_layout()
plt.show()