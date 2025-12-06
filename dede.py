import pandas as pd
import matplotlib.pyplot as plt

# --- 設定 ---
CSV_FILE = 'dedenne_data.csv'

# --- データの読み込み ---
# C++の出力ヘッダーに合わせて読み込みます
# "Trigger/Day, Net_Gain(Week), Value_Per_Trigger(Coeff)" を想定
try:
    df = pd.read_csv(CSV_FILE)
    # df = pd.read_csv(CSV_FILE, encoding='utf-16')
    # カラム名の余計な空白を除去
    df.columns = [c.strip() for c in df.columns]
except Exception as e:
    print(f"CSV読み込みエラー: {e}")
    print("C++の出力がCSV形式になっているか確認してください。")
    exit()

# --- グラフ描画 ---
fig, ax1 = plt.subplots(figsize=(12, 7))

# タイトルなど
plt.title('Dedenne Skill Contribution Analysis\n', fontsize=16)
plt.grid(True, which='both', linestyle='--', alpha=0.5)

# X軸: 1日あたりの発動回数
x = df.iloc[:, 0]  # Trigger/Day

# Y1軸 (左): 週間エナジー貢献度 (Net Gain)
y1 = df.iloc[:, 1]
color1 = 'tab:blue'
ax1.set_xlabel('Skill Triggers per Day', fontsize=12)
ax1.set_ylabel('Weekly Net Gain (x Base Meal Energy)', color=color1, fontsize=12)
line1, = ax1.plot(x, y1, color=color1, linewidth=2, label='Weekly Net Gain (Left)')
ax1.tick_params(axis='y', labelcolor=color1)

# Y2軸 (右): スキル1回あたりの価値 (Value Per Trigger)
ax2 = ax1.twinx()  # 2軸目の作成
y2 = df.iloc[:, 2]
color2 = 'tab:orange'
ax2.set_ylabel('Value per Trigger (Coefficient)', color=color2, fontsize=12)
line2, = ax2.plot(x, y2, color=color2, linewidth=2, linestyle='--', label='Value per Trigger (Right)')
ax2.tick_params(axis='y', labelcolor=color2)

# レジェンド（凡例）をまとめる
lines = [line1, line2]
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='center right', fontsize=10)

# 注釈: 現実的なライン (例えば1日3回発動付近)
target_x = 3.0
# データから近い値を探す
idx = (x - target_x).abs().idxmin()
val_gain = y1[idx]
val_eff = y2[idx]

text_str = f'Target ~{target_x}/day:\nGain: +{val_gain:.2f}x\nValue: {val_eff:.3f}/use'
ax1.annotate(text_str, xy=(x[idx], val_gain), xytext=(x[idx]+1, val_gain),
             arrowprops=dict(facecolor='black', shrink=0.05),
             bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="black", alpha=0.8))

plt.tight_layout()
plt.show()
# plt.savefig('dedenne_skill_analysis.png', dpi=300)