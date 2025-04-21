import ROOT, sys, tdrstyle, os, array, datetime
import numpy as np
from scipy.stats import beta

#f = ROOT.TFile("{}".format(sys.argv[1]))
fname = "/eos/user/d/daebi/GEMCSCTrigger/2024G_ZMu/2024G_ZMu_27Oct2024.root"
fname2 = "/eos/user/d/daebi/GEMCSCTrigger/2024EFGHIJ_ZeroBias/2024EFGHIZeroBias_27Oct2024.root"

#Old Files
#fname = "/eos/user/d/daebi/GEMCSCTrigger/2024E_ZMu/Muon0/2024E_ZMu_18Sep2024/240917_185009/2024E_ZMu_18Sep2024.root"
#fname2 = "/eos/user/d/daebi/GEMCSCTrigger/2024E_ZeroBias/ZeroBias_EFG_18Sep2024.root"


f = ROOT.TFile(fname)
f2 = ROOT.TFile(fname2)

event = f.Get("GEMCSCBendingAngleTester/OnlyRecos")
#event = f.Get("GEMCSCBendingAngleTester/AllLCTs")

event2 = f2.Get("GEMCSCBendingAngleTester/HighestEMTFpT")




ROOT.gROOT.SetBatch(1)
tdrstyle.setTDRStyle()

ROOT.EnableImplicitMT(16)

rdf_sig = ROOT.RDataFrame(event)
ROOT.RDF.Experimental.AddProgressBar(rdf_sig)
rdf_bkg = ROOT.RDataFrame(event2)
ROOT.RDF.Experimental.AddProgressBar(rdf_bkg)

if not os.path.exists("plots/"):
  os.makedirs("plots/")


plotdir = "plots/ROCCurve_12Nov_ModesNotInDenom/"
plotdir = "plots/ROCCurve_2Dec/"
if not os.path.exists(plotdir):
  os.makedirs(plotdir)


H_ref = 600
W_ref = 800
W = W_ref
H = H_ref

T = 0.12*H
B = 0.16*H
L = 0.16*W
R = 0.12*W

legend_xmin = 0.5
legend_xmax = 0.85
legend_ymin = 0.2
legend_ymax = 0.5

yscale = 1.3

layer_list = [1, 2]

evenodd_list = ["odd", "even"]

reg_list = [0] #[-1, 0, 1]
charge_list = [0] #[-1, 0, 1]
dRmatch = 0.4

base_cut = "has_emtf_track_match & has_LCT_match{layer}"
residual_cut = "(abs(LCT_match_GE{layer}_residual) < {res_cut})"
reco_cut = f"has_reco_l1_match & (abs(reco_l1_match_dR) < {dRmatch})"
mode_cut = f"((emtftrack_mode == 11) | (emtftrack_mode == 13) | (emtftrack_mode == 14) | (emtftrack_mode == 15))"
twostation_mode_cut = f"((emtftrack_mode == 9) | (emtftrack_mode == 10) | (emtftrack_mode == 11) | (emtftrack_mode == 12) | (emtftrack_mode == 13) | (emtftrack_mode == 14) | (emtftrack_mode == 15))"
zmu_cut = f"(z_cand) & (abs(z_mass-91) < 10) & (z_opposite_charge)"
#zmu_cut = f"(z_cand) & (abs(z_mass-91) < 10)" #Old file doesn't have opp charge branch

eta_val = 0
eta_cut = f"(LCT_match_GE1_roll >= 0)" if eta_val == 0 else f"(LCT_match_GE1_roll == {eta_val})"


#mode_cut = f"(1)"
#zmu_cut = f"(1)"

xbins = 50
xlow = 0
xhigh = 50

doRegionLevel = True
doChargeLevel = True
doBaMin = False

