#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 16 09:26:31 2023

@author: hasith
"""

import numpy as np
import matplotlib.pyplot as plt

import pandas as pd

file = 'HAB_launch-14_10_23/payload_data/1HZ02.csv';
df = pd.read_csv(file)


## temp  vs altitude plot


plt.plot(df['msTemp(C)'][1200:-1],df['Alt(M)'][1200:-1]/1000)
plt.plot(df['extT(C)'][1200:-1],df['Alt(M)'][1200:-1]/1000)
#plt.plot(df['Alt(M)'],df['msTemp(C)']) 
plt.xlabel('Temp (C)');
plt.ylabel('Altitude (km)')

plt.legend(['internal','external'])

#plt.figure()
#plt.plot(df['GndSpd(M/S)'][1200:-1])


plt.figure()
plt.title('vertical speed vs altitude')
vs = np.array(df['VertVel(M/S)'][1200:-1])

vs_fil = np.zeros_like(vs)
window = 15

for i in range(0,len(vs)-window-1):
    #print(i)
    vs_fil[i+int(window/2)] = np.mean(vs[i:i+window])

plt.plot(df['Alt(M)'][1200:-1]/1000,vs,'.')
plt.plot(df['Alt(M)'][1200:-1]/1000,vs_fil,'.')

plt.ylabel(r'vertical speed $ms^{-1}$')
plt.xlabel('altitude (km)')
plt.legend(['data','filtered'])


### ground speed
plt.figure()
plt.title('ground speed speed vs altitude')
gs = np.array(df['GndSpd(M/S)'][1200:-1])

heading = np.array(df['Head(Deg)'][1200:-1])

gs_fil = np.zeros_like(vs)
heading_fil = np.zeros_like(vs)
window = 15

for i in range(0,len(vs)-window-1):
    #print(i)
    gs_fil[i+int(window/2)] = np.mean(gs[i:i+window])
    heading_fil[i+int(window/2)] = np.mean(heading[i:i+window])

plt.subplot(211)
plt.plot(df['Alt(M)'][1200:-1]/1000,gs,'.')
plt.plot(df['Alt(M)'][1200:-1]/1000,gs_fil,'-')

plt.ylabel(r'ground speed ($ms^{-1}$)')
plt.xlabel('altitude (km)')
plt.legend(['data','filtered'])

plt.subplot(212)
plt.plot(df['Alt(M)'][1200:-1]/1000,heading,'.')
plt.plot(df['Alt(M)'][1200:-1]/1000,heading_fil,'-')
plt.ylabel(r'heading (deg)')
plt.xlabel('altitude (km)')

plt.legend(['data','filtered'])


plt.figure()

acc_x =  np.array(df['accel x'][1200:-1]);
acc_y =  np.array(df['accel y'][1200:-1]);
acc_z = np.array(df['accel z'][1200:-1]);

plt.plot(acc_x)
plt.plot(acc_y)
plt.plot(acc_z)

a_val = np.sqrt(np.power(acc_x,2)+np.power(acc_y,2)+np.power(acc_z,2))

plt.figure()
plt.plot(a_val)



