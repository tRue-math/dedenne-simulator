import pandas as pd
import matplotlib.pyplot as plt

# --- 設定 ---
CSV_FILE = 'dedenne_data_cap.csv'

# --- データの読み込み ---
# C++の出力ヘッダーに合わせて読み込みます
# "Trigger/Day, Net_Gain_30%(Week), Rest_Rate_30%, ... , Net_Gain_70%(Week), Rest_Rate_70%" を想定
try:
    # df = pd.read_csv(CSV_FILE)
    df = pd.read_csv(CSV_FILE, encoding='utf-16')
    # カラム名の余計な空白を除去
    df.columns = [c.strip() for c in df.columns]
except Exception as e:
    print(f"CSV読み込みエラー: {e}")
    print("C++の出力がCSV形式になっているか確認してください。")
    exit()

# --- グラフ描画 ---
fig, ax1 = plt.subplots(figsize=(12, 7))

# タイトルなど
plt.title('Dedenne Cap Analysis\n', fontsize=16)
plt.grid(True, which='both', linestyle='--', alpha=0.5)

# X軸: 1日あたりの発動回数
x = df.iloc[:, 0]  # Trigger/Day

# Y1軸 (左): 週間エナジー貢献度 (Net Gain)
y1s = [df.iloc[:, 1+i*2] for i in range(5)]  # 各キャップのNet Gain列
color1 = 'tab:blue'
ax1.set_xlabel('Skill Triggers per Day', fontsize=12)
ax1.set_ylabel('Weekly Net Gain (x Base Meal Energy)', color=color1, fontsize=12)
line1s = [ax1.plot(x, y1s[i], linewidth=2, label=f'Gain {30+i*10}%')[0] for i in range(5)]
ax1.tick_params(axis='y', labelcolor=color1)

# Y2軸 (右): デデンネ休憩率 (Dedenne Rest Rate)
ax2 = ax1.twinx()  # 2軸目の作成
y2s = [df.iloc[:, 2+i*2] for i in range(5)]  # 各キャップのRest Rate列
color2 = 'tab:orange'
ax2.set_ylabel('Dedenne Rest Rate', color=color2, fontsize=12)
line2s = [ax2.plot(x, y2s[i], linewidth=2, linestyle=':', label=f'Rest {30+i*10}%')[0] for i in range(5)]
ax2.tick_params(axis='y', labelcolor=color2)

# レジェンド（凡例）をまとめる
leg1 = ax1.legend(handles=line1s, loc='upper left', fontsize=10, title='Weekly Net Gain')
ax1.add_artist(leg1)
ax1.legend(handles=line2s, loc='center left', fontsize=10, title='Dedenne Rest Rate')

plt.tight_layout()
# plt.show()
plt.savefig('dedenne_cap_analysis.png', dpi=300)