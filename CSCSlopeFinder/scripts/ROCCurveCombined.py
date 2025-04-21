import ROOT, sys, tdrstyle, os, array, datetime
import numpy as np

#f = ROOT.TFile("{}".format(sys.argv[1]))
fname = "/eos/user/d/daebi/GEMCSCTrigger/2024E_ZMu/Muon0/2024E_ZMu_18Sep2024/240917_185009/2024E_ZMu_18Sep2024.root"
fname2 = "/eos/user/d/daebi/GEMCSCTrigger/2024E_ZeroBias/ZeroBias_EFG_18Sep2024.root"

f = ROOT.TFile(fname)
f2 = ROOT.TFile(fname2)

event = f.Get("GEMCSCBendingAngleTester/AllLCTs")

event2 = f2.Get("GEMCSCBendingAngleTester/HighestEMTFpT")




ROOT.gROOT.SetBatch(1)
tdrstyle.setTDRStyle()

rdf_sig = ROOT.RDataFrame(event)
rdf_bkg = ROOT.RDataFrame(event2)

if not os.path.exists("plots/"):
  os.makedirs("plots/")


plotdir = "plots/ROCCurveCombined/"
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


yscale = 1.3

dRmatch = 0.4

base_cut = "has_emtf_track_match & has_LCT_match1"
residual_cut = "(abs(LCT_match_GE1_residual) < {res_cut})"
reco_cut = f"has_reco_l1_match & (abs(reco_l1_match_dR) < {dRmatch})"
mode_cut = f"((emtftrack_mode == 11) | (emtftrack_mode == 13) | (emtftrack_mode == 14) | (emtftrack_mode == 15))"
zmu_cut = f"(z_cand) & (abs(z_mass-91) < 10)"

xbins = 50
xlow = 0
xhigh = 50


legend_xmin = 0.6
legend_xmax = 0.85
legend_ymin = 0.2
legend_ymax = 0.5
cut = f"{base_cut} & {reco_cut} & {mode_cut}" #Add residual cut later for even/odd difference
res_cut = 100
emtfcutlist = [14]
bacutlist = [1000, 28, 24, 16, 12, 10, 8]

cut = cut + f" & ( ((LCT_CSC_chamber%2 == 0) & ({residual_cut.format(res_cut = 20)})) | ((LCT_CSC_chamber%2 == 1) & ({residual_cut.format(res_cut = 40)})) )"

bacutlist = [ [1000, 1000], [16, 8], [15, 7], [12, 6], [11, 5], [9, 4] ]

#cut = cut + " & emtftrack_charge == -1"
#cut = cut + " & LCT_GE1_region == 1"
cut = cut + " & LCT_CSC_ME1b == 1"


