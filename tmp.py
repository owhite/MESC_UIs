#!/usr/bin/env python3

import plotMESC

p = plotMESC.PlotMESCOutput()

datafile_name = 'MESC_logdata2.txt'
datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')

(plt, fig) = p.generatePlot(datafile_name, datatypes)
