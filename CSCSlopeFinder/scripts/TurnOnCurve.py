import ROOT
from plots import *

if __name__ == "__main__":
    import argparse
    import yaml
    parser = argparse.ArgumentParser()
    parser.add_argument('--inFiles', required=True, type=str)
    parser.add_argument('--treeName', required=False, type=str, default='OnlyRecos')
    parser.add_argument('--layer', required=False, type=int, default=1)
    parser.add_argument('--emtfcutlist', required=False, type=str, default='')
    args = parser.parse_args()


    # years_list = args.year.split(",")
    # if args.year == 'all':
        # years_list = ["2016_HIPM","2016","2017","2018"]
    # all this should go in a separate config file
    evenodd_list = ["odd", "even"]
    region_list = [-1, 1] #[-1, 0, 1]
    charge_list = [-1, 1] #[-1, 0, 1]
    emtftrack_modes = [11, 13, 14, 15] #[-1, 0, 1]
    dRmatch = 0.4
    doRegionLevel = True # separately x region
    doChargeLevel = True # separately x charge

    xbins = 50
    xlow = 0
    xhigh = 50


    do_min_test = False

    base_cut = f"has_emtf_track_match & (has_LCT_match{args.layer})"

    reco_cut = f"has_reco_l1_match & (abs(reco_l1_match_dR) < {dRmatch})"
    mode_cut = "(" + " || ".join([f"(emtftrack_mode == {mode})" for mode in emtftrack_modes]) + ")" # requiring station 1
    zmu_cut = f"(z_cand) & (abs(z_mass-91) < 10) & (z_opposite_charge)"
    hists={}
    rdf = ROOT.RDataFrame(f"GEMCSCBendingAngleTester/{args.treeName}",args.inFiles)
    ROOT.RDF.Experimental.AddProgressBar(rdf)
    for evenodd in evenodd_list:
        cut = f"{base_cut} & {reco_cut} & {mode_cut} & {zmu_cut}" #Add residual cut later for even/odd difference
        res_cut = 100
        emtfcutlist = [14]
        bacutlist = [1000, 28, 24, 16, 12, 10, 8]
        ba_min = -5
        if evenodd == "even":
            cut = cut + " & (LCT_CSC_chamber%2 == 0)"
            emtfcutlist = [5, 6, 7, 8] # emtf pt threshold in GeV
            bacutlist = [1000, 12, 10, 8, 6, 5, 4]
            res_cut  = 20 # from firmware /
            ba_min = -8
        if evenodd == "odd":
            cut = cut + " & (LCT_CSC_chamber%2 == 1)"
            emtfcutlist = [5, 6, 7, 8]
            bacutlist = [1000, 28, 24, 16, 12, 10, 8]
            res_cut = 40
            ba_min = -16
        residual_cut = f"((abs(LCT_match_GE{args.layer}_residual) < {res_cut}) & (has_LCT_match{args.layer}))"
        cut = cut + " & " + residual_cut
        if args.emtfcutlist != '':
            emtfcutlist = [int(cut) for cut in args.emtfcutlist.split(",")]

        #print(f"emtfcutlist={emtfcutlist}")

        #cut = cut + " & emtftrack_charge == -1" #reco_l1_match_charge
        #cut = cut + " & LCT_GE1_region == 1"
        cut = cut + " & (LCT_CSC_ME1b == 1)" # to check for second layer

        rdf_tmp = rdf.Filter(cut)
        # print(type(rdf_tmp))

        for reg in region_list:
            if not doRegionLevel:
                if reg in [-1, 1]: continue
            cut_reg = cut + f" & ((LCT_GE{args.layer}_region == -1) | (LCT_GE{args.layer}_region == 1))" if reg == 0 else cut + f" & (LCT_GE{args.layer}_region == {reg})"
            for charge in charge_list:
                if not doChargeLevel:
                    if charge in [-1, 1]: continue
                cut_charge = cut_reg + f" & ((reco_l1_match_charge == -1) | (reco_l1_match_charge == 1))" if charge == 0 else cut_reg + f" & (reco_l1_match_charge == {charge})"

                for emtf_cut in emtfcutlist:
                    hist_list_to = [None for _ in range(len(bacutlist))]
                    hist_list_eff = [None for _ in range(len(bacutlist))]
                    rdf_num_list = [None for _ in range(len(bacutlist))]
                    rdf_den_list = [None for _ in range(len(bacutlist))]
                    labels = [None for _ in range(len(bacutlist))]
                    # legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
                    # legend.SetNColumns(2)
                    # title=(f"EMTFCut >= {emtf_cut} -- {evenodd} -- R{reg} Charge{charge}", "C")

                    y_title = f"Marginal Efficiency / 1GeV"
                    x_title = "p_{T}(Reco)"

                    # labels =
                    hnums = [None for _ in range(len(bacutlist))]
                    hdens = [None for _ in range(len(bacutlist))]

                    for i, ba_cut in enumerate(bacutlist):
                        #print(f"i={i}, ba_cut={ba_cut}")
                        title = f"TurnOnCurve_EO{evenodd}_R{reg}_Charge{charge}_{i}"
                        #print(f"title = {title}")
                        cuts_num = []
                        cuts_den = []
                        counts_num = []
                        counts_den = []
                        ba_min_tmp = -1000 if (i == 0) else ba_min
                        cutnum = cut_charge + f" & abs(LCT_BendingAngle_GE1) <= {ba_cut} & emtftrack_pt >= {emtf_cut}"
                        #print(f"initially, cut num is = {cutnum}")
                        if do_min_test:
                            cutnum = cut_charge + f" & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) < {ba_cut}) & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) >= {ba_min_tmp}) & emtftrack_pt >= {emtf_cut}"
                        else:
                            cutnum = cut_charge + f" & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) < {ba_cut}) & emtftrack_pt >= {emtf_cut}"
                        #print(f"then, cut num is = {cutnum}")
                        cutden = cut_charge
                        #print(f"cut den is = {cutden}")
                        # print(hnums)
                        hnums[i] = rdf_tmp.Filter(cutnum).Histo1D(ROOT.RDF.TH1DModel(f"Nums{i}", f"Nums{i}", xbins, xlow, xhigh), "reco_l1_match_pt").GetValue()
                        hdens[i] = rdf_tmp.Filter(cutden).Histo1D(ROOT.RDF.TH1DModel(f"Dens{i}", f"Dens{i}", xbins, xlow, xhigh), "reco_l1_match_pt").GetValue()

                        # hnums[i]=rdf_tmp.Filter(cutnum).Histo1D((f"Nums{i}", f"Nums{i}", xbins, xlow, xhigh), "reco_l1_match_pt")
                        # hdens[i]=rdf_tmp.Filter(cutden).Histo1D((f"Dens{i}", f"Dens{i}", xbins, xlow, xhigh), "reco_l1_match_pt")
                        hist_list_to[i] = hnums[i]
                        hist_list_to[i].Divide(hdens[i])
                        # hist_list_eff[i] = hist_list_to[i]
                        # hist_list_eff[i].Divide(hist_list_to[0])
                        if i == 0:
                            labels[i]=f"Any GEM Match"
                        else:
                            labels[i]=f"BendingAngleCut <= {ba_cut}"
                        # print(hist_list_eff)
                        # print(hist_list_to)
                    # print(hist_list_eff)
                    # print(hist_list_to)
                    if do_min_test:
                        outfile_to= f"Plots/TurnOnCurve_emtfpt{emtf_cut}_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}"
                    else:
                        outfile_to= f"Plots/TurnOnCurve_emtfpt{emtf_cut}_{evenodd}_R{reg}_Charge{charge}"

                    plot_1D_histogram(hist_list_to, x_title, y_title, labels,title, outfile_to)

                    # if do_min_test:
                    #     outfile_eff = f"Efficiency_emtfpt{emtf_cut}_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}"
                    # else:
                    #     outfile_eff = f"Efficiency_emtfpt{emtf_cut}_{evenodd}_R{reg}_Charge{charge}"
                    plot_1D_histogram(hist_list_to, x_title, y_title, labels,title, outfile_to+"_rel", 0, True, hist_list_to[0])


                    # plot_1D_histogram(hist_list_eff, x_title, y_title, labels,title, outfile_eff)
                    # plot_1D_histogram(hist_list_eff, x_title, y_title, labels,title, outfile_eff+"_rel", 0, True, hist_list_to[0])

                    # plot_1D_histogram(hist_list_eff, x_title, y_title, labels,title, outfile_eff, "",skipNumber=0)

