import os
from glob import glob

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

ticks = []
for i in range(7):
    ticks.append(10**i)
for i in range(1, 10):
    ticks.append(2**i)
for i in range(1, 10):
    ticks.append(2**i * 1000)

seen = set()
ticks = sorted([x for x in ticks if not (x in seen or seen.add(x))])

# ========= Update Cargo.toml

SEPARATOR_START = '# BEGIN TICKS\n'
SEPARATOR_END = '# END TICKS\n'
HELP = '# Generated by gen_tick.py. DO NOT EDIT.\n'

feats_time = ''
feats_driver = ''
for freq in ticks:
    feature = f'tick-freq-hz-{freq:_}'
    if freq >= 1_000_000_000:
        freq_human = f"{freq / 1_000_000_000}GHz"
    elif freq >= 1_000_000:
        freq_human = f"{freq / 1_000_000}MHz"
    elif freq >= 1_000:
        freq_human = f"{freq / 1000}kHz"
    else:
        freq_human = f"{freq}Hz"

    feats_time += f"## {freq_human} Tick Rate\n"
    feats_time += f"{feature} = [\"embassy-time-driver/{feature}\"]\n"
    feats_driver += f"## {freq_human} Tick Rate\n"
    feats_driver += f"{feature} = []\n"

with open('Cargo.toml', 'r') as f:
    data = f.read()
before, data = data.split(SEPARATOR_START, maxsplit=1)
_, after = data.split(SEPARATOR_END, maxsplit=1)
with open('Cargo.toml', 'w') as f:
    f.write(before + SEPARATOR_START + HELP + feats_driver + SEPARATOR_END + after)

# ========= Update src/tick_freq_hz.rs

with open('src/tick_freq_hz.rs', 'w') as f:

    f.write('// Generated by gen_tick.py. DO NOT EDIT.\n\n')
    for hz in ticks:
        f.write(
            f'#[cfg(feature = "tick-freq-hz-{hz:_}")] pub const TICK_FREQ_HZ: u32 = {hz:_};\n')
    f.write('#[cfg(not(any(\n')
    for hz in ticks:
        f.write(f'feature = "tick-freq-hz-{hz:_}",\n')
    f.write(')))] pub const TICK_FREQ_HZ: u32 = 1_000;')


os.system('rustfmt src/tick_freq_hz.rs')