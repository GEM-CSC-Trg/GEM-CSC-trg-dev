import ROOT as rt
import array
import os

import mplhep as hep
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import matplotlib.patches as patches
hep.style.use("CMS")

# https://matplotlib.org/stable/users/explain/colors/colormaps.html
# https://mplhep.readthedocs.io/en/latest/api.html
# https://www.desy.de/~tadej/tutorial/matplotlib_tutorial.html

#plt.rcParams.update({
#    "text.usetex": True,
    #"font.family": "sans-serif",
    #"font.sans-serif": "Helvetica",
#})

def make_plotdir(plotdir):
    if not os.path.exists(plotdir):
        os.makedirs(plotdir)

def add_progress_bar(rdf):
    rt.RDF.Experimental.AddProgressBar(rdf)

def set_batch():
    rt.gROOT.SetBatch(1)

def enable_mt(threads=8):
    rt.EnableImplicitMT(threads)

def TH1F2np(histo, overflow=False):
    content = histo.GetArray()
    content.SetSize(histo.GetNbinsX() + 2)
    if overflow:
        content = np.array(content)[1:]
    else:
        content = np.array(content)[1:-1]
    binning = np.array(histo.GetXaxis().GetXbins())
    if overflow:
        binning = np.append(binning, [np.inf])
    return content, binning

period_dict = {
    "Run2_2018" : "59.83",
    "Run2_2017" : "41.48",
    "Run2_2016" : "16.8",
    "Run2_2016_HIPM" : "19.5",
    "Run2_all": "138.8",

}

def plot_1D_histogram(histograms, x_label, y_label, hist_labels, title, filename, skipNumber=None,scale=False, h_to_scale=None, bin_labels=None):
    fig, ax = plt.subplots(figsize=(10,10))
    #plt.xlabel(xlabel, fontsize=40)
    alpha=0.8
    linewidth=2
    colors = ["cornflowerblue", "salmon"]
    colorlist = ["black", "salmon", "cyan", "magenta", "cornflowerblue", "red",  "green"]
    for histogram,label,color in zip(histograms,hist_labels,colorlist):
        print(label, color)
        bins_x = []
        bins_y = []
        histograms.index(histogram)
        if skipNumber:
            if histograms.index(histogram) == skipNumber:
                print(f"skipping {histograms.index(histogram)}")
                continue
        for binnum in range(1,histogram.GetNbinsX()+1):
            bins_x.append(histogram.GetXaxis().GetBinCenter(binnum))
            bins_y.append(histogram.GetBinContent(binnum))
        bin_edges = [histogram.GetBinLowEdge(i+1) for i in range(histogram.GetNbinsX())]
        bin_edges.append(histogram.GetBinLowEdge(histogram.GetNbinsX()+1))
        # bin_edges = np.linspace(0, 50, 50 + 1)

        hep.histplot(
            np.array(bins_y),
            bins=np.array(bin_edges), #np.array(bins),
            histtype="step",
            color=color,
            # alpha=alpha,
            edgecolor=color,
            label=label,
            linewidth=linewidth,
            ax=ax,
        )
        #print(good_binnum)
    x_low = histogram.GetBinLowEdge(0)
    x_high = histogram.GetBinLowEdge(histogram.GetNbinsX())
    ax.set_xlim(xlow, xhigh)
    hep.cms.label("Preliminary", fontsize=25) #umi=period_dict[period], year=period.split('_')[1]
    ax.legend(fontsize=25)
    plt.show()
    plt.gca().set_aspect('auto')
    plt.savefig(f"{filename}.pdf", format="pdf", bbox_inches="tight")
    plt.savefig(f"{filename}.png", format="png", bbox_inches="tight")
    print(f"{filename}.png")

