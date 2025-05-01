#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    // Get rank and size from the global communicator.
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create a communicator for processes on the same physical node.
    MPI_Comm node_comm;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &node_comm);

    // Get the local (node) communicator rank and size.
    int node_rank, node_size;
    MPI_Comm_rank(node_comm, &node_rank);
    MPI_Comm_size(node_comm, &node_size);

    // Get the processor name.
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Every process prints its global and local information.
    std::cout << "Global rank " << world_rank << "/" << world_size
              << " - Node rank " << node_rank << "/" << node_size
              << " on host " << processor_name << std::endl;

    // Only the representative for each node (node_rank == 0) prints a special message.
    if (node_rank == 0) {
        std::cout << ">>> Node representative: Global rank " << world_rank
                  << " on host " << processor_name
                  << " (with " << node_size << " process(es) on this node)" << std::endl;
    }

    MPI_Comm_free(&node_comm);
    MPI_Finalize();
    return 0;
}
