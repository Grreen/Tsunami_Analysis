import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib

ffont = {'family' : 'Times New Roman', 'size' : 90}

matplotlib.rc('font', **ffont)

path = 'PATH_TO_FOLDER_WITH_MAREGRAMMS'
size = 70
i = 0
#matplotlib.rcParams.update({'font.size': 22})
while True:
    try:
        data = pd.read_table(path + 'mareogram%s.mrg' %str(i))
        ind = data.columns[0]
        print(f'{ind} max: {max(data[ind][1:])}')
        plt.figure(figsize=(45, 19.5))
        plt.plot(25 * np.arange(data[ind][0])/ 60,data[ind][1:],color='blue',linewidth=3)
        plt.minorticks_on()
        #plt.suptitle(title, fontsize=size, font="Times New Roman", weight='bold')
        plt.text(0, 0, ind, fontsize=size, font="Times New Roman", weight='bold')
        plt.xlabel('Time, min', fontsize=size, font="Times New Roman", weight='bold')
        plt.ylabel('Amplitude, m', fontsize=size, font="Times New Roman", weight='bold')
        plt.rc('Times New Roman')
        plt.tick_params(labelsize=size)
        plt.grid(which='major',color='black',linewidth=3)
        plt.grid(which='minor', color='grey', linewidth=1)
        plt.savefig(path + '%s.png' %ind)
        i += 1
    except ValueError:
        print("Fail!")

