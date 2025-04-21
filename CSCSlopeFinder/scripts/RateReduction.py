import ROOT, sys, tdrstyle, os, array

#fname = "/eos/user/d/daebi/GEMCSCTrigger/2024E_ZeroBias/ZeroBias_EFG_18Sep2024.root"
fname = "/eos/user/d/daebi/GEMCSCTrigger/2024FGH_ZeroBias/2024FGH_ZeroBias_2Oct2024.root"
fname = "/eos/user/d/daebi/GEMCSCTrigger/2024FGH_ZeroBias/2024FGH_ZeroBias_26Oct2024.root"
fname = "/eos/user/d/daebi/GEMCSCTrigger/2024EFGHIJ_ZeroBias/2024EFGHIZeroBias_27Oct2024.root"

f = ROOT.TFile(fname)

event = f.Get("GEMCSCBendingAngleTester/HighestEMTFpT") #Use only highest EMTFpT in event for rate
ROOT.gROOT.SetBatch(1)
tdrstyle.setTDRStyle()

ROOT.EnableImplicitMT(8)
rdf = ROOT.RDataFrame(event)
ROOT.RDF.Experimental.AddProgressBar(rdf)

if not os.path.exists("plots/"):
  os.makedirs("plots/")


plotdir = "plots/RateReduction_ZeroBias30Oct/"
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

canvas = ROOT.TCanvas("c1", "c1", W, H)
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


legend_xmin = 0.45
legend_xmax = 0.85
legend_ymin = 0.45
legend_ymax = 0.85

yscale = 1.3

evenodd_list = ["odd", "even"]
dRmatch = 0.4

base_cut = "has_emtf_track_match & (has_LCT_match1)"
residual_cut = "((abs(LCT_match_GE1_residual) < {res_cut}))"
reco_cut = f"has_reco_l1_match & (abs(reco_l1_match_dR) < {dRmatch})"
mode_cut = f"((emtftrack_mode == 11) | (emtftrack_mode == 13) | (emtftrack_mode == 14) | (emtftrack_mode == 15))"
zmu_cut = f"(z_cand) & (abs(z_mass-91) < 10) & (z_opposite_charge)"

xbins = 50
xlow = 0
xhigh = 50

doRegionLevel = True
doChargeLevel = True

do_min_test = False

reglist = [-1, 1]
chargelist = [-1, 1]

