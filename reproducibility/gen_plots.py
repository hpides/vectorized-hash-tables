import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
import re
import warnings
import itertools

def generate_result_tex_file(variable_dict, output_path):
    tex_content = "%!TEX root = ../thesis.tex\n\n% File was automatically generated.\n\n"
    
    tex_content += "\\newcommand{\\DefineResult}[2]{%\n"
    tex_content += "\t\\expandafter\\newcommand\\csname rmk-#1\\endcsname{#2}%\n"
    tex_content += "}\n"
    tex_content += "\\newcommand{\\Result}[1]{\\csname rmk-#1\\endcsname}\n\n"
    
    
    for identifier, value in variable_dict.items():
        tex_content += "\\DefineResult{" + str(identifier) + "}{" + str(value) + "}\n"
    
    with open(output_path, "w") as output_file:
        output_file.write(tex_content)
        
def cpu_to_prefix(cpu):
    if cpu == "A64FX ARM":
        return "A64FX"
    
    if cpu == "Intel x86":
        return "Int"
    
    if cpu == "AMD x86":
        return "Amd"
    
    if cpu == "Power":
        return "Pwr"
    
    raise ValueError(f"unknown cpu {cpu}")
        

colors = ["#E69F00" ,"#009E73" ,"#0072B2" ,"#999999"]
colors_lighter = ["#f0c466" ,"#80cfba" ,"#80b8d9" ,"#cccccc"]

sns.set_style("ticks")

USE_SVG = False

if not USE_SVG:
    from matplotlib.backends.backend_pgf import FigureCanvasPgf
    matplotlib.backend_bases.register_backend('pdf', FigureCanvasPgf)

    plt.rcParams.update({
            'svg.fonttype': 'none',
            "pgf.texsystem":   "pdflatex", # or any other engine you want to use
            "text.usetex":     True,       # use TeX for all texts
            "font.family":     "serif",
            "font.serif":      [],         # empty entries should cause the usage of the document fonts
            "font.sans-serif": [],
            "font.monospace":  [],
            # Use 10pt font in plots, to match 10pt font in document
            "axes.labelsize": 10,
            "font.size": 10,
            # Make the legend/label fonts a little smaller
            "legend.fontsize": 9,
            "xtick.labelsize": 9,
            "ytick.labelsize": 9
    })
else:
    plt.rcParams.update({
            'svg.fonttype': 'none'
    })
    
    
plt.rc('axes', axisbelow=True) 
pd.set_option('display.max_columns', None)
pd.set_option('display.expand_frame_repr', False)
pd.set_option('max_colwidth', None)

HM_SIZE = 134217728 # size of the hashmap we are interested in in the analysis

result_dict = {}

MAJOR_LINE_THICKNESS_PRINT = 1.2
MINOR_LINE_THICKNESS_PRINT = 0.7

MAJOR_LINE_THICKNESS_DIGITAL = 0.8
MINOR_LINE_THICKNESS_DIGITAL = 0.3

MAJOR_LINE_THICKNESS = MAJOR_LINE_THICKNESS_DIGITAL
MINOR_LINE_THICKNESS = MINOR_LINE_THICKNESS_DIGITAL

DISABLE_ASSERTS = False
DATA_LINEWIDTH = 0.8
MARKER_EDGEWIDTH = 0.3
MARKER_SIZE = 4
MARKER_EDGECOLOR = "black"
LABEL_ROTATION=0
LEGEND_HANDLELENGTH = 1
LEGEND_HANDLETEXTPAD = 0.1
LEGEND_COLUMNSPACING = 0.2

def load_read_benchmark_data(use_mean_aggregation = True):  # if false, median is used
    intel_x86_file = "reads_intel.csv"
    amd_x86_file = "reads_amd.csv"
    power_file = "reads_power.csv"
    a64fx_arm_file = "reads_arm.csv"

    intel_x86_results = pd.read_csv(intel_x86_file, index_col=False)
    intel_x86_results.insert(0, "Arch", "Intel x86")
    intel_x86_results.reset_index(drop=True, inplace=True)

    amd_x86_results = pd.read_csv(amd_x86_file, index_col=False)
    amd_x86_results.insert(0, "Arch", "AMD x86")
    amd_x86_results.reset_index(drop=True, inplace=True)
    
    a64fx_arm_results = pd.read_csv(a64fx_arm_file, index_col=False)    
    a64fx_arm_results.insert(0, "Arch", "A64FX ARM")
    a64fx_arm_results.reset_index(drop=True, inplace=True)

    power_results = pd.read_csv(power_file, index_col=False)
    power_results.insert(0, "Arch", "Power")
    power_results.reset_index(drop=True, inplace=True)
    
    merged_results = pd.concat([intel_x86_results, amd_x86_results, a64fx_arm_results, power_results], ignore_index = True, verify_integrity=True)
    merged_results = merged_results.loc[merged_results["Successful"] == True]

    # calculate throughput
    merged_results["WorkloadInBytes"] = merged_results["Workload"] * 1024**2 # probably not even used but whatever
    merged_results["Lookups/s"] = (merged_results["EntriesProcessed"].astype(float) * merged_results["ThreadCount"]) / (merged_results["Runtime"].astype(float) / 1000.0)
    merged_results["MLookups/s"] = merged_results["Lookups/s"].astype(float) / 1000000.0
    
    # calculate mean or median for same columns
    group_attributes = ["Arch", "Hashmap", "Compiler", "PageSize", "HugePageSize", "LoadFactor", "SQR", "Size", "Distribution", "Workload", "KeySize", "ValueSize", "EntrySize", "EntriesProcessed", "Zipf", "ThreadCount"]

    if use_mean_aggregation:
        merged_results = merged_results.groupby(group_attributes).mean().reset_index()
    else:
        merged_results = merged_results.groupby(group_attributes).median().reset_index()

    # calculate mt speedup
    def get_st_lookups(row, df1):
        if row["ThreadCount"] == 1:
            return row["Runtime"]
        
        tmp = df1[(df1["Arch"] == row["Arch"]) & (df1["Hashmap"] == row["Hashmap"]) & (df1["Compiler"] == row["Compiler"]) & (df1["Zipf"] == row["Zipf"]) & (df1["PageSize"] == row["PageSize"]) & (df1["HugePageSize"] == row["HugePageSize"]) & (df1["SQR"] == row["SQR"]) & (df1["Size"] == row["Size"]) & (df1["Distribution"] == row["Distribution"]) & (df1["LoadFactor"] == row["LoadFactor"]) & (df1["ThreadCount"] == 1)]
        if len(tmp) == 0:
            if row["Hashmap"].startswith("ChainedHashTable"):
                return float('nan')
            else:
                print(row)
                print(tmp)
                raise ValueError(row)
        else:
            if len(tmp) > 1:
                print(row)
                print(tmp)
                raise ValueError(row)
            
        return tmp.iloc[0]["Runtime"]

    merged_results["STPerf"] = merged_results.apply(lambda row: get_st_lookups(row, merged_results), axis=1)
    merged_results["STSpeedup"] = merged_results["STPerf"] / merged_results["Runtime"].astype(float)
        
    merged_results = merged_results[merged_results["Size"] == HM_SIZE]
    merged_results["ThreadCount"] = merged_results["ThreadCount"].astype(int)
    merged_results["Zipf"] = merged_results["Zipf"].astype(bool)

    return merged_results

