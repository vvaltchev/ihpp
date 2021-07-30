
# IHPP: An Intraprocedural Hot Path Profiler
This is my [B.S. thesis] project written during the accademic year 2011/2012 with minor
recent updates (Jul 2021) to make it work with recent C++ compilers and the newest
version of Intel PIN (3.20) on Linux. The original work is tagged as `bs_thesis_final`
in git. <br />

*Remarks*: the code style is not great, but it's an old project written during the
college years and untouched since then. Help from any volunteers interested in
refactoring and further developing the project will be appreciated.

## Abstract
IHPP is an innovative instrumentation profiler written based on the Intel Pin framework.
It allows arbitrary context-sensitive profiling at three different levels: at procedure
level, at basic blocks inside procedures level and at basic blocks among procedures level.
This is achieved using two novel data structures called **k-slab** forest and
**k-calling context forest** created by four professors at the [Sapienza University of Rome].
The idea on which are based these data structures is to generalize the concepts of vertex
profiling, edge profiling and full context-sensitive profiling with the introduction of
a **k** parameter which allows profiling in all intermediate points between vertex (k = 0)
and full (k = ∞) context-sensitive profiling. The profiling at procedure level, called in
IHPP *function mode*, allows one to understand which are the hottest paths or, in other terms,
the most frequently activated procedure call chains in a program. The profiling of basic blocks
inside procedures, called *intra-procedural* mode, instead, focus the study of hot paths
on activations of basic block chains inside each procedure; in that mode, the concept of
calling context of a basic block activation have to be reinterpreted as sequence of basic
blocks (inside the same procedure) that has been activated before its activation. The
intra-procedural mode has led to noticeable results, specially with the introduction of
a roll loops option which allows full context-sensitive profiling (k = ∞) without having
an unbounded output size since loops inside procedures are automatically recognized;
in this way, the intra-procedural mode becomes a way for studying the behavior of
algorithms. The last IHPP profiling level, called *inter-procedural mode*, uses the same
ideas of the previously one but, this time, completely ignoring the concept of procedure:
the hole analyzed program is considered as a set of basic blocks and their activation
paths are built independently from their procedure calling context.

[Sapienza University of Rome]: https://www.uniroma1.it/

## Acknowledgments

The **kSlab Forest** and **k-Calling Context Forest** data structures, the **forest join** operation,
the **forest inverseK** operation and the **traceObject** function (and only these ones),
are based on [k-Calling context profiling]:
*Giorgio Ausiello, Camil Demetrescu, Irene Finocchi, Donatella Firmani*,
*27th ACM SIGPLAN Conference on Object-Oriented*
*Programming, Systems, Languages and Applications, October 2012*

[k-Calling context profiling]: https://dl.acm.org/doi/10.1145/2398857.2384679

How to build the tool
-----------------------

1. Download Intel PIN 3.20 from:

    https://software.intel.com/content/www/us/en/develop/articles/pin-a-dynamic-binary-instrumentation-tool.html

2. Extract the tar.gz archive at any given location: it will contain a directory like:
    ```
    pin-3.20-98437-gf02b61307-gcc-linux
    ```

3. Now, let's create an environment variable called $PIN_TOOL with the absolute path to that directory, like:
    ```
    export PIN_TOOL=/home/john/pin/pin-3.20-98437-gf02b61307-gcc-linux
    ```

4. Finally, enter IHPP project's root directory from a command prompt and run:
    ```
    make
    ```


How to use the tool
-----------------------

To use the IHPP tool, run Intel's PIN like this:

```
$PIN_TOOL/pin -t obj-intel64/ihpp.so -funcMode -k 10 -outfile out.txt -kccf -- <your_program>
```

For more about its options, run it with -h like:

```
$PIN_TOOL/pin -t obj-intel64/ihpp.so -h -- true
```

The current help message is:

```
IHPP: An Intraprocedural Hot Path Profiler

-------------------------------------------

Syntax: <PIN> -t ihpp.so <WORKING MODE> [ -funcs <func1>,<func2>,... ]
	[ -k <K_VALUE> | -kinf ] [-outfile <FILE>] [ -xml ]
	<SHOW OPTIONS> <OTHER OPTIONS>


Working modes:
	 -funcMode : procedure-level profiling [ default mode ]
	 -intraMode : intra-procedural basic block profiling
	 -interMode : inter-procedural basic block profiling

Show options:
	-ksf: shows the k-Slab Forest
	-kccf: shows the k-Calling Context Forest
	-showFuncs: shows function's list
	-showBlocks: shows basic block's list
	-blocksDisasm: shows disassembly in the section 'All basic blocks'
	-funcsDisasm: shows disassembly in the section 'All functions'

Other options:
	-xml option produces the output file in xml format
	-joinThreads: k Slab Forests of all thread will be joined
	-rollLoops: when building the kSF in intraMode or interMode,
		loops will be rolled (-kinf is implied)

	-unrollSimpleRec: disables the rolling (by default) of
		single-function recursion in funcMode

	-insTracing: enables instruction tracing mode (slow)
```

Understanding more
-------------------

For a complete understanding about the data structures involved (such as
k-Slab Forest and k-Calling Context Forest) or how the tool actually works,
check out the PDF version of my [B.S. thesis].

[B.S. thesis]: https://github.com/vvaltchev/ihpp/blob/master/doc/bs_thesis.pdf
