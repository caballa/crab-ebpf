# PREVAIL 

**Important**: this repo is based on https://github.com/vbpf/ebpf-verifier but it keeps Crab as an external dependency so that it is still straightforward to verify eBPF programs with new Crab features and domains.

## a Polynomial-Runtime EBPF Verifier using an Abstract Interpretation Layer
A new eBPF verifier.

## Getting Started

### Dependencies from vanilla Ubuntu
```bash
sudo apt install build-essential git cmake libboost-dev libgmp-dev libmpfr-dev g++-8
sudo apt install python3-pip python3-tk
pip3 install matplotlib   # for plotting the graphs
```

### Installation
Clone, make and run:
```
git clone --recurse-submodules https://github.com/vbpf/ebpf-verifier.git
cd ebpf-verifier
make crab_install
make
```

### Running with Docker
Build and run:
```bash
docker build -t verifier .
docker run -it verifier ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=zoneCrab
1,0.062802,21792
# To run the Linux verifier you'll need a privileged container:
docker run --privileged -it verifier ebpf-samples/linux/cpustat_kern.o --domain=linux
```

### 

Example:
```
ebpf-verifier$ ./check ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=zoneCrab
1,0.062802,21792
```
The output is three comma-separated values:
* 1 or 0, for "pass" and "fail" respectively
* The runtime of the fixpoint algorithm (in seconds)
* The peak memory consumption, in kb, as reflected by the resident-set size (rss)

Usage:
```
ebpf-verifier$ ./check -h
A new eBPF verifier
Usage: ./check [OPTIONS] path [section]

Positionals:
  path FILE REQUIRED          Elf file to analyze
  section SECTION             Section to analyze

Options:
  -h,--help                   Print this help message and exit
  -l                          List sections
  -d,--dom,--domain DOMAIN    Abstract domain
  -i                          Print invariants
  -f                          Print verifier's failure logs
  -v                          Print both invariants and failures
  --asm FILE                  Print disassembly to FILE
  --dot FILE                  Export cfg to dot FILE
```

A standard alternative to the --asm flag is `llvm-objdump -S FILE`.

The cfg can be viewed using `dot` and the standard PDF viewer:
```
./check ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=zoneCrab --dot cfg.dot
dot -Tpdf cfg.dot > cfg.pdf
```

## Step-by-Step Instructions

To get the results for described in Figures 9 and 10, run the following:
```
scripts/runperf.sh ebpf-samples stats interval zoneCrab zoneElina octElina polyElina | tee results.csv
```
The first argument to the script, `ebpf-samples`, is the root directory in which
to search for elf files. You can pass any subdirectory or file, e.g.
`ebpd-samples/linux`.

The rest of the positional arguments are the numerical domains to use.

The output is a large `csv` file. The first line is a header:
```
suite,project,file,section,hash,instructions,loads,stores,jumps,joins,interval?,interval_sec,interval_kb,zoneCrab?,zoneCrab_sec,zoneCrab_kb,zoneElina?,zoneElina_sec,zoneElina_kb,octElina?,octElina_sec,octElina_kb,polyElina?,polyElina_sec,polyElina_kb
ebpf-samples,cilium,bpf_lxc.o,2/1,69a5e4fc57ca1c94,41,6,10,1,1,1,0.047696,8484,1,0.057409,21796,1,0.0948671,19192,1,0.100129,24196,1,0.12144,18732
```
* _suite_ in our case will be "ebpf-samples"
* _project_ is one of the directories in the suite. We currently have `bpf_cilium_test`, `cilium` `linux`, `ovs`,  `prototype-kernel` and  `suricata`
* _file_ is the elf file containing the programs. The compiled version of a C file
* _section_ is the elf section containing the program checked
* _hash_ is a unique hash of the eBPF code. There are duplicate programs in the benchmark (since we use files from projects "as-is"). To count the real number of programs these duplicates should be removed
* _instructions_, _loads_, _stores_, _jumps_ and _joins_ show the number of these features
* For each domain DOM, there are 3 consecutive columns:
    * "DOM?" is 0 for rejected program, 1 for accepted program
    * "DOM_sec" is the number of seconds that the fixpoint operation took
    * "DOM_kb" is the peak memory resident set size consumed by the analysis, and is an estimate for the amount of additional memory needed by the analysis  

Note that in the full benchmark, exactly 2 programs should be rejected by `zoneCrab`, our domain of choice. Other domain reject different number of programs.