def load_write_benchmark_data(use_mean_aggregation = True):  # if false, median is used
    intel_x86_file = "writes_intel.csv"
    amd_x86_file = "writes_amd.csv"
    power_file = "writes_power.csv"
    a64fx_arm_file = "writes_arm.csv"

    intel_x86_results = pd.read_csv(intel_x86_file, index_col=False) 
    intel_x86_results.insert(0, "Arch", "Intel x86")
    intel_x86_results.reset_index(drop=True, inplace=True)

    amd_x86_results = pd.read_csv(amd_x86_file, index_col=False)
    amd_x86_results.insert(0, "Arch", "AMD x86")
    amd_x86_results.reset_index(drop=True, inplace=True)
        
    a64fx_arm_results = pd.read_csv(a64fx_arm_file, index_col=False)    
    a64fx_arm_results.insert(0, "Arch", "A64FX ARM")
    a64fx_arm_results.reset_index(drop=True, inplace=True)

    power_results = pd.read_csv(power_file, index_col=False)
    power_results.insert(0, "Arch", "Power")
    power_results.reset_index(drop=True, inplace=True)
    
    merged_results = pd.concat([x for x in [intel_x86_results, amd_x86_results, a64fx_arm_results, power_results] if x is not None], ignore_index = True, verify_integrity=True)

    # calculate throughput
    merged_results["Inserts/s"] = (merged_results["EntriesProcessed"].astype(float) * merged_results["ThreadCount"]) / (merged_results["Runtime"].astype(float) / 1000.0)
    merged_results["MInserts/s"] = merged_results["Inserts/s"].astype(float) / 1000000.0
   
    # calculate mean or median for same columns

    group_attributes = ["Arch", "Hashmap", "Compiler", "PageSize", "HugePageSize", "LoadFactor", "Size", "Distribution", "KeySize", "ValueSize", "EntrySize", "EntriesProcessed", "ThreadCount"]

    if use_mean_aggregation:
        merged_results = merged_results.groupby(group_attributes).mean().reset_index()
    else:
        merged_results = merged_results.groupby(group_attributes).median().reset_index()

    # calculate mt speedup
    def get_st_lookups(row, df1):
        if row["ThreadCount"] == 1:
            return row["Runtime"]
        
        tmp = df1[(df1["Arch"] == row["Arch"]) & (df1["Hashmap"] == row["Hashmap"]) & (df1["Compiler"] == row["Compiler"]) & (df1["PageSize"] == row["PageSize"]) & (df1["HugePageSize"] == row["HugePageSize"]) & (df1["Size"] == row["Size"]) & (df1["Distribution"] == row["Distribution"]) & (df1["LoadFactor"] == row["LoadFactor"]) & (df1["ThreadCount"] == 1)]
        if len(tmp) == 0:
            if row["Hashmap"].startswith("ChainedHashTable"):
                return float('nan')
            else:
                print(row)
                print(tmp)
                raise ValueError(row)
        else:
            if len(tmp) > 1:
                print(row)
                print(tmp)
                raise ValueError(row)
            
        return tmp.iloc[0]["Runtime"]

    merged_results["STPerf"] = merged_results.apply(lambda row: get_st_lookups(row, merged_results), axis=1)
    merged_results["STSpeedup"] = merged_results["STPerf"] / merged_results["Runtime"].astype(float)        
        
    merged_results = merged_results[merged_results["Size"] == HM_SIZE]
    merged_results["ThreadCount"] = merged_results["ThreadCount"].astype(int)
    return merged_results


read_benchmark_results = load_read_benchmark_data()
write_benchmark_results = load_write_benchmark_data()

def set_size(width, fraction=1, subplots=(1, 1)):
    """Set figure dimensions to avoid scaling in LaTeX.

    Parameters
    ----------
    width: float or string
            Document width in points, or string of predined document type
    fraction: float, optional
            Fraction of the width which you wish the figure to occupy
    subplots: array-like, optional
            The number of rows and columns of subplots.
    Returns
    -------
    fig_dim: tuple
            Dimensions of figure in inches
    """
    if width == 'thesis':
        width_pt = 426.79135
    elif width == 'beamer':
        width_pt = 307.28987
    else:
        width_pt = width

    # Width of figure (in pts)
    fig_width_pt = width_pt * fraction
    # Convert from pt to inches
    inches_per_pt = 1 / 72.27

    # Golden ratio to set aesthetic figure height
    # https://disq.us/p/2940ij3
    golden_ratio = (5**.5 - 1) / 2

    # Figure width in inches
    fig_width_in = fig_width_pt * inches_per_pt
    # Figure height in inches
    fig_height_in = fig_width_in * golden_ratio * (subplots[0] / subplots[1])

    return (fig_width_in, fig_height_in)

from matplotlib.ticker import FuncFormatter
deep_palette = sns.color_palette('deep', n_colors=10)