for evenodd in evenodd_list:
    cut = f"{base_cut} & {mode_cut}"# & {mode_cut}" #Add residual cut later for even/odd difference
    res_cut = 100
    bacutlist = [1000, 28, 24, 16, 12, 10, 8]
    ba_min = -5
    if evenodd == "even":
        cut = cut + " & (LCT_CSC_chamber%2 == 0)"
        emtfcutlist = [22]
        bacutlist = [1000, 12, 10, 8, 6, 5, 4]
        res_cut  = 20
        ba_min = -2
    if evenodd == "odd":
        cut = cut + " & (LCT_CSC_chamber%2 == 1)"
        emtfcutlist = [22]
        bacutlist = [1000, 28, 24, 16, 12, 10, 8]
        res_cut = 40
        ba_min = -4

    cut = cut + " & " + residual_cut.format(res_cut = res_cut)

    #cut = cut + " & emtftrack_charge == -1"
    #cut = cut + " & LCT_GE1_region == 1"
    cut = cut + " & LCT_CSC_ME1b == 1"

    rdf_tmp = rdf.Filter(cut)


    colorlist = [ROOT.kBlack, ROOT.kOrange-3, ROOT.kCyan+1, ROOT.kMagenta, ROOT.kBlue, ROOT.kRed, ROOT.kGreen+3]


    for reg in reglist:
        if not doRegionLevel:
            if reg in [-1, 1]: continue
        cut_reg = cut + f" & ((LCT_GE1_region == -1) | (LCT_GE1_region == 1))" if reg == 0 else cut + f" & (LCT_GE1_region == {reg})"
        for charge in chargelist:
            if not doChargeLevel:
                if charge in [-1, 1]: continue
            cut_charge = cut_reg + f" & ((emtftrack_charge == -1) | (emtftrack_charge == 1))" if charge == 0 else cut_reg + f" & (emtftrack_charge == {charge})"

            hlist = []
            hlist_notnorm = []
            h_nums = []
            for i, ba_cut in enumerate(bacutlist):
                plot_branch = "emtftrack_pt"
                xAxis_title = "EMTFTrack pT Threshold"

                hname = f"BACut{ba_cut}"
                hlist.append(ROOT.TH1D(hname, hname, xbins, xlow, xhigh))
                hlist_notnorm.append(ROOT.TH1D(hname, hname, xbins, xlow, xhigh))
                xAxis = hlist[i].GetXaxis()
                xAxis.SetTitleOffset(2)
                xAxis.SetTitleSize(0.04)
                xAxis.SetTitle(f"{xAxis_title}")

                ba_min_tmp = -1000 if (i == 0) else ba_min

                #cut1 = cut_charge + f" & abs(BendingAngleRealistic) <= {ba_cut}"
                if do_min_test:
                    cut1 = cut_charge + f" & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) < {ba_cut}) & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) >= {ba_min_tmp})"
                else:
                    cut1 = cut_charge + f" & ((emtftrack_charge*LCT_GE1_region*LCT_BendingAngle_GE1*(-1)) < {ba_cut})"


                hlist[i].SetLineColor(colorlist[i])
                hlist_notnorm[i].SetLineColor(colorlist[i])

                #Prepare the count numbers in RDF for all cuts
                h_nums.append([])

                for xbin in range(xbins):
                    cut1_num = cut1 + f" & emtftrack_pt >= {xbin}"
                    h_nums[i].append(rdf_tmp.Filter(cut1_num).Count())


            nEntries = event.GetEntries(cut)

            h_nums_count = []

            legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
            legend.SetHeader(f"{evenodd} -- R{reg} Charge{charge}", "C")

            for i in range(len(h_nums)):
                h_nums_count.append([x.GetValue() for x in h_nums[i]])
                for xbin in range(xbins):
                    hlist[i].SetBinContent(xbin+1, h_nums_count[i][xbin]/nEntries)
                    hlist_notnorm[i].SetBinContent(xbin+1, h_nums_count[i][xbin])

                hlist[i].SetMarkerSize(0)
                ba_min_tmp = -1000 if (i == 0) else ba_min

                if do_min_test:
                    legend.AddEntry(hlist[i], f"{ba_min_tmp} <= BendingAngleCut <= {bacutlist[i]}")
                else:
                    legend.AddEntry(hlist[i], f"BendingAngleCut <= {bacutlist[i]}")



                #legend.AddEntry(hlist[i], f"CSC+GE1 Bending Angle <= {bacutlist[i]}")
                xAxis = hlist[i].GetXaxis()
                xAxis.SetTitleOffset(2)
                xAxis.SetTitleSize(0.04)
                xAxis.SetTitle(f"{xAxis_title}")
                yAxis_title = "Trigger Rate (A.U.)"
                yAxis = hlist[i].GetYaxis()
                yAxis.SetTitleOffset(2)
                yAxis.SetTitleSize(0.04)
                yAxis.SetTitle(f"{yAxis_title}")   
                if i == 0:
                    hlist[i].Draw("h")
                else:
                    hlist[i].Draw("h same")


            legend.SetTextSize(0.)
            legend.SetBorderSize(0)
            legend.Draw()

            latex = ROOT.TLatex()
            latex.SetNDC()
            latex.SetTextAngle(0)
            latex.SetTextColor(ROOT.kBlack)

            latex.SetTextAlign(12)
            latex.SetTextSize(0.8*ROOT.gPad.GetTopMargin())
            latex.SetTextFont(61)
            latex.DrawLatex(ROOT.gPad.GetLeftMargin(), 1 - 0.4*ROOT.gPad.GetTopMargin(), "CMS")
            latex.SetTextFont(52)
            latex.SetTextAlign(32)
            latex.SetTextSize(0.6*ROOT.gPad.GetTopMargin())
            latex.DrawLatex(1-ROOT.gPad.GetRightMargin(), 1 - 0.45*ROOT.gPad.GetTopMargin(), "Work in Progress")

            frame = canvas.GetFrame()
            frame.Draw()

            canvas.SetLogy(0)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}.pdf")
            canvas.SaveAs(plotname)

            canvas.SetLogy()

            hlist[0].SetMaximum(1e0)
            hlist[0].SetMinimum(1e-5)
            canvas.Update()

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_logy.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_logy.pdf")
            canvas.SaveAs(plotname)

            canvas.SetLogy(0)

            hlist[0].SetMaximum(0.1)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_ZOOMED1.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_ZOOMED1.pdf")
            canvas.SaveAs(plotname)


            hlist[0].SetMaximum(0.01)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_ZOOMED2.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_ZOOMED2.pdf")
            canvas.SaveAs(plotname)



            hlist[0].SetMaximum(0.003)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_ZOOMED3.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_ZOOMED3.pdf")
            canvas.SaveAs(plotname)

            hlist[0].SetMaximum(0.0005)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_ZOOMED4.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_ZOOMED4.pdf")
            canvas.SaveAs(plotname)



            legend = ROOT.TLegend(legend_xmin, legend_ymin, legend_xmax, legend_ymax)
            legend.SetHeader(f"{evenodd} -- R{reg} Charge{charge}", "C")

            for i in range(len(h_nums)):
                ba_min_tmp = -1000 if (i == 0) else ba_min

                hlist_notnorm[i].SetMarkerSize(0)

                if do_min_test:
                    legend.AddEntry(hlist_notnorm[i], f"{ba_min_tmp} <= BendingAngleCut <= {bacutlist[i]}")
                else:
                    legend.AddEntry(hlist_notnorm[i], f"BendingAngleCut <= {bacutlist[i]}")


                #legend.AddEntry(hlist_notnorm[i], f"CSC+GE1 Bending Angle <= {bacutlist[i]}")
                xAxis = hlist_notnorm[i].GetXaxis()
                xAxis.SetTitleOffset(2)
                xAxis.SetTitleSize(0.04)
                xAxis.SetTitle(f"{xAxis_title}")
                yAxis_title = "Trigger Rate (A.U.)"
                yAxis = hlist_notnorm[i].GetYaxis()
                yAxis.SetTitleOffset(2)
                yAxis.SetTitleSize(0.04)
                yAxis.SetTitle(f"{yAxis_title}")   
                if i == 0:
                    hlist_notnorm[i].Draw("h")
                else:
                    hlist_notnorm[i].Draw("h same")


            legend.SetTextSize(0.)
            legend.SetBorderSize(0)
            legend.Draw()

            latex = ROOT.TLatex()
            latex.SetNDC()
            latex.SetTextAngle(0)
            latex.SetTextColor(ROOT.kBlack)

            latex.SetTextAlign(12)
            latex.SetTextSize(0.8*ROOT.gPad.GetTopMargin())
            latex.SetTextFont(61)
            latex.DrawLatex(ROOT.gPad.GetLeftMargin(), 1 - 0.4*ROOT.gPad.GetTopMargin(), "CMS")
            latex.SetTextFont(52)
            latex.SetTextAlign(32)
            latex.SetTextSize(0.6*ROOT.gPad.GetTopMargin())
            latex.DrawLatex(1-ROOT.gPad.GetRightMargin(), 1 - 0.45*ROOT.gPad.GetTopMargin(), "Work in Progress")

            frame = canvas.GetFrame()
            frame.Draw()

            canvas.SetLogy(1)

            hlist_notnorm[0].SetMinimum(1e1)
            hlist_notnorm[0].SetMaximum(1e6)

            plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_bamin{ba_min}_notnorm.pdf")
            if not do_min_test: plotname = os.path.join(plotdir, f"RateScan_{evenodd}_R{reg}_Charge{charge}_notnorm.pdf")
            canvas.SaveAs(plotname)