for layer in layer_list:
    for evenodd in evenodd_list:
        legend_xmin = 0.4
        legend_xmax = 0.85
        legend_ymin = 0.2
        legend_ymax = 0.5
        cut = ""
        if ((layer == 1) or (layer == 2)):
            cut = f"{base_cut.format(layer = layer)} & {mode_cut.format(layer = layer)}" #Add residual cut later for even/odd difference
            #cut = f"{base_cut.format(layer = layer)} & {twostation_mode_cut.format(layer = layer)}" #Test removing mode cut and see if adding to CSC alone denominator changes things dramatically
        else:
            print("Bad layer! Break!!!")
            continue

        res_cut = 100
        emtfcutlist = [14]
        bacutlist = [1000, 28, 24, 16, 12, 10, 8]
        ba_min = -5
        if evenodd == "even":
            cut = cut + " & (LCT_CSC_chamber%2 == 0)"
            emtfcutlist = [22]
            bacutlist = [1000, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2]
            res_cut  = 20
            ba_min = -6

        if evenodd == "odd":
            cut = cut + " & (LCT_CSC_chamber%2 == 1)"
            emtfcutlist = [22]
            bacutlist = [1000, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6]
            res_cut = 40
            ba_min = -6

        #Create some 'key' coordinate for following a specific BA cut (10)
        key_vals = [10, 20, 30]

        cut = cut + " & " + residual_cut.format(layer = layer, res_cut = res_cut)


        #cut = cut + " & emtftrack_charge == 1"
        #cut = cut + " & LCT_GE1_region == -1"
        cut = cut + " & LCT_CSC_ME1b == 1"
        cut = cut + f" & {eta_cut}"

        index = 0
        csc_alone_bkg_nums_evenodd = []
        csc_alone_bkg_dens_evenodd = []

        csc_alone_sig_nums_evenodd = []
        csc_alone_sig_dens_evenodd = []

        csc_ge1_slope_bkg_nums_evenodd = []
        csc_ge1_slope_bkg_dens_evenodd = []

        csc_ge1_slope_sig_nums_evenodd = []
        csc_ge1_slope_sig_dens_evenodd = []

        csc_ge1_bkg_nums_evenodd = []
        csc_ge1_bkg_dens_evenodd = []

        csc_ge1_sig_nums_evenodd = []
        csc_ge1_sig_dens_evenodd = []

        for reg in reg_list:
            if not doRegionLevel:
                if reg in [-1, 1]: continue
            cut_reg = cut + f" & ((LCT_GE{layer}_region == -1) | (LCT_GE{layer}_region == 1))" if reg == 0 else cut + f" & (LCT_GE{layer}_region == {reg})"
            for charge in charge_list:
                if not doChargeLevel:
                    if charge in [-1, 1]: continue    


                cut_charge = cut_reg + f" & ((emtftrack_charge == -1) | (emtftrack_charge == 1))" if charge == 0 else cut_reg + f" & (emtftrack_charge == {charge})" #Don't use l1 charge for rate!!!


                #for recopt_cut, emtfpt_cut in [[27,22], [20,14]]:
                for recopt_cut, emtfpt_cut in [[10,7]]:
                    cut_sig = cut_charge + f" & {reco_cut} & reco_l1_match_pt >= {recopt_cut} & (reco_l1_match_pt <= ({recopt_cut} + 1)) & emtftrack_pt >= {emtfpt_cut} & {zmu_cut}"
                    cut_bkg = cut_charge + f" & emtftrack_pt >= {emtfpt_cut}"

                    rdf_sig_tmp = rdf_sig.Filter(cut_sig)
                    rdf_bkg_tmp = rdf_bkg.Filter(cut_bkg)


                    # csc_alone_bkg_nums = []
                    # csc_alone_bkg_dens = []

                    # csc_alone_sig_nums = []
                    # csc_alone_sig_dens = []

                    # csc_ge1_slope_bkg_nums = []
                    # csc_ge1_slope_bkg_dens = []

                    # csc_ge1_slope_sig_nums = []
                    # csc_ge1_slope_sig_dens = []

                    csc_alone_bkg_nums_evenodd.append([])
                    csc_alone_bkg_dens_evenodd.append([])

                    csc_alone_sig_nums_evenodd.append([])
                    csc_alone_sig_dens_evenodd.append([])

                    csc_ge1_slope_bkg_nums_evenodd.append([])
                    csc_ge1_slope_bkg_dens_evenodd.append([])

                    csc_ge1_slope_sig_nums_evenodd.append([])
                    csc_ge1_slope_sig_dens_evenodd.append([])

                    #for slope_cut in range(15):
                    for slope_cut in [15]:
                        bkg_num_cut = cut_bkg + f" & abs(LCT_slope) <= {slope_cut} & {mode_cut.format(layer = layer)}"
                        #bkg_num_cut = cut_bkg + f" & abs(LCT_slope) <= {slope_cut}"
                        bkg_den_cut = cut_bkg

                        sig_num_cut = cut_sig + f" & abs(LCT_slope) <= {slope_cut} & {mode_cut.format(layer = layer)}"
                        #sig_num_cut = cut_sig + f" & abs(LCT_slope) <= {slope_cut}"
                        sig_den_cut = cut_sig

                        csc_alone_bkg_nums_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
                        csc_alone_bkg_dens_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

                        csc_alone_sig_nums_evenodd[index].append(rdf_sig_tmp.Filter(sig_num_cut).Count())
                        csc_alone_sig_dens_evenodd[index].append(rdf_sig_tmp.Filter(sig_den_cut).Count())



                        bkg_num_cut = cut_bkg + f" & abs(LCT_slope_with_GE{layer}) <= {slope_cut}"
                        bkg_den_cut = cut_bkg

                        sig_num_cut = cut_sig + f" & abs(LCT_slope_with_GE{layer}) <= {slope_cut}"
                        sig_den_cut = cut_sig

                        csc_ge1_slope_bkg_nums_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
                        csc_ge1_slope_bkg_dens_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

                        csc_ge1_slope_sig_nums_evenodd[index].append(rdf_sig_tmp.Filter(sig_num_cut).Count())
                        csc_ge1_slope_sig_dens_evenodd[index].append(rdf_sig_tmp.Filter(sig_den_cut).Count())


                    csc_ge1_bkg_nums_evenodd.append([])
                    csc_ge1_bkg_dens_evenodd.append([])

                    csc_ge1_sig_nums_evenodd.append([])
                    csc_ge1_sig_dens_evenodd.append([])
                    for ba_cut in range(50):
                        bkg_num_cut = cut_bkg
                        if doBaMin:
                            bkg_num_cut = cut_bkg + f" & ((emtftrack_charge*LCT_GE{layer}_region*LCT_BendingAngle_GE{layer}*(-1)) < {ba_cut}) & ((emtftrack_charge*LCT_GE{layer}_region*LCT_BendingAngle_GE{layer}*(-1)) > {ba_min})" #Not symmetric cut
                        else:
                            bkg_num_cut = cut_bkg + f" & abs(LCT_BendingAngle_GE{layer}) <= {ba_cut}"

                        bkg_den_cut = cut_bkg

                        if doBaMin:
                            sig_num_cut = cut_sig + f" & ((emtftrack_charge*LCT_GE{layer}_region*LCT_BendingAngle_GE{layer}*(-1)) < {ba_cut}) & ((emtftrack_charge*LCT_GE{layer}_region*LCT_BendingAngle_GE{layer}*(-1)) > {ba_min})" #Not symmetric cut
                        else:
                            sig_num_cut = cut_sig + f" & abs(LCT_BendingAngle_GE{layer}) <= {ba_cut}"
                        
                        sig_den_cut = cut_sig

                        csc_ge1_bkg_nums_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
                        csc_ge1_bkg_dens_evenodd[index].append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

                        csc_ge1_sig_nums_evenodd[index].append(rdf_sig_tmp.Filter(sig_num_cut).Count())
                        csc_ge1_sig_dens_evenodd[index].append(rdf_sig_tmp.Filter(sig_den_cut).Count())
                    
                    index += 1





        #Finished setting up the filters, now the getvalue calls
        index = 0
        for reg in reg_list:
            if not doRegionLevel:
                if reg in [-1, 1]: continue
            cut_reg = cut + f" & ((LCT_GE{layer}_region == -1) | (LCT_GE{layer}_region == 1))" if reg == 0 else cut + f" & (LCT_GE{layer}_region == {reg})"
            for charge in charge_list:
                if not doChargeLevel:
                    if charge in [-1, 1]: continue        
                cut_charge = cut_reg + f" & ((emtftrack_charge == -1) | (emtftrack_charge == 1))" if charge == 0 else cut_reg + f" & (emtftrack_charge == {charge})" #Don't use l1 charge for rate!!!


                #for recopt_cut, emtfpt_cut in [[27,22], [20,14], [10,5]]:
                for recopt_cut, emtfpt_cut in [[10,7]]:





                    xpoints_csc_alone_nums = [ csc_alone_bkg_num.GetValue() for csc_alone_bkg_num in csc_alone_bkg_nums_evenodd[index] ]
                    xpoints_csc_alone_dens = [ csc_alone_bkg_den.GetValue() for csc_alone_bkg_den in csc_alone_bkg_dens_evenodd[index] ]

                    ypoints_csc_alone_nums = [ csc_alone_sig_num.GetValue() for csc_alone_sig_num in csc_alone_sig_nums_evenodd[index] ]
                    ypoints_csc_alone_dens = [ csc_alone_sig_den.GetValue() for csc_alone_sig_den in csc_alone_sig_dens_evenodd[index] ]


                    xpoints_csc_ge1_slope_nums = [ csc_ge1_slope_bkg_num.GetValue() for csc_ge1_slope_bkg_num in csc_ge1_slope_bkg_nums_evenodd[index] ]
                    xpoints_csc_ge1_slope_dens = [ csc_ge1_slope_bkg_den.GetValue() for csc_ge1_slope_bkg_den in csc_ge1_slope_bkg_dens_evenodd[index] ]

                    ypoints_csc_ge1_slope_nums = [ csc_ge1_slope_sig_num.GetValue() for csc_ge1_slope_sig_num in csc_ge1_slope_sig_nums_evenodd[index] ]
                    ypoints_csc_ge1_slope_dens = [ csc_ge1_slope_sig_den.GetValue() for csc_ge1_slope_sig_den in csc_ge1_slope_sig_dens_evenodd[index] ]


                    xpoints_csc_ge1_nums = [ csc_ge1_bkg_num.GetValue() for csc_ge1_bkg_num in csc_ge1_bkg_nums_evenodd[index] ]
                    xpoints_csc_ge1_dens = [ csc_ge1_bkg_den.GetValue() for csc_ge1_bkg_den in csc_ge1_bkg_dens_evenodd[index] ]

                    ypoints_csc_ge1_nums = [ csc_ge1_sig_num.GetValue() for csc_ge1_sig_num in csc_ge1_sig_nums_evenodd[index] ]
                    ypoints_csc_ge1_dens = [ csc_ge1_sig_den.GetValue() for csc_ge1_sig_den in csc_ge1_sig_dens_evenodd[index] ]

                    #Done with index loading, can increment now
                    index += 1

                    xpoints_csc_alone = []
                    ypoints_csc_alone = []

                    xpoints_csc_ge1_slope = []
                    ypoints_csc_ge1_slope = []

                    for i in range(len(xpoints_csc_alone_nums)):
                        xpoints_csc_alone.append(xpoints_csc_alone_nums[i]/xpoints_csc_alone_dens[i])
                        ypoints_csc_alone.append(ypoints_csc_alone_nums[i]/ypoints_csc_alone_dens[i])

                        xpoints_csc_ge1_slope.append(xpoints_csc_ge1_slope_nums[i]/xpoints_csc_ge1_slope_dens[i])
                        ypoints_csc_ge1_slope.append(ypoints_csc_ge1_slope_nums[i]/ypoints_csc_ge1_slope_dens[i])

                    np_xpoints_csc_alone = np.array(xpoints_csc_alone)
                    np_ypoints_csc_alone = np.array(ypoints_csc_alone)

                    np_xpoints_csc_ge1_slope = np.array(xpoints_csc_ge1_slope)
                    np_ypoints_csc_ge1_slope = np.array(ypoints_csc_ge1_slope)

                    xpoints_csc_ge1 = []
                    ypoints_csc_ge1 = []

                    special_x = []
                    special_y = []

                    special_error_x = []
                    special_error_y = []

                    special_err_x_under = []
                    special_err_x_over = []

                    special_err_y_under = []
                    special_err_y_over = []

                    key_x = []
                    key_y = []

                    for i in range(len(xpoints_csc_ge1_nums)):
                        xpoints_csc_ge1.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
                        ypoints_csc_ge1.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])

                        if i in bacutlist:
                            special_x.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
                            special_y.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])

                            if i in key_vals:
                                key_x.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
                                key_y.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])

                            # err_xpoints_num = np.sqrt(xpoints_csc_ge1_nums[i])/xpoints_csc_ge1_nums[i]
                            # err_xpoints_den = np.sqrt(xpoints_csc_ge1_dens[i])/xpoints_csc_ge1_dens[i]

                            # err_ypoints_num = np.sqrt(ypoints_csc_ge1_nums[i])/ypoints_csc_ge1_nums[i]
                            # err_ypoints_den = np.sqrt(ypoints_csc_ge1_dens[i])/ypoints_csc_ge1_dens[i]

                            # special_error_x.append((xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])*np.sqrt(err_xpoints_num**2 + err_xpoints_den**2))
                            # special_error_y.append((ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])*np.sqrt(err_ypoints_num**2 + err_ypoints_den**2))

                            p_x = xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i]
                            n_x = xpoints_csc_ge1_dens[i]
                            err_x = np.sqrt(p_x*(1-p_x)/n_x)
                            special_error_x.append(err_x)

                            p_y = ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i]
                            n_y = ypoints_csc_ge1_dens[i]
                            err_y = np.sqrt(p_y*(1-p_y)/n_y)
                            special_error_y.append(err_y)


                            #Beta returns value of up/down, but we want the error, so we must subtract from original
                            k = xpoints_csc_ge1_nums[i]
                            n = xpoints_csc_ge1_dens[i]
                            alpha = 0.32
                            p_u, p_o = beta.ppf([alpha/2, 1 - alpha/2], [k, k + 1], [n - k + 1, n - k])
                            special_err_x_under.append(p_u-xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
                            special_err_x_over.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i]-p_o)

                            k = ypoints_csc_ge1_nums[i]
                            n = ypoints_csc_ge1_dens[i]
                            alpha = 0.32
                            p_u, p_o = beta.ppf([alpha/2, 1 - alpha/2], [k, k + 1], [n - k + 1, n - k])
                            special_err_y_under.append(p_u-ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])
                            special_err_y_over.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i]-p_o)
                            


                    xpoints_csc_ge1.append(1.0) #Hard put a final 1,1 to make the TGraph Integral work
                    ypoints_csc_ge1.append(1.0) 


                    np_xpoints_csc_ge1 = np.array(xpoints_csc_ge1)
                    np_ypoints_csc_ge1 = np.array(ypoints_csc_ge1)

                    np_special_x = np.array(special_x)
                    np_special_y = np.array(special_y)

                    np_special_error_x = np.array(special_error_x)
                    np_special_error_y = np.array(special_error_y)


                    np_special_err_x_under = np.array(special_err_x_under)
                    np_special_err_x_over = np.array(special_err_x_over)

                    np_special_err_y_under = np.array(special_err_y_under)
                    np_special_err_y_over = np.array(special_err_y_over)



                    np_key_x = np.array(key_x)
                    np_key_y = np.array(key_y)


                    canvasname = f"emtf{emtfpt_cut}_reco{recopt_cut}_{evenodd}"
                    canvas = ROOT.TCanvas(canvasname, canvasname, W, H)
                    canvas.SetFillColor(0)
                    canvas.SetBorderMode(0)
                    canvas.SetFrameFillStyle(0)
                    canvas.SetFrameBorderMode(0)
                    canvas.SetLeftMargin( L/W )
                    canvas.SetRightMargin( R/W )
                    canvas.SetTopMargin( T/H )
                    canvas.SetBottomMargin( B/H )
                    canvas.SetTickx(0)
                    canvas.SetTicky(0)

                    ROOT.gPad.SetGridx()
                    ROOT.gPad.SetGridy()

                    legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
                    legend.SetHeader(f"EMTFCut >= {emtfpt_cut} -- {evenodd} -- R{reg} Charge{charge} -- Eta{eta_val}", "C")


                    g1 = ROOT.TGraph(len(xpoints_csc_alone), np_xpoints_csc_alone, np_ypoints_csc_alone)
                    g1.SetLineColor(ROOT.kRed)
                    #g1.Draw()

                    g2 = ROOT.TGraph(len(xpoints_csc_ge1), np_xpoints_csc_ge1, np_ypoints_csc_ge1)
                    g2.SetLineColor(ROOT.kBlue)
                    g2.SetMarkerStyle(ROOT.kCircle)
                    #g2.Draw("same")

                    g3 = ROOT.TGraph(len(xpoints_csc_ge1_slope), np_xpoints_csc_ge1_slope, np_ypoints_csc_ge1_slope)
                    g3.SetLineColor(ROOT.kMagenta)
                    g3.SetMarkerStyle(ROOT.kStar)
                    #g3.Draw("same")

                    #colorlist = [ROOT.kBlack, ROOT.kOrange-3, ROOT.kCyan+1, ROOT.kMagenta, ROOT.kBlue, ROOT.kRed, ROOT.kGreen+3]
                    #g4 = ROOT.TGraph(len(special_x), np_special_x, np_special_y)#, color=[ROOT.kGreen+3, ROOT.kRed, ROOT.kBlue, ROOT.kMagenta, ROOT.kCyan+1, ROOT.kOrange-3])

                    #This was using symmetric errors, but we want to use non-symmetric errors
                    #g4 = ROOT.TGraphErrors(len(special_x), np_special_x, np_special_y, np_special_error_x, np_special_error_y)#, color=[ROOT.kGreen+3, ROOT.kRed, ROOT.kBlue, ROOT.kMagenta, ROOT.kCyan+1, ROOT.kOrange-3])

                    #TGraphAsymmErrors allows an xlow_err and xhigh_err
                    g4 = ROOT.TGraphAsymmErrors(len(special_x), np_special_x, np_special_y, np_special_err_x_under, np_special_err_x_over, np_special_err_y_under, np_special_err_y_over)#, color=[ROOT.kGreen+3, ROOT.kRed, ROOT.kBlue, ROOT.kMagenta, ROOT.kCyan+1, ROOT.kOrange-3])
                    g4.SetMarkerStyle(ROOT.kCircle)
                    #g4.Draw("same text")

                    g5 = ROOT.TGraph(len(np_key_x), np_key_x, np_key_y)
                    g5.SetMarkerColor(ROOT.kRed)
                    g5.SetMarkerStyle(ROOT.kCircle)

                    mgname = f"MG_EMTF{emtfpt_cut}_RECO{recopt_cut}_{evenodd}"
                    mg = ROOT.TMultiGraph(mgname, mgname)

                    #mg.GetXaxis().SetLimits(0.0, 1.0)
                    #mg.GetYaxis().SetLimits(0.5, 1.0)

                    xAxis = mg.GetXaxis()
                    xAxis.SetTitleSize(0.04)
                    xAxis.SetTitleOffset(2)
                    xAxis.SetTitle(f"Trigger Rate Remaining (EMTFPt > {emtfpt_cut})")
                    #Trigger rate reduction factor

                    yAxis = mg.GetYaxis()
                    yAxis.SetTitleSize(0.04)
                    yAxis.SetTitleOffset(2)
                    yAxis.SetTitle(f"Marginal Efficiency / 1GeV (RecoPt = {recopt_cut})")

                    mg.Add(g1, "lp")
                    mg.Add(g2, "l")
                    mg.Add(g3, "p")
                    mg.Add(g4, "p")
                    mg.Add(g5, "p")
                    mg.Draw("AP")

                    mg.SetMinimum(0.2)



                    legend.AddEntry(g1, f"CSC Alone (Current Binning) <= Slope")
                    #legend.AddEntry(g1, f"3 Station Mode Cut")
                    #legend.AddEntry(g3, f"CSC+GE1 SlopeBins, <= Slope")
                    legend.AddEntry(g2, f"CSC+GE1 <= BendingAngle")

                    legend.SetTextSize(0.)
                    legend.SetBorderSize(0)
                    legend.Draw()

                    canvas.Modified()

                    savename = "example.pdf"
                    if doBaMin:
                        savename = os.path.join(plotdir, f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_L{layer}.pdf")
                    else:
                        savename = os.path.join(plotdir, f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}_{evenodd}_R{reg}_Charge{charge}_L{layer}.pdf")

                    canvas.SaveAs(savename)

                    print("Saved")
