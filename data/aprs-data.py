#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct  2 08:38:33 2023
Author : Hasith Perera

"""

import numpy as np
import matplotlib.pyplot as plt

import pandas as pd


file = 'NEBP-2/Recovery-Direct_APRSAX25rcvd-2023-10-01.csv'
df = pd.read_csv(file,header=None)

#0 - time
#2 - location data



df['data'] = df[2].str.split('=')
altitude = np.zeros([df.shape[0]])
for i in range(0,df.shape[0]):
    data = df['data'][i]
    #print(data)
    
    if data[2]=='':
        altitude[i]=np.nan
        print(f'{i} !no alt')
        pass
    else:
        tmp_alt = float(data[2][0:-1])
        #print("{} float:{}".format((data[2]),tmp_alt))
        altitude[i] = tmp_alt
        
df['alt'] = altitude
df['fixed_dt']=pd.to_datetime(df[0], dayfirst=True)

plt.plot(df['fixed_dt'],df['alt'],'o-')
#plt.xticks(np.arange(0, df.shape[0], step=20),rotation='vertical')
#plt.tight_layout()

## load MSU data

#plt.figure()
file_MSU = './NEBP-2/flight-300234064701700-2023-10-01.csv'
df_msu = pd.read_csv(file_MSU)

df_msu['fixed_dt']=pd.to_datetime(df_msu['datetime'], dayfirst=True)

plt.plot(df_msu['fixed_dt'],df_msu['altitude'],'.-')
#plt.xticks(np.arange(0, df_msu.shape[0], step=40),rotation='vertical')
#plt.tight_layout()
#plt.show()



##

file_fi = 'NEBP-2/aprsfi_data.dat'

df2 = pd.DataFrame()

with open(file_fi) as fp:
    lines = fp.readlines()

date_time = []
alt = np.zeros([len(lines)])

for i,line in enumerate(lines):
    data_tmp = line.split('?')
    val = str(data_tmp[0][0:-3])
    print(val)
    date_time.append(val+"-4:00")
    
    alt[i] = float(data_tmp[1].split("=")[-1][0:-2])


df2['datetime'] = date_time;
df2['alt'] = alt

df2['fixed_dt'] = pd.to_datetime(df2['datetime'], dayfirst=True,utc=False)



plt.plot(df2['fixed_dt'],df2['alt'],'o-')

plt.xlabel('UTC time')
plt.ylabel('Altitude (m)')
plt.legend(['APRS Recovery','Iridium','APRS.fi'])