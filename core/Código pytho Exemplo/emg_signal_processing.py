from scipy import signal
import matplotlib.pyplot as plt
import numpy as np

def notch_filter(x, samplerate, plot=False):
    # Centralizar o sinal subtraindo a média
    x = x - np.mean(x)

    # Definir as frequências de corte do filtro notch
    high_cutoff_notch = 59 / (samplerate / 2)
    low_cutoff_notch = 61 / (samplerate / 2)

    # Filtro Band Stop (BSF) ou Band Reject Filter
    [b, a] = signal.butter(4, [high_cutoff_notch, low_cutoff_notch], btype='stop')

    # Aplicar o filtro notch
    x_filt = signal.filtfilt(b, a, x.T)

    # Plotar o sinal original e filtrado se a opção de plotar estiver ativada
    if plot:
        t = np.arange(0, len(x) / samplerate, 1 / samplerate)
        plt.plot(t, x)
        plt.plot(t, x_filt.T, 'k')
        plt.autoscale(tight=True)
        plt.xlabel('Tempo')
        plt.ylabel('Amplitude (mV)')
        plt.show()

    return x_filt

def bp_filter(x, low_f, high_f, samplerate, plot=False):
    # Definir as frequências de corte do filtro passa-banda
    low_cutoff_bp = low_f / (samplerate / 2)
    high_cutoff_bp = high_f / (samplerate / 2)

    # Filtro Butterworth passa-banda de ordem 5
    [b, a] = signal.butter(5, [low_cutoff_bp, high_cutoff_bp], btype='bandpass')

    # Aplicar o filtro passa-banda
    x_filt = signal.filtfilt(b, a, x)

    # Plotar o sinal original e filtrado se a opção de plotar estiver ativada
    if plot:
        t = np.arange(0, len(x) / samplerate, 1 / samplerate)
        plt.plot(t, x)
        plt.plot(t, x_filt, 'k')
        plt.autoscale(tight=True)
        plt.xlabel('Tempo')
        plt.ylabel('Amplitude (mV)')
        plt.show()

    return x_filt

def plot_signal(x, samplerate, chname):
    # Criar vetor de tempo
    t = np.arange(0, len(x) / samplerate, 1 / samplerate)
    
    # Plotar o sinal no domínio do tempo
    plt.plot(t, x)
    plt.autoscale(tight=True)
    plt.xlabel('Tempo')
    plt.ylabel('Amplitude (mV)')
    plt.title(chname)
    plt.show()
