import numpy as np
import pandas as pd
import math
import matplotlib.pyplot as plt
import pywt


def features_estimation(signal, channel_name, fs, frame, step, plot=True):
    """
    Calcula características temporais, de frequência e tempo-frequência a partir do sinal.
    :param signal: sinal em forma de array numpy.
    :param channel_name: nome da canal EMG em análise.
    :param fs: taxa de amostragem usada para adquirir o sinal.
    :param frame: tamanho da janela deslizante.
    :param step: tamanho do passo da janela deslizante.
    :param plot: variável booleana para plotar as características estimadas.
    :return: total_feature_matrix -- DataFrame pandas com as características.
    :return: features_names -- lista python com os nomes das características.
    """

    features_names = ['VAR', 'RMS', 'IEMG', 'MAV', 'LOG', 'WL', 'ACC', 'DASDV', 'ZC', 'WAMP', 'MYOP', "FR", "MNP", "TP",
                      "MNF", "MDF", "PKF", "WENT"]

    time_matrix = time_features_estimation(signal, frame, step)
    frequency_matrix = frequency_features_estimation(signal, fs, frame, step)
    time_frequency_matrix = time_frequency_features_estimation(signal, frame, step)
    total_feature_matrix = pd.DataFrame(np.column_stack((time_matrix, frequency_matrix, time_frequency_matrix)).T,
                                        index=features_names)

    print('Características EMG do canal {} foram extraídas com sucesso'.format(channel_name))

    if plot:
        plot_features(signal, channel_name, fs, total_feature_matrix, step)

    return total_feature_matrix, features_names


def time_features_estimation(signal, frame, step):
    """
    Calcula características temporais do sinal usando o método de janela deslizante.
    :param signal: sinal em forma de array numpy.
    :param frame: tamanho da janela deslizante.
    :param step: tamanho do passo da janela deslizante.
    :return: time_features_matrix: matriz numpy com as características temporais empilhadas por colunas.
    """

    variance = []
    rms = []
    iemg = []
    mav = []
    log_detector = []
    wl = []
    aac = []
    dasdv = []
    zc = []
    wamp = []
    myop = []

    th = np.mean(signal) + 3 * np.std(signal)

    for i in range(frame, signal.size, step):
        x = signal[i - frame:i]

        variance.append(np.var(x))
        rms.append(np.sqrt(np.mean(x ** 2)))
        iemg.append(np.sum(abs(x)))  # Integral
        mav.append(np.sum(np.absolute(x)) / frame)  # Valor Absoluto Médio
        log_detector.append(np.exp(np.sum(np.log10(np.absolute(x))) / frame))
        wl.append(np.sum(abs(np.diff(x))))  # Comprimento de onda
        aac.append(np.sum(abs(np.diff(x))) / frame)  # Mudança média de amplitude
        dasdv.append(
            math.sqrt((1 / (frame - 1)) * np.sum((np.diff(x)) ** 2)))  # Valor absoluto do desvio padrão da diferença
        zc.append(zcruce(x, th))  # Zero-Crossing
        wamp.append(wilson_amplitude(x, th))  # Amplitude de Willison
        myop.append(myopulse(x, th))  # Taxa de porcentagem de Myopulse

    time_features_matrix = np.column_stack((variance, rms, iemg, mav, log_detector, wl, aac, dasdv, zc, wamp, myop))
    return time_features_matrix


def frequency_features_estimation(signal, fs, frame, step):
    """
    Calcula características de frequência do sinal usando o método de janela deslizante.
    :param signal: sinal em forma de array numpy.
    :param fs: taxa de amostragem do sinal.
    :param frame: tamanho da janela deslizante.
    :param step: tamanho do passo da janela deslizante.
    :return: frequency_features_matrix: matriz numpy com as características de frequência empilhadas por colunas.
    """

    fr = []
    mnp = []
    tot = []
    mnf = []
    mdf = []
    pkf = []

    for i in range(frame, signal.size, step):
        x = signal[i - frame:i]
        frequency, power = spectrum(x, fs)

        fr.append(frequency_ratio(frequency, power))  # Razão de frequência
        mnp.append(np.sum(power) / len(power))  # Potência média
        tot.append(np.sum(power))  # Potência total
        mnf.append(mean_freq(frequency, power))  # Frequência média
        mdf.append(median_freq(frequency, power))  # Frequência mediana
        pkf.append(frequency[power.argmax()])  # Frequência de pico

    frequency_features_matrix = np.column_stack((fr, mnp, tot, mnf, mdf, pkf))

    return frequency_features_matrix


