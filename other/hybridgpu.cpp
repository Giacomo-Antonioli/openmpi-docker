#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <chrono>

int main(int argc, char** argv) {
    // Initialize MPI.
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Define matrix dimensions and the number of repetitions.
    // Adjust these parameters to generate a workload that runs longer.
    const int M = 1024;         // Each matrix is M x M (example: 1024x1024)
    const int repetitions = 20;   // Number of times to perform the matrix multiplication

    // Allocate matrices as vectors.
    // Matrix A is initialized with 1.0; B with 2.0; C will store the result.
    std::vector<float> A(M * M, 1.0f);
    std::vector<float> B(M * M, 2.0f);
    std::vector<float> C(M * M, 0.0f);

    // Create pointer variables for mapping.
    float* A_ptr = A.data();
    float* B_ptr = B.data();
    float* C_ptr = C.data();

    // Record start time.
    auto start = std::chrono::high_resolution_clock::now();

    // Perform the heavy GPU computation: repeated dense matrix multiplication.
    // The computation is offloaded to the GPU using OpenMP target offloading.
    for (int rep = 0; rep < repetitions; rep++) {
        // Offload the nested loops to the GPU.
        #pragma omp target teams distribute parallel for collapse(2) \
            map(to: A_ptr[0:M*M], B_ptr[0:M*M]) map(from: C_ptr[0:M*M])
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < M; j++) {
                float sum = 0.0f;
                for (int k = 0; k < M; k++) {
                    sum += A_ptr[i * M + k] * B_ptr[k * M + j];
                }
                C_ptr[i * M + j] = sum;
            }
        }

        // Optionally, print progress from MPI rank 0 every few iterations.
        if (world_rank == 0 && rep % 5 == 0) {
            std::cout << "Iteration " << rep << " completed." << std::endl;
        }
    }

    // Record end time and compute elapsed duration.
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = end - start;
    if (world_rank == 0) {
        std::cout << "Elapsed time for GPU matrix multiplications: "
                  << elapsedSeconds.count() << " seconds." << std::endl;
    }

    // As an additional demonstration, launch an OpenMP parallel region on the CPU.
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        std::cout << "MPI Rank " << world_rank
                  << ", CPU thread " << thread_id
                  << " of " << num_threads << " running." << std::endl;
    }

    MPI_Finalize();
    return 0;
}
