Ull Multiobjective Framework 
==========================
The Ull Multiobjective Framework provides the necessary tools to perform 
dynamic load balancing in parallel iterative problems, mainly for heterogeneous systems. It is designed to simplify
complex metric gathering and algorithms with a simple interface that require few lines of code.

There are three implemented strategies to perform the dynamic load balancing:

* Calibrate
* Energy Heuristic
* Time Heuristic

Both the Calibrate and Time Heuristic perform load balance based on the differences in computational power.
They differenciate from each other in how they generate new distributions.
The Energy heuristic perform operations that are similar to the Time Heuristic, focused mainly in reducing energy
consumption.

More details can be found in the associated publications.

Documentation
-------------

Doxygen-generated can be found at:

https://hpc-ull.github.io/ullmf/

License
-------
UllMF is released under the GPLv2 license.

Dependencies
------------
Required:
* MPI, required for gathering metrics from the different parallel processes. 

Optional:
* [EML](https://github.com/HPC-ULL/eml), for Energy measurement support

Contact
-------
Universidad de La Laguna, High Performance Computing group <cap@pcg.ull.es>

Homepage: http://cap.pcg.ull.es

Acknowledgements
----------------

This work was supported by the Spanish Ministry of Science, Innovation and Universities through
the TIN2016-78919-R project, the Government of the Canary Islands,
with the project ProID2017010130 and the grant TESIS2017010134, which is
co-financed by the Ministry of Economy, Industry, Commerce and Knowledge of Canary Islands and 
the European Social Funds (ESF), operative program integrated of Canary Islands 2014-2020
Strategy Aim 3, Priority Topic 74(85\%); the Spanish network CAPAP-H, and the European COST Action CHIPSET.


