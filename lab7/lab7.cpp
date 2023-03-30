#include <iostream>
#include <mpi.h>
#include <math.h>

const int N = 6;
const int M = 6;

int main(int argc, char*argv[])
{
    int source, commSize;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &source);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    //std::cout << "commsize::" << commSize << std::endl;

    int** matrix = new int* [N];
    //matrix fill
    std::cout << "begin matrix::" << std::endl;
    for (int i = 0; i < N; i++) {
        matrix[i] = new int[M];
        for (int j = 0; j < M; j++) {
            matrix[i][j] = rand() % 2;
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    //sending columns
    for (int i = 0; i < commSize; i++) {
        if (source != i) {
            MPI_Send(&M, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    int comm = source;
    for (int i = 1; i < N; i += 2) {
        if (comm == source) {
            comm = (comm + 1) % commSize;
        }
        //std::cout << "cur comm::" << comm << std::endl;

        // rows that will be send
		int rows = std::min(N - i + 1, 3);

        //std::cout << "sender rows:: " << rows << "vals;: " << N << ' ' << i << "both::" << N - i << std::endl;

		MPI_Send(&rows, 1, MPI_INT, comm, 0, MPI_COMM_WORLD);

		MPI_Send(matrix[i - 1], M, MPI_INT, comm, 0, MPI_COMM_WORLD);
		MPI_Send(matrix[i], M, MPI_INT, comm, 0, MPI_COMM_WORLD);
		if (rows == 3)
			MPI_Send(matrix[i + 1], M, MPI_INT, comm, 0, MPI_COMM_WORLD);

        // next comm
        comm = (comm + 1) % commSize;
    }

    // deactivate comms
    for (int comm = 0; comm < commSize; comm++) {
        if (comm != source) {
            int rows = 0;
            MPI_Send(&rows, 1, MPI_INT, comm, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Status status;
    comm = source;

    // get changed rows
    for (int i = 1; i < N; i += 2) {
		//std::cout << "test::" << i << std::endl;
        if (comm == source) {
            comm = (comm + 1) % commSize;
        }
        MPI_Recv(matrix[i], M, MPI_INT, comm, 0, MPI_COMM_WORLD, &status);
        //for (int j = 0; j < M; j++) {
        //    std::cout << matrix[i][j] << ' ';
        //}
//        std::cout << std::endl;
        comm = (comm + 1) % commSize;
    }

    std::cout << "result matrix::" << std::endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
}
