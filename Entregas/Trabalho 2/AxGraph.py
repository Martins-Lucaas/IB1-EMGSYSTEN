import serial
import matplotlib.pyplot as plt
import keyboard

def check_escape():
    return keyboard.is_pressed('esc')

try:
    ser = serial.Serial('COM3', 115200)
    print("Deu Certo:")
except serial.SerialException as e:
    print("Erro ao abrir porta serial:", e)
    exit()

plt.ion()

x = []
y = []

try:
    while True:
        if check_escape():
            print("Tecla 'esc' pressionada. Saindo...")
            break

        try:
            data = ser.readline().decode().strip()
            value = int(data)
            x.append(len(x))
            y.append(value)
            plt.clf()
            plt.plot(x, y)
            plt.xlabel('Tempo')
            plt.ylabel('Valor do Potenciômetro')
            plt.pause(0.05)
        except (ValueError, UnicodeDecodeError) as e:
            print("Erro ao decodificar dados:", e)
            break
except KeyboardInterrupt:
    pass
finally:
    ser.close()
