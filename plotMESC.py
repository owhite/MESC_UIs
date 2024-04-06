#! /usr/bin/env python3

import getopt
import json
import os
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.patches import Rectangle


class PlotMESCOutput:
    def __init__(self):
        pass

    def openFile(self, dname):
        data_dict = {}

        with open(dname, 'r') as file:
            key = None
            value = []

            for line in file:
                line = line.strip()

                # Check if the line starts with '[' and ends with ']'
                if line.startswith('[') and line.endswith(']'):
                    if key is not None:
                        data_dict[key] = "\n".join(value)
                        value = []

                    key = line[1:-1]  # Remove brackets
                else:
                    value.append(line)

            if key is not None:
                data_dict[key] = "\n".join(value)

            return(data_dict)

    def loadDataFrame(self, d):
        # These creep in for squirrely reasons
        d = d.replace("}{", "}\n{") 
        json_lines = d.strip().split('\n')
        data = [json.loads(line) for line in json_lines]
        df = pd.DataFrame(data)
        return(df)

    def loadGetResults(self, str):
        d = {}
        rows = str.split("\n")
        for row in rows:
            elements = row.split("\t")
            key = elements[0]
            value = elements[1:]
            d[key] = value

        return(d)

    # pass the file that serves as source of data
    def generatePlot(self, data_file, datatypes):
        dict = self.openFile(data_file)

        if not dict.get("JSON BLOCK"):
            print("something is wrong with json, returning")
            return(None, None)
                
        df = self.loadDataFrame(dict["JSON BLOCK"])

        # this is what comes back from a "get" call to Jens term
        if not dict.get("get"):
            print("something is wrong with get, returning")
            return(None, None)
        results = self.loadGetResults(dict["get"])

        df['phaseA'] = np.sqrt( (df['id'] * df['id']) + (df['iq'] * df['iq']) )

        t = np.arange(len(df['ehz']))

        peak_tmos = int(df['TMOS'].max())
        peak_tmot = int(df['TMOT'].max())
        peak_amps = int(df['phaseA'].max())

        fig, host = plt.subplots()
        fig.subplots_adjust(right=0.75)
        fig.set_figheight(7)
        fig.set_figwidth(9)
    
        # display errors, if any
        for x, e in  enumerate(df['error']):
            if e != 0:
                rect_x = x  
                rect_y = 0  
                rect_width = int(len(df['ehz']) * .04) 
                rect_height = df['ehz'].max()

                # rect = Rectangle((rect_x, rect_y), rect_width, rect_height, linewidth=1, edgecolor='y', facecolor='y')

        ax1 = host.twinx()
        ax2 = host.twinx()
        ax3 = host.twinx()
        ax2.spines.right.set_position(("axes", 1.1))
        ax3.spines.right.set_position(("axes", 1.2))
    
        title = ("{0}A, FW={1}".format(int(float(results['curr_max'][2])),
                                            int(float(results['fw_curr'][0]))))
        plt.title(title)
        # plt.figtext(0.85, 0.01, text, fontsize=12, color='black', ha='right')

        text = F"peak tmos={peak_tmos}\npeak tmot={peak_tmot}\npeak Amps={peak_amps}"
        plt.figtext(0.01, 0.01, text, fontsize=12, color='black', ha='left')

        datatype = datatypes[0]
        color = 'tab:red'
        host.set_ylim(0, df[datatype].max() + (.05 * df[datatype].max()))
        host.set_xlabel("Samples", color='black')
        host.tick_params(axis='y', labelcolor=color)
        host.set_ylabel(datatype, color=color)
        host.plot(t, df[datatype], color=color, label = datatype)
        fig.legend(loc = "upper left")

        # Add the rectangle patch to the plot
        # host.add_patch(rect)

        datatype = datatypes[1]
        span = df[datatype].max() - df[datatype].min()
        plt_min = df[datatype].min() - (0.05 * span)
        plt_max = df[datatype].max() + (0.05 * span)

        if datatype != 'adc1':
            plt_min = 0

        color = 'tab:blue'
        datatype = datatype
        ax1.set_ylabel(datatype, color=color)  
        ax1.tick_params(axis='y', labelcolor=color)
        ax1.set_ylim(0, plt_max)
        ax1.plot(t, df[datatype], color=color, label = datatype)
        fig.legend(loc = "upper left")

        datatype = datatypes[2]
        span = df[datatype].max() - df[datatype].min()
        plt_min = df[datatype].min() - (0.05 * span)
        plt_max = df[datatype].max() + (0.05 * span)

        if datatype != 'adc1':
            plt_min = 0

        color = 'tab:green'
        ax2.set_ylabel(datatype, color=color)  
        ax2.tick_params(axis='y', labelcolor=color)
        ax2.set_ylim(0, plt_max)
        ax2.plot(t, df[datatype], color=color, label = datatype)
        fig.legend(loc = "upper left")

        datatype = datatypes[3]
        span = df[datatype].max() - df[datatype].min()
        plt_min = df[datatype].min() - (0.05 * span)
        plt_max = df[datatype].max() + (0.05 * span)

        if datatype != 'adc1':
            plt_min = 0

        color = 'black'
        ax3.set_ylabel(datatype, color=color)  
        ax3.tick_params(axis='y', labelcolor=color)
        ax3.set_ylim(plt_min, plt_max)
        ax3.plot(t, df[datatype], color=color, label = datatype)
        fig.legend(loc = "upper left")

        return(plt, fig)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:],"d:o:t:",["datafile=", "outputfile=", "types="])
    except getopt.GetoptError:
        print ('program.py -d <datafile> -o <output> -t <types>')
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-d", "--datafile"):
            dname = arg
        if opt in ("-o", "--outputfile"):
            oname = arg
        if opt in ("-t", "--types"):
            type = arg

    datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')

    if type == "iq": # doesnt do anything yet
        datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
    if type == "temp":
        datatypes = ('ehz', 'phaseA', 'TMOS', 'TMOT')
    if type == "vbus": 
        datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
        

    #{"adc1", "ehz", "error", "id", "iq", "iqreq","TMOS", "TMOT", "vbus", "Vd", "Vq"}
    p = PlotMESCOutput()
    datatypes = ('ehz', 'phaseA', 'iqreq', 'vbus')
    (plt, fig) = p.generatePlot(dname, datatypes, dname.replace(".txt", ""))
    
    plt.savefig(oname + ".png")
    plt.show()
    plt.close(fig)

    # on command line, run:
    # ./plot.py -d output.log_03-21-24_20:57.txt -o output_file
    # where output.log_03-21-24_20:57.txt contains results from collecting MESC data

if __name__ == "__main__":
    main()
        

            
