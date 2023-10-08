#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct  4 00:00:10 2023

@author: hasith
"""

import numpy as np
import matplotlib.pyplot as plt

import pandas as pd

file_MSU = './NEBP-2/flight-300234064701700-2023-10-01.csv'
df_msu = pd.read_csv(file_MSU)

df_msu['fixed_dt']=pd.to_datetime(df_msu['datetime'], dayfirst=True)

plt.plot(df_msu['fixed_dt']-df_msu['fixed_dt'][0],df_msu['altitude'],'.-')


# this was seleced based on visual imputs
start_i = 380
end_i = 580


x = df_msu['fixed_dt'][start_i:end_i]-df_msu['fixed_dt'][0]
y = df_msu['altitude'][start_i:end_i]

plt.plot(x,y,'o')

dy = y[end_i-1]-y[start_i]
dt = x[end_i-1]-x[start_i]

dt = np.timedelta64(dt,'s').astype(int)
print("dt:",dt)
print("dy:",dy)

print("dy/dt (ms^-2):", dy/dt)

alt = df_msu['altitude'].to_numpy()
t = df_msu['fixed_dt'].to_numpy()

dt_s = np.zeros(t.shape[0])
dy = np.zeros(t.shape[0])

for i in range(1,t.shape[0]-2):
    dt_s[i] = np.timedelta64(t[i+1]-t[i-1],'s').astype(int)
    dy[i] = alt[i+1]-alt[i-1]



plt.figure()
plt.plot(df_msu['altitude']/1000,dy/dt_s)
plt.ylabel(r'V $(ms^{-1})$')
plt.xlabel('alt (km)')
