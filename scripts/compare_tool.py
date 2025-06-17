#### this is for comparing two separate results, given a file/bunch of files,
#### The eventual cuts to apply can be put in a specific config file in your area
#### thus you need to set the config file path in the script below
#### the comparison can be event-by-event or file-based for the moment

"""
import argparse
import os
import sys
import yaml

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare two sets of results.")
    parser.add_argument(
        "--config",
        type=str,
        required=True,
        help="Path to the configuration file containing comparison settings.",
    )
    parser.add_argument(
        "--file1",
        type=str,
        required=True,
        help="Path to the first result file or directory.",
    )
    parser.add_argument(
        "--file2",
        type=str,
        required=True,
        help="Path to the second result file or directory.",
    )

    args = parser.parse_args()

    # Load configuration
    with open(args.config, 'r') as config_file:
        config = yaml.safe_load(config_file)

    # Implement comparison logic here
    print(f"Comparing {args.file1} with {args.file2} using configuration: {config}")
"""


import awkward as ak
import numpy as np
import uproot
import ROOT
import psutil
import os


emu_name = "/eos/user/d/daebi/patrick_readerfile_zmu2024I/patrick_full.root"

offline_name = "/eos/user/d/daebi/patrick_readerfile_zmu2024I/me_full.root"

print("Opening files")
emu_file = uproot.open(emu_name)
offline_file = uproot.open(offline_name)

emu_vars_to_load = ['RUN', 'Event', 'endcap', 'chamber', 'keyWG', 'eightStrip', 'quality', 'slope', 'bend', 'residual', 'bendingangle', 'layer2bool', 'is_emul', 'clusterbx']
offline_vars_to_load = ['runNum', 'lumiBlock', 'evtNum', 'LCT_CSC_endcap', 'LCT_CSC_chamber', 'LCT_wiregroup', 'LCT_eighthstrip', 'LCT_quality', 'LCT_slope', 'LCT_bend', 'LCT_match_GE1_residual', 'LCT_match_GE2_residual', 'LCT_BendingAngle_GE1', 'LCT_BendingAngle_GE2', 'LCT_match_GE1_BX', 'LCT_match_GE2_BX']

print("Converting to arrays")
emu_tree = emu_file['GEMCSCTriggerPrimitivesReader/LCT_tree'].arrays(emu_vars_to_load, 'Event < 10000')
offline_tree = offline_file['GEMCSCBendingAngleTester/AllLCTs'].arrays(offline_vars_to_load, 'evtNum < 10000')

print(f"Made Arrays. Memory usage in MB is {psutil.Process(os.getpid()).memory_info()[0] / float(2 ** 20)}")



# First scan over all LCTs and find same event/chamber/strip events
# We need to flatten the Emu branches into the correct shape

emu_tree = emu_tree[ak.all(emu_tree.is_emul == 1, axis=1)] # emu saves two LCT collection per event, [data, emu], filter by is_emul

emu_run = ak.flatten(ak.broadcast_arrays(emu_tree.RUN, emu_tree.chamber)[0])
emu_lumi = ak.flatten(ak.broadcast_arrays(emu_tree.RUN, emu_tree.chamber)[0])
emu_event = ak.flatten(ak.broadcast_arrays(emu_tree.Event, emu_tree.chamber)[0])
emu_endcap = ak.flatten(emu_tree.endcap)
emu_chamber = ak.flatten(emu_tree.chamber)
emu_eightStrip = ak.flatten(emu_tree.eightStrip)
emu_quality = ak.flatten(emu_tree.quality)
emu_residual = ak.flatten(emu_tree.residual)
emu_bendingangle = ak.flatten(emu_tree.bendingangle)
emu_bx = ak.flatten(emu_tree.clusterbx)
emu_layer2bool = ak.flatten(emu_tree.layer2bool)

class LCT_object:
    def __init__(self, run, lumi, event, endcap, chamber, eightStrip, quality):
        self.run = run
        self.lumi = lumi
        self.event = event
        self.endcap = endcap
        self.chamber = chamber
        self.eightStrip = eightStrip
        self.quality = quality
        self.residual_layer1 = None
        self.residual_layer2 = None
        self.bendingangle_layer1 = None
        self.bendingangle_layer2 = None
        self.bx_layer1 = None
        self.bx_layer2 = None

    def set_layer(self, layer2bool, residual, bendingangle, bx):
        if layer2bool:
            self.residual_layer2 = residual
            self.bendingangle_layer2 = bendingangle
            self.bx_layer2 = bx
        else:
            self.residual_layer1 = residual
            self.bendingangle_layer1 = bendingangle
            self.bx_layer1 = bx

# Build a dict to group by (event, chamber, eightStrip, quality)
lct_dict = {}
print("Starting the emu dict")
for run, lumi, evt, end, ch, es, q, res, ba, bx, l2 in zip(emu_run, emu_lumi, emu_event, emu_endcap, emu_chamber, emu_eightStrip, emu_quality, emu_residual, emu_bendingangle, emu_bx, emu_layer2bool):
    key = (run, evt, end, ch, es, q)
    if key not in lct_dict:
        lct_dict[key] = LCT_object(run, lumi, evt, end, ch, es, q)
    lct_dict[key].set_layer(l2, res, ba, bx)