for recopt_cut, emtfpt_cut in [[24,22]]:
    cut_sig = cut + f" & {reco_cut} & reco_l1_match_pt >= {recopt_cut} & (reco_l1_match_pt <= ({recopt_cut} + 1)) & emtftrack_pt >= {emtfpt_cut} & {zmu_cut}"
    cut_bkg = cut + f" & emtftrack_pt >= {emtfpt_cut}"

    rdf_sig_tmp = rdf_sig.Filter(cut_sig)
    rdf_bkg_tmp = rdf_bkg.Filter(cut_bkg)


    csc_alone_bkg_nums = []
    csc_alone_bkg_dens = []

    csc_alone_sig_nums = []
    csc_alone_sig_dens = []

    csc_ge1_slope_bkg_nums = []
    csc_ge1_slope_bkg_dens = []

    csc_ge1_slope_sig_nums = []
    csc_ge1_slope_sig_dens = []

    for slope_cut in range(15):
        bkg_num_cut = cut_bkg + f" & abs(LCT_slope) <= {slope_cut}"
        bkg_den_cut = cut_bkg

        sig_num_cut = cut_sig + f" & abs(LCT_slope) <= {slope_cut}"
        sig_den_cut = cut_sig

        csc_alone_bkg_nums.append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
        csc_alone_bkg_dens.append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

        csc_alone_sig_nums.append(rdf_sig_tmp.Filter(sig_num_cut).Count())
        csc_alone_sig_dens.append(rdf_sig_tmp.Filter(sig_den_cut).Count())



        bkg_num_cut = cut_bkg + f" & abs(LCT_slope_with_GE1) <= {slope_cut}"
        bkg_den_cut = cut_bkg

        sig_num_cut = cut_sig + f" & abs(LCT_slope_with_GE1) <= {slope_cut}"
        sig_den_cut = cut_sig

        csc_ge1_slope_bkg_nums.append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
        csc_ge1_slope_bkg_dens.append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

        csc_ge1_slope_sig_nums.append(rdf_sig_tmp.Filter(sig_num_cut).Count())
        csc_ge1_slope_sig_dens.append(rdf_sig_tmp.Filter(sig_den_cut).Count())


    csc_ge1_bkg_nums = []
    csc_ge1_bkg_dens = []

    csc_ge1_sig_nums = []
    csc_ge1_sig_dens = []
    for ba_cut in bacutlist:
        ba_odd = ba_cut[0]
        ba_even = ba_cut[1]
        cut_ba_odd = f"((LCT_CSC_chamber%2 == 1) & (abs((-(LCT_eighthstrip-LCT_match_GE1_padES_aligned))) <= {ba_odd}))"
        cut_ba_even = f"((LCT_CSC_chamber%2 == 0) & (abs((-(LCT_eighthstrip-LCT_match_GE1_padES_aligned))) <= {ba_even}))"
        ba_cut = f"({cut_ba_odd} | {cut_ba_even})"

        bkg_num_cut = cut_bkg + f" & {ba_cut}"
        bkg_den_cut = cut_bkg

        sig_num_cut = cut_sig + f" & {ba_cut}"
        sig_den_cut = cut_sig

        csc_ge1_bkg_nums.append(rdf_bkg_tmp.Filter(bkg_num_cut).Count())
        csc_ge1_bkg_dens.append(rdf_bkg_tmp.Filter(bkg_den_cut).Count())

        csc_ge1_sig_nums.append(rdf_sig_tmp.Filter(sig_num_cut).Count())
        csc_ge1_sig_dens.append(rdf_sig_tmp.Filter(sig_den_cut).Count())

    print("About to call the GetValues, lets see how long")
    ROOT.RDF.Experimental.AddProgressBar(rdf_bkg)
    ROOT.RDF.Experimental.AddProgressBar(rdf_sig)
    print(datetime.time())

    xpoints_csc_alone_nums = [ csc_alone_bkg_num.GetValue() for csc_alone_bkg_num in csc_alone_bkg_nums ]
    xpoints_csc_alone_dens = [ csc_alone_bkg_den.GetValue() for csc_alone_bkg_den in csc_alone_bkg_dens ]

    ypoints_csc_alone_nums = [ csc_alone_sig_num.GetValue() for csc_alone_sig_num in csc_alone_sig_nums ]
    ypoints_csc_alone_dens = [ csc_alone_sig_den.GetValue() for csc_alone_sig_den in csc_alone_sig_dens ]


    xpoints_csc_ge1_slope_nums = [ csc_ge1_slope_bkg_num.GetValue() for csc_ge1_slope_bkg_num in csc_ge1_slope_bkg_nums ]
    xpoints_csc_ge1_slope_dens = [ csc_ge1_slope_bkg_den.GetValue() for csc_ge1_slope_bkg_den in csc_ge1_slope_bkg_dens ]

    ypoints_csc_ge1_slope_nums = [ csc_ge1_slope_sig_num.GetValue() for csc_ge1_slope_sig_num in csc_ge1_slope_sig_nums ]
    ypoints_csc_ge1_slope_dens = [ csc_ge1_slope_sig_den.GetValue() for csc_ge1_slope_sig_den in csc_ge1_slope_sig_dens ]


    xpoints_csc_ge1_nums = [ csc_ge1_bkg_num.GetValue() for csc_ge1_bkg_num in csc_ge1_bkg_nums ]
    xpoints_csc_ge1_dens = [ csc_ge1_bkg_den.GetValue() for csc_ge1_bkg_den in csc_ge1_bkg_dens ]

    ypoints_csc_ge1_nums = [ csc_ge1_sig_num.GetValue() for csc_ge1_sig_num in csc_ge1_sig_nums ]
    ypoints_csc_ge1_dens = [ csc_ge1_sig_den.GetValue() for csc_ge1_sig_den in csc_ge1_sig_dens ]

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
    for i in range(len(xpoints_csc_ge1_nums)):
        xpoints_csc_ge1.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
        ypoints_csc_ge1.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])

        if i in bacutlist:
            special_x.append(xpoints_csc_ge1_nums[i]/xpoints_csc_ge1_dens[i])
            special_y.append(ypoints_csc_ge1_nums[i]/ypoints_csc_ge1_dens[i])

    np_xpoints_csc_ge1 = np.array(xpoints_csc_ge1)
    np_ypoints_csc_ge1 = np.array(ypoints_csc_ge1)

    np_special_x = np.array(special_x)
    np_special_y = np.array(special_y)

    print(datetime.time())

    canvasname = f"emtf{emtfpt_cut}_reco{recopt_cut}"
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

    print("Are the numbers real?")

    print(xpoints_csc_alone)
    print(np_xpoints_csc_alone)
    print(np_ypoints_csc_alone)
    

    g1 = ROOT.TGraph(len(xpoints_csc_alone), np_xpoints_csc_alone, np_ypoints_csc_alone)
    g1.SetLineColor(ROOT.kRed)
    #g1.Draw()

    print("g2")
    print(xpoints_csc_ge1)
    print(np_xpoints_csc_ge1)
    print(np_ypoints_csc_ge1)

    g2 = ROOT.TGraph(len(xpoints_csc_ge1), np_xpoints_csc_ge1, np_ypoints_csc_ge1)
    g2.SetLineColor(ROOT.kBlue)
    g2.SetMarkerStyle(ROOT.kCircle)
    #g2.Draw("same")

    print("g3")
    print(xpoints_csc_ge1_slope)
    print(np_xpoints_csc_ge1_slope)
    print(np_ypoints_csc_ge1_slope)

    g3 = ROOT.TGraph(len(xpoints_csc_ge1_slope), np_xpoints_csc_ge1_slope, np_ypoints_csc_ge1_slope)
    g3.SetLineColor(ROOT.kMagenta)
    g3.SetMarkerStyle(ROOT.kStar)
    #g3.Draw("same")

    print("g4")
    print(xpoints_csc_ge1)
    print(np_xpoints_csc_ge1)
    print(np_ypoints_csc_ge1)
    
    #colorlist = [ROOT.kBlack, ROOT.kOrange-3, ROOT.kCyan+1, ROOT.kMagenta, ROOT.kBlue, ROOT.kRed, ROOT.kGreen+3]
    #g4 = ROOT.TGraph(len(special_x), np_special_x, np_special_y)#, color=[ROOT.kGreen+3, ROOT.kRed, ROOT.kBlue, ROOT.kMagenta, ROOT.kCyan+1, ROOT.kOrange-3])
    #g4.SetMarkerStyle(ROOT.kCircle)
    #g4.Draw("same text")

    print("Finished processing, making the multigraphs")

    mgname = f"MG_EMTF{emtfpt_cut}_RECO{recopt_cut}"
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
    #mg.Add(g4, "p")
    mg.Draw("AP")

    mg.SetMinimum(0.5)



    legend.AddEntry(g1, f"CSC Alone, <= Slope")
    #legend.AddEntry(g3, f"CSC+GE1 SlopeBins, <= Slope")
    legend.AddEntry(g2, f"CSC+GE1, <= BendingAngle")

    legend.SetTextSize(0.)
    legend.SetBorderSize(0)
    legend.Draw()

    canvas.Modified()

    canvas.SaveAs(plotdir+f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}.pdf")

    print("Saved")


    legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)

    mgname = f"MG_EMTF{emtfpt_cut}_RECO{recopt_cut}"
    mg2 = ROOT.TMultiGraph(mgname, mgname)

    print("New multigraph")

    xAxis = mg2.GetXaxis()
    xAxis.SetTitleSize(0.04)
    xAxis.SetTitleOffset(2)
    xAxis.SetTitle(f"Trigger Rate Remaining (EMTFPt > {emtfpt_cut})")
    #Trigger rate reduction factor

    yAxis = mg2.GetYaxis()
    yAxis.SetTitleSize(0.04)
    yAxis.SetTitleOffset(2)
    yAxis.SetTitle(f"Marginal Efficiency / 1GeV (RecoPt = {recopt_cut})")

    #mg2.Add(g1, "lp")
    mg2.Add(g2, "l")
    mg2.Add(g3, "p")
    #mg2.Add(g4, "p")
    mg2.Draw("AP")

    print("Draw")

    mg2.SetMinimum(0.5)



    #legend.AddEntry(g1, f"CSC Alone, <= Slope")
    #legend.AddEntry(g3, f"CSC+GE1 SlopeBins, <= Slope")
    legend.AddEntry(g2, f"CSC+GE1, <= BendingAngle")

    legend.SetTextSize(0.)
    legend.SetBorderSize(0)
    legend.Draw()

    canvas.Modified()
    print("Modified")

    canvas.SaveAs(plotdir+f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}_CSCGEMSlope_CSCGEM.pdf")







    legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
    mgname = f"MG_EMTF{emtfpt_cut}_RECO{recopt_cut}"
    mg3 = ROOT.TMultiGraph(mgname, mgname)

    xAxis = mg3.GetXaxis()
    xAxis.SetTitleSize(0.04)
    xAxis.SetTitleOffset(2)
    xAxis.SetTitle(f"Trigger Rate Remaining (EMTFPt > {emtfpt_cut})")
    #Trigger rate reduction factor

    yAxis = mg3.GetYaxis()
    yAxis.SetTitleSize(0.04)
    yAxis.SetTitleOffset(2)
    yAxis.SetTitle(f"Marginal Efficiency / 1GeV (RecoPt = {recopt_cut})")

    mg3.Add(g1, "lp")
    mg3.Add(g2, "l")
    #mg3.Add(g3, "p")
    #mg3.Add(g4, "p")
    mg3.Draw("AP")

    mg3.SetMinimum(0.5)



    legend.AddEntry(g1, f"CSC Alone, <= Slope")
    #legend.AddEntry(g3, f"CSC+GE1 SlopeBins, <= Slope")
    legend.AddEntry(g2, f"CSC+GE1, <= BendingAngle")

    legend.SetTextSize(0.)
    legend.SetBorderSize(0)
    legend.Draw()

    canvas.Modified()

    canvas.SaveAs(plotdir+f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}_CSCSlope_CSCGEM.pdf")







    legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
    mgname = f"MG_EMTF{emtfpt_cut}_RECO{recopt_cut}"
    mg4 = ROOT.TMultiGraph(mgname, mgname)

    xAxis = mg4.GetXaxis()
    xAxis.SetTitleSize(0.04)
    xAxis.SetTitleOffset(2)
    xAxis.SetTitle(f"Trigger Rate Remaining (EMTFPt > {emtfpt_cut})")
    #Trigger rate reduction factor

    yAxis = mg4.GetYaxis()
    yAxis.SetTitleSize(0.04)
    yAxis.SetTitleOffset(2)
    yAxis.SetTitle(f"Marginal Efficiency / 1GeV (RecoPt = {recopt_cut})")

    #mg4.Add(g1, "lp")
    mg4.Add(g2, "l")
    #mg4.Add(g3, "p")
    #mg4.Add(g4, "p")
    mg4.Draw("AP")

    mg4.SetMinimum(0.5)



    #legend.AddEntry(g1, f"CSC Alone, <= Slope")
    #legend.AddEntry(g3, f"CSC+GE1 SlopeBins, <= Slope")
    legend.AddEntry(g2, f"CSC+GE1, <= BendingAngle")

    legend.SetTextSize(0.)
    legend.SetBorderSize(0)
    legend.Draw()

    canvas.Modified()

    canvas.SaveAs(plotdir+f"ROC_recopt{recopt_cut}_emtfpt{emtfpt_cut}_CSCGEM.pdf")

    print("end")
print("end2")