def analysis_plot(hashmap_filter, palette_dict, ARCHITECTURES, LOAD_FACTORS, width, show_thp = True, show_pref = True, show_unroll = True,
                  show_avx512 = True, show_intel_512 = False,
                  hide_svbcast=False, hide_non_svbcast=False, hide_non_s2n=False, hide_non_avx512 = False, hide_external=True,
                  split_by_simd_size = False, hide_non_thp = False, word_filter=[], simd_sizes = [], exclude_prefixes = [], 
                  file_path=None, force_include=None, power_force_include=None, ncol_legend=8, collect_all_labels=True, 
                  hide_soa = True, hide_clang = True,  hide_msb_fingerprints=True, hide_likely_hints=True, hide_non_likely_hints=False, hide_vec_iterators=True,
                  hide_notestz=False, hide_lsb_fingerprints=False, legend_label_dict = {}, hide_simd_larger_128 = False, legend_height_shift = 0,
                  hide_s2n=False, hide_non_512_on_intel=False, hide_non_256_on_amd = False, hide_sve=False, hide_neon=False, hide_16b_fingerprints=False, hide_avx512_smaller_512=False, additional_include_filter=[],
                  hide_vfp_16b=False, show_nonmultshift64=False, hide_multshift64=False, show_modulo=False, hide_non_modulo=False, show_dense=False, hide_uniform=False,
                  hide_8b_fingerprints=False, hide_xx_hash=False, hide_128b_hasher=False, hide_multaddshift_murmur=False, hide_multaddshift=False, hide_msblsb_multshift=False,
                  show_stringkeys=False, show_inline_dummytuple=False, show_pointer_dummytuple=False,show_intpointer=False, only_show_dummy=False, only_show_dummy_and_intptr=False, filter_load_factor_90=False,
                  additional_prefetching=[], label_order = None, show_fallback = False, hide_256_on_intel = False, hide_msblsb_fingerprints=True, hide_lsbmsb_fingerprints=True, hide_non_kv_budget=True, ylabel_padding=0, xlabel_padding=0,
                  plot_uniform_and_dense = False, multshift_only_lsbmsb=False, hide_insertions=False, disable_legend_border=False, reduce_xticks=False, reduce_yticks=False, reduce_minor=False, other_ylimit=False, power_axis=False, single_row_lf90=False,
                  plot_speedup = False, force_sqr=None, force_write_lf=None, arch_ylimits={}, thread_count=1, analyze_threadcount=False, only_two_sqr=False, markevery=None, hide_zipf = True, hue_order=None, hide_non_zipf=False, extra_legend_columnspacing = 0, extra_legend_handlelength = 0,
                  hide_dummytuple128 = True):
    
    num_rows = len(ARCHITECTURES)
    to_enumerate = ARCHITECTURES
    
    if (plot_speedup or analyze_threadcount) and force_sqr is None:
        raise ValueError("need to supply force_sqr")
    
    if plot_speedup and analyze_threadcount:
        raise ValueError("can only do one of them")
    
    assert not (split_by_simd_size and plot_uniform_and_dense), "Can only have one of split_by_simd_size and plot_uniform_and_dense active"
    
    if show_stringkeys and (hide_xx_hash or not show_nonmultshift64):
        assert 1 == 0, "Stringkeys need xx hash"
    
    if split_by_simd_size:
        assert len(simd_sizes) > 0, "Need to tell me at least 1 simd sizes"
        assert len(ARCHITECTURES) == 1, "Can do SIMD Size Plot only for 1 Architecture ATM"
        num_rows = len(simd_sizes) 
        to_enumerate = simd_sizes
        
    if plot_uniform_and_dense:
        assert len(ARCHITECTURES) == 1, "Can do Dense/Uniform only for 1 Architecture ATM"
        num_rows = 2
        to_enumerate = ["Uniform", "Dense"]
        
    if single_row_lf90:
        num_rows = 1
        LOAD_FACTORS = [90]
        hide_insertions = True
        
    figsize = set_size(width, subplots=(num_rows,len(LOAD_FACTORS) + 1))
    xlabel_fontsize = 7
    ylabel_fontsize = 7
    title_fontsize = 8
    legend_fontsize = 8
    
    if hide_insertions:
        num_cols = len(LOAD_FACTORS)
    else:
        num_cols = len(LOAD_FACTORS) + 1
    
    if single_row_lf90:
        num_cols = len(ARCHITECTURES)
        
    figsize = set_size(width, subplots=(num_rows,num_cols))

    sharex = False if reduce_xticks else "col"
    
    fig, axes = plt.subplots(num_rows, num_cols, sharex=sharex, sharey="row", figsize=figsize)
    
    
    if not single_row_lf90:
        plt.subplots_adjust(hspace = 0.05, wspace = 0.07)
    else:
        plt.subplots_adjust(hspace = 0.05, wspace = 0.1)

    read_data = read_benchmark_results.copy()
    write_data = write_benchmark_results.copy()
    
    def map_name(row, hide_gcc = True):
        name = row["Hashmap"]

        new_name = f"Unknown_{name}"
        simd = False
        splitted = name.split(";")
        
        if "LinearProbingAoS" in name:
            new_name = "LP"
        elif "QuadraticProbingAoS" in name:
            new_name = "QP"
        elif "RecalculatingRobinHoodAoS" in name:
            new_name = "RecalcRH"
        elif "StoringRobinHoodAoS" in name:
            new_name = "StoreRH"
        elif "LinearProbingPackedSoA" in name:
            new_name = "LPPackedSoA"
        elif "LinearProbingSoA" in name:
            new_name = "LPSoA"
        elif "UnchunkedSIMDSoAHashTable" in name:
            new_name = "UnchkdSIMD"
            simd = True
        elif "FingerprintingSIMDSoAHashTable" in name:
            new_name = "FingerPSIMD"
            simd = True
        elif "BucketingSIMDHashTable" in name:
            new_name = "BucketSIMD"
            simd = True
        elif "AbseilFlatHashTable" in name:
            new_name = "AbseilFlat_Ext"
        elif "MartinusRHFlatHashTable" in name:
            new_name = "MRHFlat_Ext"
        elif "F14ValueHashTable" in name:
            new_name = "F14V_Ext"
        elif "ChainedHashTable" in name:
            new_name = "Chain"
        
        if new_name in ["FingerPSIMD", "BucketSIMD"]:
            fingerprint_t = splitted[3]
            
            if "uint8_t" in fingerprint_t:
                new_name += "_8B"
            elif "uint16_t" in fingerprint_t:
                new_name += "_16B"
            else:
                new_name += f"_UnknownFingerprintT{fingerprint_t.strip()}"
                        
            if "LSBLSB" in name:
                new_name += "_LSBLSB"
                
            if "MSBLSB" in name:
                new_name += "_MSBLSB"
                
            if "LSBMSB" in name:
                new_name += "_LSBMSB"
            
            if "FPPB" in name:
                for part in splitted:
                    if "FPPB" in part:
                        new_name += f"_{part.replace('>', '').strip()}"
                        
            if "Bucket" in name:
                for part in splitted:
                        if "KeyValueAoSStoringBucket" in part:
                            new_name += "_KVBu"
                            
            if "UMINV" in name:
                new_name += "_VecIt"
        
        if "AutoPadded" in name:
            new_name += "_AP"
        elif "NaturalAligned" in name:
            new_name += "_NA"
        elif "Unaligned" in name:
            new_name += "_UN"
            
        if "Budget" in name:
            for split in splitted:
                if "Budget" in split and "Additional" not in split:
                    new_name += f"_{split.replace('Budget', 'Bud').strip()}"
                    break

        if (not hide_gcc) or row["Compiler"] != "gcc":
            new_name += f"_{row['Compiler']}"
        
        is_fallback = False
        
        if simd:
            if "NO_TESTZ" in name:
                new_name += "_NOTESTZ"
            elif "TESTZ" in name:
                new_name += "_TESTZ"
            elif "MANUAL_ON_MATCH" in name:
                new_name += "_MOM"
            else:
                new_name += "_UnknownSIMDALGO"
 
            for part in splitted:
                if "Fallback" in part and "NoFallback" not in part:
                    new_name += f"_{re.sub('[^0-9]','', part)}"
                    is_fallback = True

            if row["Arch"] == "Intel x86" or row["Arch"] == "AMD x86":
                if not is_fallback:
                    simd_str = "_UnknownSIMDSize"
                    for part in splitted:
                        if "__" in part and "mmask" not in part:
                            simd_str = f"_{re.sub('[^0-9]','', part)}"

                    new_name += simd_str
                
                if "NoAVX512" not in name:
                    assert("AVX512" in name)
                    new_name += "_AVX512"
            
            elif row["Arch"] == "Power":

                if not is_fallback:
                    new_name += "_128"
                
                if "UnchunkedSIMDSoAHashTable" in name:
                    if "UMINV" in name:
                        new_name += "_Garbage"
                
            elif row["Arch"] == "A64FX ARM":
                if "NoSVE;" not in name:
                    assert("SVE" in name)
                    assert("svuint" in name)
                    assert("svbool" in name)
                    if not is_fallback:
                        new_name += "_512_SVE"
                    else:
                        new_name += "_SVE"

                    if "NoSVEBroadcast" not in name:
                        assert("SVEBroadcast" in name)
                        new_name += "_SVBCast"

                else:
                    assert("svuint" not in name), f"Invalid name {name}"
                    assert("svbool" not in name)

                    if not is_fallback:
                        new_name += "_128_NEON"
                    else:
                        new_name += "_NEON"
                        
                    if "SSE2NEON" in name:
                        new_name += "_S2N"
                    elif "AARCH64" in name:
                        new_name += "_A64MM"
                    elif "UMINV" in name:
                        new_name += "_UMINV"
                    else:
                        new_name += "_UnknownARMALGO"
            else:
                new_name += f"_UnkownSIMDArch{row['Arch']}"
        
        if "NoLikelyHints" not in name:
            if "LikelyHints" in name:
                new_name += "_Hints"
        
        if "NoPref" not in name:
            for part in splitted:
                if "Pref" in part:
                    new_name += f"_{part.strip()}"
                    
                if "AddPrf" in part and "NoAddPrf" not in part:
                    new_name += f"_{part.strip().replace('>','')}"

        if "NoTHP" not in name:
            new_name += "_THP"
            
        if "NoUnroll" not in name:
            for part in splitted:
                if "Unroll" in part:
                    new_name += f"_{part.strip().replace('>','')}"
                    
                    
        if "ModHasher" in name:
            new_name += "_ModFinal"
        else:
            assert("BitHasher") in name, f"Unknown finalizer in {name}"
            
        if "MultShift128B" in name:
            new_name += "_MultShift128"
        elif "MultAddShift64" in name:
            new_name += "_MultAddShift64"
        elif "MultAddShift128" in name:
            new_name += "_MultAddShift128"
        elif "Murmur" in name:
            new_name += "_Murmur"
        elif "XX" in name:
            new_name += "_XX"
        else:
            assert "MultShift64B" in name, f"Unkown hash in {name}"
                    
        assert("Unknown" not in new_name or DISABLE_ASSERTS), f"Found unknown hashmap {new_name} (orig name = {name})"

        if row["Distribution"] == "Dense":
            new_name += "_Dense"
        else:
            assert row["Distribution"] == "Uniform", f"Unhandled Distribution {row['Distribution']}"
        
        if "stringkey" in name:
            new_name += "_StringKey"
            
        if "DummyTuple" in name and "ptr_DummyTuple" not in name and "DummyTuple128" not in name:
            new_name += "_DummyTuple"
            
        if "ptr_DummyTuple" in name and "ptr_DummyTuple128" not in name:
            new_name += "_PtrDummyTuple"
            
        if "DummyTuple128" in name and "ptr_DummyTuple128" not in name:
            new_name += "_128DummyTuple"
        
        if "ptr_DummyTuple128" in name:
            new_name += "_Ptr128DummyTuple"
            
        if "ptr_uint64_t" in name:
            new_name += "_PtrUint"
            
        if is_fallback:
            new_name += "_Fallback"
            
        if "Zipf" in row and row["Zipf"] == True:
            new_name += "_Zipf"
            
        #if row["ThreadCount"] > 1:
        #    new_name += f"_Thread{row['ThreadCount']}"
        
        return new_name

    if force_include is None:
        force_include = []
        
    if power_force_include is None:
        power_force_include = []
        
    full_read_data = read_data.copy()
    full_write_data = write_data.copy()

    def filter_data(data):
        data["PlotIdentifier"] = data.apply(lambda row : map_name(row.copy()), axis=1)
        data = data[data["Runtime"] != 0]
        
        if len(additional_include_filter) > 0:
            data = data[data["Hashmap"].str.contains(hashmap_filter) | data["Hashmap"].str.contains('|'.join(additional_include_filter)) | data["PlotIdentifier"].isin(force_include) | ((data["Arch"] == "Power") & data["PlotIdentifier"].isin(power_force_include))]
        else:
            data = data[data["Hashmap"].str.contains(hashmap_filter) | data["PlotIdentifier"].isin(force_include) | ((data["Arch"] == "Power") & data["PlotIdentifier"].isin(power_force_include))]
        
        data = data.loc[~data["PlotIdentifier"].str.contains("_Garbage") | data["PlotIdentifier"].isin(force_include)]

        if split_by_simd_size or plot_uniform_and_dense:
            data = data[data["Arch"] == ARCHITECTURES[0]]
        
        if not show_thp:
            data = data.loc[~data["PlotIdentifier"].str.contains("THP") | data["PlotIdentifier"].isin(force_include)]

        if not show_pref:
            data = data.loc[~data["PlotIdentifier"].str.contains("Pref") | data["PlotIdentifier"].isin(force_include)]

        if not show_unroll:
            data = data.loc[~data["PlotIdentifier"].str.contains("Unroll") | data["PlotIdentifier"].isin(force_include)]

        if not show_avx512:
            if (split_by_simd_size and "512" in simd_sizes) or show_intel_512:
                data = data.loc[~data["PlotIdentifier"].str.contains("AVX512") | data["PlotIdentifier"].isin(force_include) | data["PlotIdentifier"].str.contains("_512") ]
            else:
                data = data.loc[~data["PlotIdentifier"].str.contains("AVX512") | ~data["Arch"].str.contains("x86") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_avx512:
            data = data.loc[data["PlotIdentifier"].str.contains("AVX512") |  ~data["Arch"].str.contains("Intel") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_thp:
            data = data.loc[data["PlotIdentifier"].str.contains("THP") | data["PlotIdentifier"].isin(force_include)]

        if hide_svbcast:
            data = data.loc[~data["PlotIdentifier"].str.contains("_SVBCast") | ~data["Arch"].str.contains("ARM") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_svbcast:    
            data = data.loc[data["PlotIdentifier"].str.contains("_SVBCast") | ~data["PlotIdentifier"].str.contains("_SVE") |  ~data["Arch"].str.contains("ARM") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_s2n:
            data = data.loc[data["PlotIdentifier"].str.contains("_S2N") | data["PlotIdentifier"].str.contains("_SVE") | ~data["Arch"].str.contains("ARM") | data["PlotIdentifier"].isin(force_include)]

        if hide_external:
            data = data.loc[~data["PlotIdentifier"].str.contains("_Ext") | data["PlotIdentifier"].isin(force_include)]

        if hide_soa:
            data = data.loc[~data["PlotIdentifier"].str.contains("SoA") | data["PlotIdentifier"].isin(force_include)]

        if hide_clang:
            data = data.loc[~data["PlotIdentifier"].str.contains("clang") | data["PlotIdentifier"].isin(force_include)]

        if hide_msb_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_MSBLSB") | ~data["PlotIdentifier"].str.contains("_LSBMSB") | data["PlotIdentifier"].isin(force_include)]

        if hide_likely_hints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_Hints") | data["PlotIdentifier"].isin(force_include)]

        if hide_vec_iterators:
            data = data.loc[~data["PlotIdentifier"].str.contains("_VecIt") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_likely_hints:
            data = data.loc[data["PlotIdentifier"].str.contains("_Hints") | data["PlotIdentifier"].isin(force_include)]

        if hide_notestz:   
            data = data.loc[~data["PlotIdentifier"].str.contains("_NOTESTZ") | data["PlotIdentifier"].isin(force_include)]
    
        if hide_lsb_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_LSBLSB") | data["PlotIdentifier"].isin(force_include)]
      
        if hide_lsbmsb_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_LSBMSB") | data["PlotIdentifier"].isin(force_include)]

        if hide_msblsb_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_MSBLSB") | data["PlotIdentifier"].isin(force_include)]
            

        if hide_simd_larger_128:
            data = data.loc[data["PlotIdentifier"].str.contains("_128") | data["PlotIdentifier"].isin(force_include)]

        if hide_s2n:
            data = data.loc[~data["PlotIdentifier"].str.contains("_S2N") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_512_on_intel:
            data = data.loc[data["PlotIdentifier"].str.contains("_512") | ~data["Arch"].str.contains("Intel") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_256_on_amd:
            data = data.loc[data["PlotIdentifier"].str.contains("_256") | ~data["Arch"].str.contains("AMD") | data["PlotIdentifier"].isin(force_include)]

        if hide_sve:
            data = data.loc[data["PlotIdentifier"].str.contains("_128") | ~data["Arch"].str.contains("A64FX") | data["PlotIdentifier"].isin(force_include)]

        if hide_neon:
            data = data.loc[data["PlotIdentifier"].str.contains("_512") | ~data["Arch"].str.contains("A64FX") | data["PlotIdentifier"].isin(force_include)]
            
        if hide_16b_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_16B") | data["PlotIdentifier"].isin(force_include)]

        if hide_8b_fingerprints:
            data = data.loc[~data["PlotIdentifier"].str.contains("_8B") | data["PlotIdentifier"].isin(force_include)]

        if hide_avx512_smaller_512:
            data = data.loc[~data["PlotIdentifier"].str.contains("AVX512") | data["PlotIdentifier"].str.contains("_512") |  ~data["Arch"].str.contains("x86") | data["PlotIdentifier"].isin(force_include)]

        if hide_vfp_16b:
            data = data.loc[~data["PlotIdentifier"].str.contains("FingerPSIMD_16B") | data["PlotIdentifier"].isin(force_include)]

        hash_suffixes = ["_MultShift128", "_MultAddShift64", "_MultAddShift128", "_Murmur", "_XX"]
        if not show_nonmultshift64: 
            data = data.loc[~data["PlotIdentifier"].str.contains('|'.join(hash_suffixes)) | data["PlotIdentifier"].isin(force_include)]

        if hide_multshift64:
            data = data.loc[data["PlotIdentifier"].str.contains('|'.join(hash_suffixes)) | data["PlotIdentifier"].isin(force_include)]
        
        mas_suffixes = ["_MultAddShift64", "_MultAddShift128"]        
        
        if hide_multaddshift:
            data = data.loc[~data["PlotIdentifier"].str.contains('|'.join(mas_suffixes)) | data["PlotIdentifier"].isin(force_include)]

        if hide_msblsb_multshift:
            data = data.loc[~(data["PlotIdentifier"].str.contains("_MSBLSB") & ~data["PlotIdentifier"].str.contains('|'.join(hash_suffixes))) | data["PlotIdentifier"].isin(force_include)]
            
        non_ms_suffixes = ["_Murmur", "_XX"]            
            
        if multshift_only_lsbmsb:
            data = data.loc[data["PlotIdentifier"].str.contains('|'.join(non_ms_suffixes)) | data["PlotIdentifier"].str.contains("_LSBMSB") | data["PlotIdentifier"].isin(force_include)]

        if not show_modulo:
            data = data.loc[~data["PlotIdentifier"].str.contains("_Mod") | data["PlotIdentifier"].isin(force_include)]

        if hide_non_modulo:
            data = data.loc[data["PlotIdentifier"].str.contains("_Mod") | data["PlotIdentifier"].isin(force_include)]

        if not show_dense:
            data = data.loc[~(data["PlotIdentifier"].str.contains("_Dense")) | data["PlotIdentifier"].isin(force_include)]

        if hide_uniform:
            data = data.loc[data["PlotIdentifier"].str.contains("_Dense") | data["PlotIdentifier"].isin(force_include)]

        non_multshifthash_suffixes = ["_MultAddShift64", "_MultAddShift128", "_Murmur", "_XX"]

        if not (plot_speedup or analyze_threadcount):
            data = data.loc[data["ThreadCount"] == thread_count]
            
        if plot_speedup:
            data = data.loc[~data["STSpeedup"].isna()]
            if "SQR" in data.columns:
                data = data.loc[data["SQR"] == force_sqr]
                
        if force_write_lf is not None:
            if "SQR" not in data.columns:
                data = data.loc[data["LoadFactor"] == force_write_lf]      
                
        if analyze_threadcount and "SQR" in data.columns:
            data = data.loc[data["SQR"] == force_sqr]
            
        if hide_xx_hash:
            data = data.loc[~data["PlotIdentifier"].str.contains("_XX") | data["PlotIdentifier"].isin(force_include)]

        for prefix in exclude_prefixes:
            data = data.loc[~data["PlotIdentifier"].str.startswith(prefix) | data["PlotIdentifier"].isin(force_include)]

        onehundred28_suffixes = ["_MultAddShift128", "_MultShift128"]

        if hide_128b_hasher:
            data = data.loc[~data["PlotIdentifier"].str.contains('|'.join(onehundred28_suffixes)) | data["PlotIdentifier"].isin(force_include)]

        funny_hash_suffixes = ["_MultAddShift128", "_MultAddShift64", "_Murmur"]
 
        if hide_multaddshift_murmur:
            data = data.loc[~data["PlotIdentifier"].str.contains('|'.join(funny_hash_suffixes)) | data["PlotIdentifier"].isin(force_include)]

        if not show_stringkeys:
            data = data.loc[~data["PlotIdentifier"].str.contains("_StringKey") | data["PlotIdentifier"].isin(force_include)]
            
        if hide_dummytuple128:
            data = data.loc[~data["PlotIdentifier"].str.contains("_128DummyTuple") | data["PlotIdentifier"].isin(force_include)]
            data = data.loc[~data["PlotIdentifier"].str.contains("_Ptr128DummyTuple") | data["PlotIdentifier"].isin(force_include)]

        if not show_inline_dummytuple:
            data = data.loc[~data["PlotIdentifier"].str.contains("_DummyTuple") | data["PlotIdentifier"].isin(force_include)]

        if not show_pointer_dummytuple:
            data = data.loc[~data["PlotIdentifier"].str.contains("_PtrDummyTuple") | data["PlotIdentifier"].isin(force_include)]

        if not show_intpointer:
            data = data.loc[~data["PlotIdentifier"].str.contains("_PtrUint") | data["PlotIdentifier"].isin(force_include)]

        if only_show_dummy:
            data = data.loc[data["PlotIdentifier"].str.contains("DummyTuple") | data["PlotIdentifier"].isin(force_include)]

        if only_show_dummy_and_intptr:
            data = data.loc[data["PlotIdentifier"].str.contains("DummyTuple") | data["PlotIdentifier"].str.contains("_PtrUint") | data["PlotIdentifier"].isin(force_include)]

        if filter_load_factor_90:
            data = data.loc[data["LoadFactor"] < 90]
            
        if single_row_lf90:
            data = data.loc[data["LoadFactor"] == 90]
        
        if hide_zipf:
            data = data.loc[~data["PlotIdentifier"].str.contains("_Zipf") | data["PlotIdentifier"].isin(force_include)]

        if only_two_sqr:
            if "SQR" in data.columns:
                data = data.loc[(data["SQR"] == 0) | (data["SQR"] == 100)]
        
        if hide_non_kv_budget:
            data = data.loc[~data["PlotIdentifier"].str.contains("Chain") | ~data["PlotIdentifier"].str.contains("BudKV")  | data["PlotIdentifier"].isin(force_include)]

        if not show_fallback:
            data = data.loc[~data["PlotIdentifier"].str.contains("_Fallback") | data["PlotIdentifier"].isin(force_include)]
            
        if hide_256_on_intel:
            data = data.loc[~data["PlotIdentifier"].str.contains("_256") | ~data["Arch"].str.contains("Intel") | data["PlotIdentifier"].isin(force_include)]
        
        if hide_non_zipf:
            data = data.loc[data["PlotIdentifier"].str.contains("_Zipf") | data["PlotIdentifier"].isin(force_include)]
        
        if show_pref:
            add_pref_suffix = [f"_AddPrf{x}" for x in additional_prefetching]
            if len(add_pref_suffix) > 0:
                data = data.loc[data["PlotIdentifier"].str.contains('|'.join(add_pref_suffix)) | ~data["PlotIdentifier"].str.contains("_AddPrf") | data["PlotIdentifier"].isin(force_include)]
            else:
                data = data.loc[~data["PlotIdentifier"].str.contains("_AddPrf") | data["PlotIdentifier"].isin(force_include)]
                
        def filter_words(row):
            identifier = row["PlotIdentifier"]
            for word in word_filter:
                identifier = identifier.replace(word, "")

            return identifier
        
        if len(word_filter) > 0 and len(data) > 0:
            data["PlotIdentifier"] = data.apply(lambda row : filter_words(row.copy()), axis=1)
                        
        return data
    
    read_data = filter_data(read_data)
    write_data = filter_data(write_data)

    if not single_row_lf90:
        if not hide_insertions:
            if len(to_enumerate) == 1:
                axes[0].set_title(f"Insertions", size=title_fontsize)
            else:
                axes[0,0].set_title(f"Insertions", size=title_fontsize)

        for i, lf in enumerate(LOAD_FACTORS):
            if not hide_insertions:
                idx = i + 1
            else:
                idx = i

            if len(to_enumerate) == 1:
                axes[idx].set_title(f"Lookups@LF {lf}", size=title_fontsize)
            else:
                axes[0, idx].set_title(f"Lookups@LF {lf}", size=title_fontsize)
    else:
        for i, arch in enumerate(to_enumerate):
            axes[i].set_title(f"{arch.replace('A64FX ', '')}", size=title_fontsize)


    return_read_data2 = read_data.copy()
    return_write_data2 = write_data.copy()
    
    markers = ["o", "s", "P", "X", "D", "v", "h", ">", "<", "^", "d"]
    marker_dict = dict()
    for i,key in enumerate(list(palette_dict)):
        marker_dict[key] = markers[i]    
    
    filter_str = "PlotIdentifier"        

    for i, arch in enumerate(to_enumerate):
        hue_str = "PlotIdentifier"

        if plot_uniform_and_dense:
            def distri_name_fix(row):
                if "_Dense" in row[hue_str]:
                    return row[hue_str]
                else:
                    return f"{row[hue_str]}_Uniform"

            write_data["DistribPlotIdentifier"] = write_data.apply(lambda row : distri_name_fix(row.copy()), axis=1)
            read_data["DistribPlotIdentifier"] = read_data.apply(lambda row : distri_name_fix(row.copy()), axis=1)
            filter_str = "DistribPlotIdentifier"

        if split_by_simd_size:
            read_arch_data = read_data.loc[read_data[filter_str].str.contains(f"_{arch}") | read_data[filter_str].isin(force_include)]
            write_arch_data = write_data.loc[write_data[filter_str].str.contains(f"_{arch}") | write_data[filter_str].isin(force_include)]
        else:
            if plot_uniform_and_dense:
                read_arch_data = read_data.loc[read_data[filter_str].str.contains(f"_{arch}")]
                write_arch_data = write_data.loc[write_data[filter_str].str.contains(f"_{arch}")]
            else:
                read_arch_data = read_data[read_data["Arch"] == arch]
                write_arch_data = write_data[write_data["Arch"] == arch]   
                    
        if not hide_insertions:
            # Plot Insertions
            if len(to_enumerate) == 1:
                ax = axes[0]
            else:
                ax = axes[i,0]

            insert_data = write_arch_data.copy()

            if plot_uniform_and_dense:
                def distri_name_fix(row):
                    return row[hue_str].replace("_Dense", "")

                insert_data["DPlotIdentifier"] = insert_data.apply(lambda row : distri_name_fix(row.copy()), axis=1)
                hue_str = "DPlotIdentifier"

            if split_by_simd_size:
                def simd_name_fix(row, simd_size):
                    repl1 = row[hue_str].replace(f"_{simd_size}", "")

                    if hide_non_s2n:
                        repl1= repl1.replace("_S2N", "")

                    if hide_non_svbcast:
                        repl1 = repl1.replace("_SVBCast", "")

                    if simd_size == "512" and hide_non_avx512 == False:
                        return repl1.replace("_AVX512", "").replace("_SVE", "")
                    elif simd_size == "512":
                        return repl1.replace("_SVE", "")
                    elif simd_size == "128":
                        return repl1.replace("_NEON", "")
                    else:
                        return repl1

                insert_data["SIMDPlotIdentifier"] = insert_data.apply(lambda row : simd_name_fix(row.copy(), arch), axis=1)
                hue_str = "SIMDPlotIdentifier"


            if len(insert_data) > 0:
                
                if plot_speedup:
                    x_str = "ThreadCount"
                    y_str = "STSpeedup"
                elif analyze_threadcount:
                    x_str = "ThreadCount"
                    y_str = "MInserts/s"
                else:
                    x_str = "LoadFactor"
                    y_str = "MInserts/s"
                    
                p = sns.lineplot(ax=ax, data=insert_data, x=x_str, y=y_str,
                                 hue=hue_str,  hue_order=hue_order, style=hue_str, markers=marker_dict, markevery=markevery, dashes=False, 
                                 linewidth = DATA_LINEWIDTH, palette=palette_dict,
                                 markeredgewidth=MARKER_EDGEWIDTH, markersize=MARKER_SIZE, markeredgecolor=MARKER_EDGECOLOR)
                warnings.filterwarnings("ignore") # mathplotlib spams some kind of weird warning here for whatever reason
                p.set_yticklabels(p.get_yticks(), size = ylabel_fontsize)
                warnings.resetwarnings()

                xticks = [25,50,70,90]
                
                if filter_load_factor_90:
                    xticks = [25,50,70]
                    
                if (plot_speedup or analyze_threadcount):
                    xticks = [1, 2, 4, 8, 16, 32, 64]
                    
                if reduce_xticks and i < len(to_enumerate) - 1:
                    xticks = []

                p.set_xticks(xticks)

                p.set_xticklabels(p.get_xticks(), size = xlabel_fontsize, rotation=LABEL_ROTATION)
                if plot_speedup or analyze_threadcount and len(p.get_xticks()) > 0:
                    p.set_xticklabels([1, None, None, 8, 16, 32, 64], size = xlabel_fontsize, rotation=LABEL_ROTATION)

                if i == len(to_enumerate) - 1:
                    if plot_speedup or analyze_threadcount:
                        p.set_xlabel("Threads", fontsize=xlabel_fontsize, labelpad=xlabel_padding)
                    else:
                        p.set_xlabel("LF", fontsize=xlabel_fontsize, labelpad=xlabel_padding)

                else:
                    p.set_xlabel("", fontsize=xlabel_fontsize, labelpad=xlabel_padding)
                ax.yaxis.set_major_formatter(FuncFormatter(lambda x, _: ('%f' % x).rstrip('0').rstrip('.')))

                ax.get_legend().remove()

                ax.get_yaxis().set_minor_locator(matplotlib.ticker.AutoMinorLocator())
                
                if not reduce_minor:
                    ax.get_yaxis().set_minor_locator(matplotlib.ticker.AutoMinorLocator())
                    ax.grid(axis='y', which='minor', linewidth=MINOR_LINE_THICKNESS)
                else:
                    ax.get_yaxis().set_major_locator(matplotlib.ticker.MaxNLocator(min_n_ticks=3, nbins=4))
                    ax.minorticks_off()
                    
                ax.grid(axis='y', which='major', linewidth=MAJOR_LINE_THICKNESS)

                    
        # Plot Lookups
        for j, lf in enumerate(LOAD_FACTORS):
            if hide_insertions:
                col_idx = j
            else:
                col_idx = j + 1
                              
            if single_row_lf90:
                col_idx = i
            
            lf_data = read_arch_data[read_arch_data["LoadFactor"] == lf].copy()
                        
            if len(to_enumerate) == 1 or single_row_lf90:
                ax = axes[col_idx]
            else:
                ax = axes[i,col_idx]

            if len(lf_data) == 0:
                continue
                
            hue_str = "PlotIdentifier"
            
            if plot_uniform_and_dense:
                def distri_name_fix(row):
                    return row[hue_str].replace("_Dense", "")

                lf_data["DPlotIdentifier"] = lf_data.apply(lambda row : distri_name_fix(row.copy()), axis=1)
                hue_str = "DPlotIdentifier"

            
            if split_by_simd_size:                
                def simd_name_fix(row, simd_size):
                    repl1 = row[hue_str].replace(f"_{simd_size}", "")
                    
                    if hide_non_s2n:
                        repl1= repl1.replace("_S2N", "")
                        
                    if hide_non_svbcast:
                        repl1 = repl1.replace("_SVBCast", "")
                    
                    if simd_size == "512" and hide_non_avx512 == False:
                        return repl1.replace("_AVX512", "").replace("_SVE", "")
                    elif simd_size == "512":
                        return repl1.replace("_SVE", "")
                    elif simd_size == "128":
                        return repl1.replace("_NEON", "")
                    else:
                        return repl1
                
                lf_data["SIMDPlotIdentifier"] = lf_data.apply(lambda row : simd_name_fix(row.copy(), arch), axis=1)
                hue_str = "SIMDPlotIdentifier"
                            
            
            if plot_speedup:
                x_str = "ThreadCount"
                y_str = "STSpeedup"
            elif analyze_threadcount:
                x_str = "ThreadCount"
                y_str = "MLookups/s"
            else:
                x_str = "SQR"
                y_str = "MLookups/s"
                            
            p = sns.lineplot(ax=ax, data=lf_data, x=x_str, y=y_str, hue=hue_str, hue_order=hue_order, style=hue_str, markers=marker_dict, markevery=markevery, dashes=False, linewidth = DATA_LINEWIDTH, palette=palette_dict, markeredgewidth=MARKER_EDGEWIDTH, markersize=MARKER_SIZE, markeredgecolor=MARKER_EDGECOLOR)
            warnings.filterwarnings("ignore") # mathplotlib spams some kind of weird warning here for whatever reason

            if reduce_yticks and not single_row_lf90 and (not hide_insertions and (insert_data is not None or len(insert_data) > 0 or j > 0)):
                ax.tick_params(axis='y', which="both", length=0, width=0)

            if single_row_lf90 and i > 0:
                ax.tick_params(axis='y', which="both", length=0, width=0)
            
            p.set_yticklabels(p.get_yticks(), size = ylabel_fontsize)
            warnings.resetwarnings()
            
            if reduce_xticks:
                if i == len(to_enumerate) - 1 or single_row_lf90:
                    if not (plot_speedup or analyze_threadcount):
                        p.set_xticks([0, 50, 100])
                    else:
                        p.set_xticks([1, 2, 4, 8, 16, 32, 64])
                else:
                    p.set_xticks([])
            else:
                if not (plot_speedup or analyze_threadcount):
                    p.set_xticks([0, 50, 100])
                else:
                    p.set_xticks([1, 2, 4, 8, 16, 32, 64])
                
            p.set_xticklabels(p.get_xticks(), size = xlabel_fontsize, rotation=LABEL_ROTATION)
            
            if plot_speedup or analyze_threadcount and len(p.get_xticks()) > 0:
                p.set_xticklabels([1, None, None, 8, 16, 32, 64], size = xlabel_fontsize, rotation=LABEL_ROTATION)
            
            if i == len(to_enumerate) - 1 or single_row_lf90:
                if plot_speedup or analyze_threadcount:
                    p.set_xlabel("Threads", fontsize=xlabel_fontsize, labelpad=xlabel_padding)
                else:
                    p.set_xlabel("SQR", fontsize=xlabel_fontsize, labelpad=xlabel_padding)
            else:
                p.set_xlabel("", fontsize=xlabel_fontsize, labelpad=xlabel_padding)
    
            ax.set_ylim(bottom=0,top=arch_ylimits.get(arch, None))
            
            ax.yaxis.set_major_formatter(FuncFormatter(lambda x, _: ('%f' % x).rstrip('0').rstrip('.')))
            
            
            if not reduce_minor:
                ax.get_yaxis().set_minor_locator(matplotlib.ticker.AutoMinorLocator())
                ax.grid(axis='y', which='minor', linewidth=MINOR_LINE_THICKNESS)      
            else:
                n_bins = 3 if other_ylimit else 4
                min_n_ticks = 2 if other_ylimit else 3
                
                if power_axis and i == len(to_enumerate) - 1:
                    n_bins = 3
                    min_n_ticks = 2
                
                ax.get_yaxis().set_major_locator(matplotlib.ticker.MaxNLocator(min_n_ticks=min_n_ticks, nbins=n_bins))
                ax.minorticks_off()
                
            ax.grid(axis='y', which='major', linewidth=MAJOR_LINE_THICKNESS)

                
            ax.get_legend().remove()

    for i, arch in enumerate(to_enumerate):
        for j, lf in enumerate(LOAD_FACTORS):           
            if split_by_simd_size:
                if len(simd_sizes) == 1:
                    axes[j].set_ylabel(f"{ARCHITECTURES[0].replace('ARM','')} - {arch}\nM Ops/s"  if j == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
                else:
                    axes[i, j].set_ylabel(f"{ARCHITECTURES[0].replace('ARM','')} - {arch}\nM Ops/s"  if j == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
            else:
                if plot_uniform_and_dense:
                    axes[i, j].set_ylabel(f"{arch}\nM Ops/s" if j == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
                else:
                    if single_row_lf90:
                        axes[i].set_ylabel("M Ops/s" if i == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
                    else:
                        if len(ARCHITECTURES) == 1:
                            axes[j].set_ylabel(f"{arch.replace('A64FX ', '') }\nM Ops/s" if j == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
                        else:
                            axes[i, j].set_ylabel(f"{arch.replace('A64FX ', '') }\nM Ops/s"  if j == 0 else "", fontsize=ylabel_fontsize, labelpad=ylabel_padding)
    
    handles, labels = None, None
    
    all_labels = []
    
    if collect_all_labels:
        handles = []
        labels = []
        for ax in axes.flat:
            _handles, _labels = ax.get_legend_handles_labels()
            all_labels += _labels
            
            for idx,label in enumerate(_labels):
                if label not in labels:
                    labels.append(label)
                    handles.append(_handles[idx])
                
        labels, handles = zip(*sorted(zip(labels, handles), key=lambda t: t[0]))
        
    else:
        for ax in axes.flat:
            _handles, _labels = ax.get_legend_handles_labels()
            all_labels += _labels
            if labels is None or len(labels) < len(_labels):
                handles = _handles
                labels = _labels

    if len(LOAD_FACTORS)  > 2:
        bbox = (0,-0.01 + legend_height_shift,1,1)
    else:
        if ncol_legend > 3:
            bbox = (0,-0.03 + legend_height_shift,1,1)
        else:
            bbox = (0,-0.08 + legend_height_shift,1,1)
        
    transformed_labels = ()
    for label in labels:
        if label in legend_label_dict:
            transformed_labels = transformed_labels + (legend_label_dict[label],)
        else:
            label = label.replace("_", "-") # ensure latex can handle this
            transformed_labels = transformed_labels + (label,)
       
    if label_order is not None:
        handles = [handles[idx] for idx in label_order]
        transformed_labels = [transformed_labels[idx] for idx in label_order]
    
    
    legend = fig.legend(handles, transformed_labels, frameon=not disable_legend_border, loc='lower center', handlelength=LEGEND_HANDLELENGTH + extra_legend_handlelength, columnspacing=LEGEND_COLUMNSPACING + extra_legend_columnspacing, handletextpad=LEGEND_HANDLETEXTPAD, ncol = ncol_legend, bbox_to_anchor = bbox, bbox_transform = fig.transFigure, fontsize=legend_fontsize, title_fontsize=legend_fontsize, markerscale=0.5)
    
    # set the linewidth of each legend object
    for legobj in legend.legendHandles:
        legobj.set_linewidth(DATA_LINEWIDTH)
        legobj.set_markersize(MARKER_SIZE)
        legobj.set_markeredgewidth(MARKER_EDGEWIDTH)
        legobj.set_markeredgecolor(MARKER_EDGECOLOR)
        
    if file_path is not None:
        if "pdf" in file_path:
            plt.savefig(file_path, bbox_inches='tight', backend='pgf')
        else:
            plt.savefig(file_path, bbox_inches='tight')

  
    plt.show()
    
    return return_read_data2, return_write_data2


### FIGURE 1 ###

pal_dict = {'BucketSIMD_8B_THP': "#fd8d3c",
            'RecalcRH_UN_THP':  "#2c7fb8",
            }
_ = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86", "AMD x86", "A64FX ARM", "Power"], [90], 250,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True,
                   hide_non_avx512=True, hide_lsbmsb_fingerprints=False,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=[],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_16b_fingerprints=True, hide_vfp_16b=True,
                   force_include=["RecalcRH_UN_THP"],
                   legend_label_dict={"LP_UN_THP": "LP(AoS)", "RecalcRH_UN_THP": "Robin Hood", "BucketSIMD_8B_THP": "Bucket-Based Comparison"},
                   file_path="figure1.pdf", ncol_legend=2,
                   legend_height_shift=-0.8, ylabel_padding=0, xlabel_padding=0, extra_legend_columnspacing=0.5, extra_legend_handlelength=1,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True, single_row_lf90=True)

### FIGURE 4 ###

compare_pal = {"LP_UN_THP": "#a1dab4", "RecalcRH_UN_THP": "#2c7fb8", "Chain_BudKV_THP": "#253494", "LPPackedSoA_THP": "#41b6c4"}
_ = analysis_plot("", compare_pal, ["Intel x86", "AMD x86", "A64FX ARM", "Power"],  [50, 90], 240, ncol_legend=4, legend_height_shift=-0.05, ylabel_padding=0, xlabel_padding=0,
                  show_thp=False, show_pref=False, show_unroll=False, extra_legend_columnspacing=0.2, extra_legend_handlelength=0.7,
                  exclude_prefixes=['StoreRH', 'LP_AP', 'QP_UN', 'QP_NA', 'LP_NA', 'RecalcRH_AP', 'LP_UN', 'RecalcRH_NA', 
                                    'RecalcRH_UN', 'QP_AP', 'UnchkdSIMD_NOTESTZ_AVX512', 'UnchkdSIMD_TESTZ', 'UnchkdSIMD_NOTESTZ', 'UnchkdSIMD_TESTZ_AVX512', 'FingerPSIMD'],
                  force_include=["RecalcRH_UN_THP",   "LP_UN_THP", "LPPackedSoA_THP", "Chain_BudKV_THP"],
                  legend_label_dict={"RecalcRH_UN_THP": "RecalcRH", "StoreRH_THP": "StoringRH", "QP_UN_THP": "QP(Unal.)", "LP_UN_THP": "LP(AoS)", "LPPackedSoA_THP": "LP(SoA)", "Chain_BudKV_THP": "Chained"},
                   file_path="figure4.pdf", label_order=[2,1,3,0], disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True,
                   hue_order=(["Chain_BudKV_THP","RecalcRH_UN_THP", "LP_UN_THP", "LPPackedSoA_THP" ]))

### FIGURE 5 ###

pal_dict = {'LPPackedSoA_THP': "#a1dab4", 'UnchkdSIMD_TESTZ_128_THP': "#2c7fb8", 'UnchkdSIMD_TESTZ_256_THP': "#253494",
            'UnchkdSIMD_TESTZ_512_THP': "#41b6c4"
           }
_ = analysis_plot("UnchunkedSIMDSoAHashTable", pal_dict, ["Intel x86", "AMD x86", "A64FX ARM"], [50, 90], 240,  ncol_legend=5, legend_height_shift=-0.1, ylabel_padding=0, xlabel_padding=0,
           show_thp=True, hide_non_thp=True, show_pref=False, show_unroll=False,
           hide_non_s2n=False, hide_s2n=True, hide_svbcast=True,
           hide_external=True, hide_non_avx512=True, show_avx512=True,
           force_include=["LPPackedSoA_THP"],
           exclude_prefixes=["UnchkdSIMD_NOTESTZ", "UnchkdSIMD_MOM", "UnchkdSIMD_TESTZ_128_NEON_A64MM"],
           word_filter=["_SVE", "_AVX512", "_NEON_UMINV"],
           legend_label_dict={'UnchkdSIMD_TESTZ_128_THP': "128", 
                              'UnchkdSIMD_TESTZ_256_THP': "256", 
                              'UnchkdSIMD_TESTZ_512_THP': "512", 
                              'LP_UN_THP':  'LP(AoS)', 'LPPackedSoA_THP': 'LP(SoA)'},
           file_path="figure5.pdf", disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True,
                 extra_legend_columnspacing=0.5, extra_legend_handlelength=1)

### FIGURE 6 ###

pal_dict = { 'FingerPSIMD_8B_TESTZ_256_THP': "#2c7fb8", 'FingerPSIMD_8B_TESTZ_512_THP': "#253494", 
             'FingerPSIMD_16B_TESTZ_256_THP': "#fd8d3c", 'FingerPSIMD_16B_TESTZ_512_THP': "#e31a1c",
             'FingerPSIMD_16B_TESTZ_128_THP': "#fecc5c", 'FingerPSIMD_8B_TESTZ_128_THP': "#41b6c4" }

_  = analysis_plot("FingerprintingSIMDSoAHashTable", pal_dict, ["Intel x86", "A64FX ARM"], [50, 90], 240, 
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True, 
                   hide_non_avx512=True, hide_lsbmsb_fingerprints=False, hide_likely_hints=False,
                   hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, 
                   hide_non_s2n=False, exclude_prefixes=["FingerPSIMD_8B_NOTEST", "FingerPSIMD_16B_NOTEST", 'FingerPSIMD_16B_LSBMSB_NOTESTZ'],
                   legend_label_dict={'FingerPSIMD_8B_TESTZ_256_THP': "8/256", 'FingerPSIMD_8B_TESTZ_512_THP': "8/512", 
                                     'FingerPSIMD_16B_TESTZ_256_THP': "16/256", 'FingerPSIMD_16B_TESTZ_512_THP': "16/512",
                                     'FingerPSIMD_16B_TESTZ_128_THP': "16/128", 'FingerPSIMD_8B_TESTZ_128_THP': "8/128"}, 
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB"],
                   file_path="figure6.pdf", ncol_legend=6, label_order=[3,0,4,1,5,2],
                  legend_height_shift=-0.25, ylabel_padding=0.5, xlabel_padding=0.5,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True
                  )

### FIGURE 7 ###

pal_dict = { 'BucketSIMD_8B_TESTZ_512_THP': "#253494", 
             'BucketSIMD_16B_TESTZ_512_THP': "#e31a1c",
             'BucketSIMD_16B_TESTZ_128_THP': "#fecc5c", 'BucketSIMD_8B_TESTZ_128_THP': "#41b6c4" }


_  = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86", "A64FX ARM"], [50, 90], 240,   
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_lsbmsb_fingerprints=False,  hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True, hide_non_avx512=True,
                   hide_non_s2n=False, exclude_prefixes=[], hide_256_on_intel=True,
                   legend_label_dict={'BucketSIMD_8B_TESTZ_512_THP': "8/512", 
                                     'BucketSIMD_16B_TESTZ_512_THP': "16/512",
                                     'BucketSIMD_16B_TESTZ_128_THP': "16/128", 'BucketSIMD_8B_TESTZ_128_THP': "8/128"}, 
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   file_path="figure7.pdf", ncol_legend=4,
                  label_order=[2,3,0,1],
                  legend_height_shift=-0.25, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True,
                  extra_legend_columnspacing=0.5, extra_legend_handlelength=1,)

### FIGURE 8 ###

pal_dict = {"BucketSIMD_16B_THP": "#e31a1c", 'BucketSIMD_8B_THP': "#fd8d3c",
            'FingerPSIMD_8B_THP': "#fecc5c", 'RecalcRH_UN_THP':  "#2c7fb8",
            'LP_UN_THP':  "#a1dab4", 'Chain_BudBucket16FP_THP': "#253494"}

_ = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86", "AMD x86", "A64FX ARM", "Power"], [50, 90], 240,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True,
                   hide_non_avx512=True, hide_lsbmsb_fingerprints=False,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=["FingerprintingSIMDSoAHashTable"],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_16b_fingerprints=False, hide_vfp_16b=True,
                   force_include=["LP_UN_THP", "Chain_BudBucket16FP_THP", "RecalcRH_UN_THP"],
                   legend_label_dict={"LP_UN_THP": "LP", "RecalcRH_UN_THP": "RH", "FingerPSIMD_8B_THP": "VFP(8)", "Chain_BudBucket16FP_THP": "Chain(16)", "BucketSIMD_8B_THP": "BBC(8)", "BucketSIMD_16B_THP": "BBC(16)"},
                   file_path="figure8.pdf", ncol_legend=6,
                   label_order=[4,5,2,3,1,0],
                   legend_height_shift=-0.05, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True, power_axis=True)

### FIGURE 9 ###

pal_dict = {'BucketSIMD_8B_MSBLSB_THP_XX': "#fd8d3c", 'LP_UN_THP_XX': "#a1dab4", 
            'BucketSIMD_8B_MSBLSB_THP_XX_StringKey': "#e31a1c", "LP_UN_THP_XX_StringKey": "#2c7fb8" }

_  = analysis_plot("BucketingSIMDHashTable", pal_dict, ["AMD x86", "Power"],  [50, 90], 240, ncol_legend=4,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_msblsb_fingerprints=False, hide_lsbmsb_fingerprints=True, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=[],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_MSBFP", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_8b_fingerprints=False, hide_16b_fingerprints=True,
                   force_include=["LP_UN_THP_XX", "LP_UN_THP_XX_StringKey"],
                   show_nonmultshift64 = True, hide_128b_hasher=True, hide_xx_hash=False, hide_multaddshift_murmur=True, hide_multshift64=True,
                   show_stringkeys=True,
                   legend_label_dict={"LP_UN_THP_XX_StringKey": "LP+Str", "LP_UN_THP_XX": "LP+Int",
                                      "BucketSIMD_8B_MSBLSB_THP_XX_StringKey": "BBC(8)+Str", "BucketSIMD_8B_MSBLSB_THP_XX": "BBC(8)+Int"},
                   file_path="figure9.pdf",
                  legend_height_shift=-0.22, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True, other_ylimit=True, only_two_sqr=False,
                  extra_legend_columnspacing=0.1, extra_legend_handlelength=0.3)

### FIGURE 10 ###

pal_dict = {'BucketSIMD_8B_THP_DummyTuple': "#fd8d3c",
            'LP_UN_THP_DummyTuple': "#a1dab4",
            'Chain_BudBucket8FP_THP_DummyTuple': "#d7b5d8", 
            'BucketSIMD_8B_THP_PtrDummyTuple': "#e31a1c", 
            "LP_UN_THP_PtrDummyTuple": "#41b6c4",
            'Chain_BudBucket8FP_THP_PtrDummyTuple': "#df65b0"}

read_d, write_d  = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86", "A64FX ARM"], [50, 90], 240,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True, 
                   hide_msb_fingerprints=False, hide_msblsb_fingerprints=True, hide_lsbmsb_fingerprints=False, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True,
                   hide_lsb_fingerprints=True, hide_notestz=True, hide_non_avx512=True, ncol_legend=6,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=[],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_8b_fingerprints=False, hide_16b_fingerprints=True, hide_vfp_16b=True,
                   force_include=["LP_UN_THP_PtrDummyTuple", 'LP_UN_THP_DummyTuple', 'Chain_BudBucket8FP_THP_DummyTuple', 'Chain_BudBucket8FP_THP_PtrDummyTuple'],
                   show_inline_dummytuple=True, show_pointer_dummytuple=True, only_show_dummy=True,
                   legend_label_dict={"BucketSIMD_8B_THP_DummyTuple": "BBC+Inl", "LP_UN_THP_DummyTuple": "LP+Inl",
                                      "Chain_BudBucket8FP_THP_DummyTuple": "CH+Inl", "BucketSIMD_8B_THP_PtrDummyTuple": "BBC+Ptr",
                                      "LP_UN_THP_PtrDummyTuple": "LP+Ptr", "Chain_BudBucket8FP_THP_PtrDummyTuple": "CH+Ptr"},
                   file_path="figure10.pdf", legend_height_shift=-0.23, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True)

### FIGURE 11 ###

pal_dict = {"BucketSIMD_8B_THP_Zipf": "#e31a1c", 'BucketSIMD_8B_THP': "#fd8d3c",
            'FingerPSIMD_8B_THP': "#d7b5d8", 'FingerPSIMD_8B_THP_Zipf':  "#41b6c4",
            'LP_UN_THP':  "#a1dab4", 'LP_UN_THP_Zipf': "#253494"}

_ = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86"], [50, 70, 90], 240,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True,
                   hide_non_avx512=True, hide_lsbmsb_fingerprints=False,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=["FingerprintingSIMDSoAHashTable"],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_16b_fingerprints=True, hide_vfp_16b=True,
                   force_include=["LP_UN_THP_Zipf"],
                   legend_label_dict={"LP_UN_THP_Zipf": "LP(AoS)", "FingerPSIMD_8B_THP_Zipf": "VFP(8)", "BucketSIMD_8B_THP_Zipf": "BBC(8)"},
                   file_path="figure11.pdf", ncol_legend=3,
                   legend_height_shift=-0.6, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True,
                   hide_zipf=False, hide_non_zipf=True, hide_insertions=True, label_order=[2,1,0],
                 extra_legend_columnspacing=0.5, extra_legend_handlelength=1,)

### FIGURE 12 ###

pal_dict = {"BucketSIMD_16B_THP": "#e31a1c", 'BucketSIMD_8B_THP': "#fd8d3c",
            'FingerPSIMD_8B_THP': "#fecc5c", 'RecalcRH_UN_THP':  "#2c7fb8",
            'LP_UN_THP':  "#a1dab4", 'Chain_BudBucket16FP_THP': "#253494"}

_ = analysis_plot("BucketingSIMDHashTable", pal_dict, ["Intel x86", "AMD x86", "A64FX ARM", "Power"], [50, 90], 240,
                   show_thp=True, show_pref=False, show_unroll=False, hide_non_thp=True,  hide_non_likely_hints=True,
                   hide_msb_fingerprints=False, hide_likely_hints=False, hide_vec_iterators=True, hide_svbcast=True, hide_lsb_fingerprints=True, hide_notestz=True,
                   hide_non_avx512=True, hide_lsbmsb_fingerprints=False,
                   hide_non_s2n=False, exclude_prefixes=[], additional_include_filter=["FingerprintingSIMDSoAHashTable"],
                   word_filter=["_NEON_S2N", "_AVX512", "_SVE", "_Hints", "_LSBMSB", "_TESTZ", "_512", "_128", "_256", "_KVBu", "_64FPPB",  "_32FPPB", "_16FPPB", "_8FPPB"],
                   hide_non_512_on_intel=True, hide_non_256_on_amd = True, hide_neon=True, hide_16b_fingerprints=True, hide_vfp_16b=True,
                   force_include=["LP_UN_THP", "RecalcRH_UN_THP"],
                   legend_label_dict={"LP_UN_THP": "LP(AoS)", "RecalcRH_UN_THP": "RecalcRH", "FingerPSIMD_8B_THP": "VFP(8)", "Chain_BudBucket16FP_THP": "Chained(16)", "BucketSIMD_8B_THP": "BBC(8)", "BucketSIMD_16B_THP": "BBC(16)"},
                   file_path="figure12.pdf", ncol_legend=4,
                   label_order=[2,3,1,0],
                   legend_height_shift=-0.045, ylabel_padding=0, xlabel_padding=0,
                   disable_legend_border=True, reduce_xticks=True, reduce_yticks=True, reduce_minor=True,
                   analyze_threadcount=True,  force_write_lf=90, force_sqr=50, hide_insertions=False, arch_ylimits={"Power": 500},
                   markevery=[0,3,4,5,6], extra_legend_columnspacing=0.3, extra_legend_handlelength=0.7)