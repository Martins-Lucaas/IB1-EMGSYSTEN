import pandas as pd
from emg_signal_processing import bp_filter, notch_filter, plot_signal
from emg_extract import features_estimation
from pathlib import Path

# Carregar dados do arquivo Excel
caminho_sinal = Path('IB1-EMGSYSTEN/dados/emg.xlsx')
sinal_emg = pd.read_excel(caminho_sinal).values
nome_canal = 'MÃO DIREITA'

# Frequência de amostragem de 2000 (2000 amostras por segundo)
frequencia_amostragem = 2e3
quadro = 500
passo = 250

# Plotar sinal sEMG bruto
plot_signal(sinal_emg, frequencia_amostragem, nome_canal)

sinal_emg = sinal_emg.reshape((sinal_emg.size,))
# Filtro Band Stop (BSF)
sinal_filtrado = notch_filter(sinal_emg, frequencia_amostragem,
                               True)
# Filtro Passa-Banda (BPF)
sinal_filtrado = bp_filter(sinal_filtrado, 10, 500,
                            frequencia_amostragem, True)

# Extração de Características do EMG
caracteristicas_emg, nomes_caracteristicas = features_estimation(sinal_filtrado, nome_canal,
                                                   frequencia_amostragem, quadro, passo)
