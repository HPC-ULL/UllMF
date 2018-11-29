Usage
=====
To use UllMF, your iterative problem has to be sorrounded following this example:

~~~
#include <ullmf.h>

// ... MPI Initialization ...
// Mpi communicator can be MPI_COMM_WORLD or a user defined  
// num_procs = Number of processors;
// id = ProcessID; N = ProblemSize 
// bs = size of workload unit in the memory
counts[id] = N / num_procs; 
displs[id] = id * counts[id];
ullmf_calibration_t*  dlb;
int root = 0;

ullmf_strategy_t* strategy =  ullmf_strategy_heuristic_energy;
ullmf_mpi_setup(&calib, counts, displs, bs, strategy, root, mpi_comm);

ullmf_mpi_init(calib); 

for (i = 0; i < n; i++) { 
  ullmf_mpi_start(calib);
  for (i = displs[id]; i < displs[id] + counts[id]; i++) {
    // ... Work ...
  }
  ullmf_mpi_stop(calib, counts, displs);
  // ... Share results with other processes
}
ullmf_mpi_shutdown(calib); // Finish measurements if they are not stopped
ullmf_mpi_free(calib);
~~~

@ref ullmf_mpi_setup has to be called to initialize the dynamic load balancer _dlb_.
In order to do so, an strategy has to be selected (e.g: @ref ullmf_strategy_heuristic_energy
@ref ullmf_strategy_calibrate).

Then, in between each problem iteration @ref ullmf_mpi_start and @ref ullmf_mpi_stop have to be called.
It is important that for every start there is an associated stop. This will redistribute
and change de values inside the counts and displs vectors.

Finally any internal unfinished operation can be terminated with @ref ullmf_mpi_shutdown, followed
by a memory free with @ref ullmf_mpi_free if the _dlb_ is not going to be used again. 
