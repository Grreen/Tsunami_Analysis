path_real = 'YOUR_PATH' # Данные полученный с сайта с реальными мареографами
path_calculate = 'YOUR_PATH' # Данные полученный после вычислений

import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

# Чтение данных из файла
with open(path_real, 'r') as file:
    water_levels = np.array([float(line.strip()) for line in file])

with open(path_calculate, 'r') as file:
    water_levels_calculate = np.array([float(line.strip()) for line in file])

print(len(water_levels_calculate))

# Создаем массив времени
time = np.arange(len(water_levels))

# Применяем фильтр высоких частот для удаления приливной волны
cutoff_frequency = 1 / 12  # Уменьшаем частоту среза (1/12 часа)
b, a = signal.butter(3, cutoff_frequency, 'highpass', fs=1)
tsunami_filtered = signal.filtfilt(b, a, water_levels)

# Применяем медианный фильтр для удаления выбросов
window_size = 7  # Увеличиваем размер окна для медианного фильтра
tsunami_filtered = signal.medfilt(tsunami_filtered, kernel_size=window_size)

# Применяем фильтр низких частот для сглаживания
cutoff_frequency_low = 0.2  # Уменьшаем частоту среза для большего сглаживания
b_low, a_low = signal.butter(3, cutoff_frequency_low, 'lowpass', fs=1)
tsunami_filtered = signal.filtfilt(b_low, a_low, tsunami_filtered)

# Применяем дополнительное сглаживание с помощью скользящего среднего
window = signal.windows.hann(11)  # Создаем окно Ханна
tsunami_filtered = signal.convolve(tsunami_filtered, window, mode='same') / sum(window)

# Вычитаем среднее значение, чтобы колебания были относительно нуля
tsunami_filtered -= np.mean(tsunami_filtered)

# Визуализация
plt.figure(figsize=(12, 8))
plt.subplot(2, 1, 1)
plt.plot(time, water_levels)
plt.title('Исходный уровень воды')
plt.ylabel('Уровень (м)')

plt.subplot(2, 1, 2)
plt.plot(time, tsunami_filtered)
plt.plot(time, water_levels_calculate)
plt.title('Отфильтрованная волна цунами')
plt.ylabel('Уровень (м)')
plt.xlabel('Время (часы)')

plt.tight_layout()
plt.show()

