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

data = df[2].str.split('=')