# Reproducibility Guide

In this guide, we explain how to regenerate the figures from our VLDB paper.
Since the results depend on the hardware and toolchain, in order to get the exact same results, the experiments need to be run on the same machines.
For the VLDB reproducibility process, we provide access to HPI computing infrastructure.
Please reach out to us in order to set up your HPI account.
Nevertheless, the code can be run on any system.
To use gcc 11.2 and CMake 3.21 from source, we refer to [this repository](https://github.com/MaxiBoether/gcc-cmake-clang-from-source).
On the HPI infrastructure, we provide all prerequisites.

## Accessing HPI Infrastructure

Assuming you have an HPI guest and DELAB account, we first need to setup access to HPI infrastructure.
For this, first connect to HPI VPN as outlined in the [HPI Wiki](https://confluence.hpi.de/display/ITAdmin/Nutzung+VPN?showLanguage=en_GB).
Viewing this wiki page also requires logging in using your HPI credentials.
After setting up the VPN connection, you should be able to login to the login node via `username@delab.i.hpi.de`.
We will run experiments on one machine from each of the four categories.
Here, we list one machine for each architecture that you can use to re-generate the benchmarking data.

- Intel x86: cx06.delab.i.hpi.de
- AMD x86: cx27.delab.i.hpi.de
- Power: fp02.delab.i.hpi.de
- ARM: ca03.delab.i.hpi.de

On these machines, the gcc/cmake toolchain is provided.
In the following, each of the steps needs to be run on all machines.

In order to manage connections, we recommend to either start one screen session per ssh connection using `screen -S intel|amd|power|arm` or use `tmux` to have one pane per connection.
To connect to the x86 nodes, you can use the following command from the login node:
```bash
srun -A SOME_SLURM_GROUP --time=5-00:00:00 -p magic -w cx06|cx27 --mem=70G --cpus-per-task=72 --exclusive --pty bash
```

For ARM, you can use the following command:
```bash
srun -A SOME_SLURM_GROUP --time=5-00:00:00 -p magic -w ca03 --mem=29G --cpus-per-task=30 --exclusive --pty bash
```

For Power, you can use the following command:
```bash
srun -A SOME_SLURM_GROUP --time=5-00:00:00 -p magic -w fp02 --mem=200G --cpus-per-task=96 --exclusive --pty bash
```

In case you do not know your slurm group, please reach out to us. 
Note that since this is shared infrastructure, but we are requesting exclusive benchmarking access, you might not be allocated space on a node immediately.
Hence, we recommend to start this in a screen session and check back later if you did not login immediately.


## Setup and generating the query data and running the benchmarks

On each machine, we operate on scratch SSD storage.
Navigate to `/scratch`, create a directory like `mkdir reproducibility`, and `cd reproducibility` into it.
Then, you need to clone this repository using `git clone https://github.com/hpides/vectorized-hash-tables.git`.

First, we need to generate the data containing keys and values to be inserted and queried from the hashmaps, and then execute the according benchmarks.
To this end, after connecting to the machine and starting a tmux session on the machine, run the `reproducibility/run_bms.sh` script in the repository to compile the source code for all experiments, generate data, and run the benchmarks.

Note that this can take a longer period of time.
Next, we need to merge all results and transfer them to our main machine.
We provide a supporting script for this.
For each machine, pleaes run the `reproducibility/collect_results.sh` script.
It will collect and upload the data and print out two links from which you should download the files to your local computer.

## Generating the Figures
The figures are generated on your local machine.
For this, you also need to clone the repository and move the downloaded files from the previous step into the `reproducibility` subdirectory.

We assume that you use mamba or conda to manage Python environments.
Navigate into the `reproducibility` subdirectory.
Then, setup the environment using `mamba env create -f ./environment.yml`.
Activate the environment using `mamba activate hashmaps`.
Then, to generate all plots, run `python gen_plots.py`, and all figures will be generated from the supplied data.