
IHPP: An Intraprocedural Hot Path Profiler
-------------------------------------------

Author: Vladislav K. Valtchev (vladi32@gmail.com)



The kSlabForest and kCCF data structures, 
the forest join operation, the forest inverseK operation 
and the traceObject function (and only these ones),
are based on the theoretical research of: 

Giorgio Ausiello, Camil Demetrescu, Irene Finocchi, Donatella Firmani
	
Sapienza University of Rome
[ Universita' degli studi di Roma "La Sapienza" ]




BUILDING
---------------

1. Download Intel PIN from http://www.pintool.org/
2. Copy this directory in <PIN_PATH>/source/tools/


Under Linux:

3. Enter in this directory from a command prompt
4. make

Under Windows:

3. Open VC2010/ihpp.sln with Microsoft Visual Studio 2010
4. Build as every other project


Warning: Windows support is EXPERIMENTAL.