Any subset of the available domains is valid. So in order to compare the two different
implementations of the `zone` domain, one can run
```
scripts/runperf.sh ebpf-samples/linux stats zoneCrab zoneElina | results.csv
python3 scripts/makeplot.py results.csv stores
```
The script `scripts/makeplot.py` takes a csv file in the format described above, and the key to plot against (usually instructions or stores) and plots two graphs: on showing runtime as a function of the number of stores, and the other is the memory consumption as a function of the number of stores.

While the paper states that the runtime is quadratic, the results are
expected to be nearly linear for all the domains - except probably the domain
`octElina` which does not show consistent runtime characteristics.

Note that the number of programs is slightly different from the numbers presented in Section 7.1 (benchmarks). The precise numbers depend on how one count duplicate programs (as can be found using the hash column in the resulting csv file), the addition of a new repository (prototype-kernel) and removal of variation on existing repository (ovs-noprint). Also, we did not count programs smaller than certain size.

### Caveat
When performed on a VM without sufficient memory, some analyses of some domains
are terminated by the OS due to insufficient memory, resulting in "-1" runtime
and skewing the graph. To avoid this, the failing cases should be omitted.

4GB RAM should be enough for `zoneCrab`, our domain of choice, but other domains
may require much more than that. To reproduce the results as will be published
in the final version, it is recommended to use bare-metal Linux machine. 

## Testing the Linux verifier

To run the Linux verifier, you must use `sudo`:
```
sudo ./check ebpf-samples/linux/cpustat_kern.o --domain=linux
```

## Counter and Artificial examples

The folder `counter/` contains other examples used to demonstrate the usefulness of our tools, compared to the existing verifier. To compile the examples, run 
```
make -C counter
scripts/runperf.sh counter/objects stats zoneCrab
```

Two examples of real-world false positive are taken from the Linux samples suite.
The file `xdp_tx_iptunnel_kern.o` is valid and passes both the Linux tool and ours.
However, in the original source code there are redundant loads from memory to a varaible holding the same value. These were added happen due to untracked register spilling that led to false positive. Two fixes are compiled into `xdp_tx_iptunnel_1_kern.o` and `xdp_tx_iptunnel_2_kern.o`. Both pass our verifier (without any special effort) but fail the existing one:
```
$ ./check counter/objects/xdp_tx_iptunnel_2_kern.o
1,0.314213,86740
$ sudo ./check counter/objects/xdp_tx_iptunnel_2_kern.o --domain=linux -v
<... long trace reporting an alleged failure>
```

### Double-strncmp experiment
This experiment quadratic blowup in the Linux verifier, versus linear runtime in our tool.
Be sure to run with `sudo`, since Linux requires special permissions for this.
```
sudo scripts/experiment.sh | tee blowup.csv
python3 scripts/makeplot.py blowup.csv iterations False
```

### Programs with loops
There are several simple programs with loops in the folder `counter/src`, called `simple_loop_*.c` and `manual_memset*.c`. The Linux verifier rejects them immediately:
```
$ sudo ./check counter/objects/simple_loop_ptr_backwards.o --domain=linux -v
counter/objects/simple_loop_ptr_backwards.o
	sk_skb/loop-ptr,bpf_load_program(prog_cnt=0) err=22
back-edge from insn 7 to 5
```

Using our tool, the safety (but not termination) of some loop-based programs can be verified:
```
$ ./check counter/objects/simple_loop_ptr_backwards.o
1,0.018346,7900
```
(not all the programs in the folder are verified)

### Important components:

The analyzer code is divided to two main parts: front end, parsing eBPF binaries into CFG, and backend, translating the eBPF CFG into crab-specific CFG of constraints.

The front end (files named `asm_*`) is potentially reusable by any other analyzer or tool for eBPF. The most important file is `src/asm_syntax.hpp`, which describes the syntax of the language in a relatively self-explanatory way (structs and variant types).

The backend is mostly confined into `src/crab_verifier.cpp` and `src/crab_constraints.cpp`. The latter does the translation of eBPF instructions to the language handled by crab.

### Apron

The experiments in the paper do not include the APRON library. The tool supports
two domains: `octApron` and `polyApron`. However, they are mutually exclusive
with elina. In order to run the tool with these domains, Crab should be
reinstalled, and the variable `MOD` should be set:

```
make crab_clean clean
make MOD=APRON crab_install
make

$ ./check ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=polyApron
1,1.37877,0
$ ./check ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=octApron
1,0.200318,0
$ ./check ebpf-samples/cilium/bpf_lxc.o 2/1 --domain=octElina
Could not convert: --dom = octElina
Run with --help for more information.
```
To recompile with Elina support, reinstall crab without `MOD` (or with `MOD=ELINA`):
```
make crab_clean clean
make MOD=APRON crab_install
make
```