lct_objects = list(lct_dict.values())





offline_event = offline_tree.evtNum
offline_chamber = offline_tree.LCT_CSC_chamber
offline_eightStrip = offline_tree.LCT_eighthstrip
offline_quality = offline_tree.LCT_quality



# Build a dict to group by (event, chamber, eightStrip, quality)
offline_lct_dict = {}
print("Starting the offline dict")
for run, lumi, evt, end, ch, es, q, res1, res2, ba1, ba2, bx1, bx2 in zip(
    offline_tree.runNum,
    offline_tree.lumiBlock,
    offline_tree.evtNum,
    offline_tree.LCT_CSC_endcap,
    offline_tree.LCT_CSC_chamber,
    offline_tree.LCT_eighthstrip,
    offline_tree.LCT_quality,
    offline_tree.LCT_match_GE1_residual,
    offline_tree.LCT_match_GE2_residual,
    offline_tree.LCT_BendingAngle_GE1,
    offline_tree.LCT_BendingAngle_GE2,
    offline_tree.LCT_match_GE1_BX,
    offline_tree.LCT_match_GE2_BX
):
    key = (run, evt, end, ch, es, q)
    if key not in offline_lct_dict:
        offline_lct_dict[key] = LCT_object(run, lumi, evt, end, ch, es, q)
    offline_lct_dict[key].set_layer(0, res1, ba1, bx1)
    offline_lct_dict[key].set_layer(1, res2, ba2, bx2)

offline_lct_objects = list(offline_lct_dict.values())


print(f"Made LCT Lists. Memory usage in MB is {psutil.Process(os.getpid()).memory_info()[0] / float(2 ** 20)}")



# Build sets of keys for both lists
emu_keys = set((l.run, l.event, l.endcap, l.chamber, l.eightStrip, l.quality) for l in lct_objects)
offline_keys = set((l.run, l.event, l.endcap, l.chamber, l.eightStrip, l.quality) for l in offline_lct_objects)

# LCTs in emu but not in offline
emu_not_in_offline_keys = emu_keys - offline_keys
emu_not_in_offline = [l for l in lct_objects if (l.run, l.event, l.endcap, l.chamber, l.eightStrip, l.quality) in emu_not_in_offline_keys]

# LCTs in offline but not in emu
offline_not_in_emu_keys = offline_keys - emu_keys
offline_not_in_emu = [l for l in offline_lct_objects if (l.run, l.event, l.endcap, l.chamber, l.eightStrip, l.quality) in offline_not_in_emu_keys]

print(f"LCTs in emu but not in offline: {len(emu_not_in_offline)}")
print(f"LCTs in offline but not in emu: {len(offline_not_in_emu)}")

print(f"Memory usage in MB is {psutil.Process(os.getpid()).memory_info()[0] / float(2 ** 20)}")


# Find LCTs where the quality does not match between emu and offline
quality_mismatch = []
# Build dicts for fast lookup by (event, chamber, eightStrip)
emu_dict = {(l.run, l.event, l.endcap, l.chamber, l.eightStrip): l for l in lct_objects}
offline_dict = {(l.run, l.event, l.endcap, l.chamber, l.eightStrip): l for l in offline_lct_objects}

common_keys = set(emu_dict.keys()) & set(offline_dict.keys())
for key in common_keys:
    emu_lct = emu_dict[key]
    offline_lct = offline_dict[key]
    if emu_lct.quality != offline_lct.quality:
        quality_mismatch.append((key, emu_lct.quality, offline_lct.quality))

print(f"LCTs with mismatched quality: {len(quality_mismatch)}")

print(f"Memory usage in MB is {psutil.Process(os.getpid()).memory_info()[0] / float(2 ** 20)}")


# Find LCTs where the residuals or bending angles do not match between emu and offline
residual_bending_mismatch = []
for key in common_keys:
    emu_lct = emu_dict[key]
    offline_lct = offline_dict[key]
    # Compare both layers for residuals and bending angles
    if (
        not emu_lct.residual_layer1 == offline_lct.residual_layer1 or
        not emu_lct.residual_layer2 == offline_lct.residual_layer2 or
        not emu_lct.bendingangle_layer1 == offline_lct.bendingangle_layer1 or
        not emu_lct.bendingangle_layer2 == offline_lct.bendingangle_layer2
    ):
        residual_bending_mismatch.append((
            key,
            (emu_lct.residual_layer1, offline_lct.residual_layer1),
            (emu_lct.residual_layer2, offline_lct.residual_layer2),
            (emu_lct.bendingangle_layer1, offline_lct.bendingangle_layer1),
            (emu_lct.bendingangle_layer2, offline_lct.bendingangle_layer2)
        ))

print(f"LCTs with mismatched residuals or bending angles: {len(residual_bending_mismatch)}")

print(f"Memory usage in MB is {psutil.Process(os.getpid()).memory_info()[0] / float(2 ** 20)}")