def time_frequency_features_estimation(signal, frame, step):
    """
    Calcula características tempo-frequência do sinal usando o método de janela deslizante.
    :param signal: sinal em forma de array numpy.
    :param frame: tamanho da janela deslizante.
    :param step: tamanho do passo da janela deslizante.
    :return: h_wavelet: lista
    """
    h_wavelet = []

    for i in range(frame, signal.size, step):
        x = signal[i - frame:i]

        E_a, E = wavelet_energy(x, 'db2', 4)
        E.insert(0, E_a)
        E = np.asarray(E) / 100

        h_wavelet.append(-np.sum(E * np.log2(E)))

    return h_wavelet


def wilson_amplitude(signal, th):
    x = abs(np.diff(signal))
    umbral = x >= th
    return np.sum(umbral)


def myopulse(signal, th):
    umbral = signal >= th
    return np.sum(umbral) / len(signal)


def spectrum(signal, fs):
    m = len(signal)
    n = next_power_of_2(m)
    y = np.fft.fft(signal, n)
    yh = y[0:int(n / 2 - 1)]
    fh = (fs / n) * np.arange(0, n / 2 - 1, 1)
    power = np.real(yh * np.conj(yh) / n)

    return fh, power


def frequency_ratio(frequency, power):
    power_low = power[(frequency >= 30) & (frequency <= 250)]
    power_high = power[(frequency > 250) & (frequency <= 500)]
    ULC = np.sum(power_low)
    UHC = np.sum(power_high)

    return ULC / UHC


def shannon(x):
    n = len(x)
    nb = 19
    hist, bin_edges = np.histogram(x, bins=nb)
    counts = hist / n
    nz = np.nonzero(counts)

    return np.sum(counts[nz] * np.log(counts[nz]) / np.log(2))


def zcruce(X, th):
    th = 0
    cruce = 0
    for cont in range(len(X) - 1):
        can = X[cont] * X[cont + 1]
        can2 = abs(X[cont] - X[cont + 1])
        if can < 0 and can2 > th:
            cruce = cruce + 1
    return cruce


def mean_freq(frequency, power):
    num = 0
    den = 0
    for i in range(int(len(power) / 2)):
        num += frequency[i] * power[i]
        den += power[i]

    return num / den


def median_freq(frequency, power):
    power_total = np.sum(power) / 2
    temp = 0
    tol = 0.01
    errel = 1
    i = 0

    while abs(errel) > tol:
        temp += power[i]
        errel = (power_total - temp) / power_total
        i += 1
        if errel < 0:
            errel = 0
            i -= 1

    return frequency[i]


def wavelet_energy(x, mother, nivel):
    coeffs = pywt.wavedecn(x, wavelet=mother, level=nivel)
    arr, _ = pywt.coeffs_to_array(coeffs)
    et = np.sum(arr ** 2)
    ca = coeffs[0]
    ea = 100 * np.sum(ca ** 2) / et
    ed = []

    for k in range(1, len(coeffs)):
        cd = list(coeffs[k].values())
        cd = np.asarray(cd)
        ed.append(100 * np.sum(cd ** 2) / et)

    return ea, ed


def next_power_of_2(x):
    return 1 if x == 0 else 2 ** (x - 1).bit_length()


def med_freq(f, P):
    plot = np.sum(P) / 2
    temp = 0
    tol = 0.01
    errel = 1
    i = 0

    while abs(errel) > tol:
        temp += P[i]
        errel = (plot - temp) / plot
        i += 1
        if errel < 0:
            errel = 0
            i -= 1

    return f[i]


def plot_features(signal, channel_name, fs, feature_matrix, step):
    """
    Argumentos:
    signal -- array numpy representando o registro de um sinal.
    channel_name -- variável string com o nome do canal EMG em análise (Título).
    fs -- variável int com a taxa de amostragem usada para adquirir o sinal.
    feature_matrix -- DataFrame pandas.
    step -- variável int com o tamanho do passo usado no método de janela deslizante.
    """

    ts = np.arange(0, len(signal) / fs, 1 / fs)
    # para idx, f in enumerate(tfeatures.T):
    for key in feature_matrix.T:
        tf = step * (np.arange(0, len(feature_matrix.T[key]) / fs, 1 / fs))
        fig = plt.figure()

        ax = fig.add_subplot(111, label="1")
        ax2 = fig.add_subplot(111, label="2", frame_on=False)
        ax.plot(ts, signal, color="C0")
        ax.autoscale(tight=True)
        plt.title(channel_name + ": " + key)
        ax.set_xlabel("Tempo")
        ax.set_ylabel("mV")

        ax2.plot(tf, feature_matrix.T[key], color="red")
        ax2.yaxis.tick_right()
        ax2.autoscale(tight=True)
        ax2.set_xticks([])
        ax2.set_yticks([])
        # mng = plt.get_current_fig_manager()
        # mng.window.state('zoomed')
        plt.show()
