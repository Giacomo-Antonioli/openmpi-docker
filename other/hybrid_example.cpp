#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <thread> // For std::thread::hardware_concurrency (if desired)

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    // Get the global rank and size.
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Split MPI_COMM_WORLD into sub-communicators,
    // grouping processes on the same physical node.
    MPI_Comm node_comm;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &node_comm);

    int node_rank, node_size;
    MPI_Comm_rank(node_comm, &node_rank);
    MPI_Comm_size(node_comm, &node_size);

    // Each process on a node can detect the total available cores.
    // Option 1: Using OpenMP's interface.
    int total_cores = omp_get_num_procs();

    // Option 2: Using the C++ standard (if supported)
    // int total_cores = std::thread::hardware_concurrency();

    // Compute the number of OpenMP threads per MPI process on the same node.
    // In many cases, a simple division is a good starting point:
    int threads_per_proc = total_cores / node_size;
    if (threads_per_proc < 1) threads_per_proc = 1; // Ensure at least one thread.

    // Optionally, print out the detected configuration.
    if (node_rank == 0) { // Let only one process per node print the summary
        std::cout << "Node info: Total cores available = " << total_cores
                  << ", MPI processes on this node = " << node_size
                  << ", OpenMP threads per process = " << threads_per_proc
                  << std::endl;
    }

    // Set the number of OpenMP threads for subsequent parallel regions.
    omp_set_num_threads(threads_per_proc);

    // Simple OpenMP parallel region to demonstrate hybrid usage.
    #pragma omp parallel
    {
        int omp_tid = omp_get_thread_num();
        int omp_nthreads = omp_get_num_threads();

        // Each process prints its MPI and OpenMP identification.
        std::cout << "MPI Rank " << world_rank
                  << " (node rank " << node_rank << ") - "
                  << "Thread " << omp_tid
                  << " of " << omp_nthreads << std::endl;
    }

    MPI_Comm_free(&node_comm);
    MPI_Finalize();
    return 0;
}
