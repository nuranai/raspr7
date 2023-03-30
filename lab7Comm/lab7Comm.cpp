#include <iostream>
#include <mpi.h>


int main(int argc, char*argv[])
{
    MPI_Init(&argc, &argv);

    MPI_Status status;
    int columns;

    //int me;

    //MPI_Comm_rank(MPI_COMM_WORLD, &me);

    //receiving columns
    MPI_Recv(&columns, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

    //source
    int source = status.MPI_SOURCE;

    //matrix init
    int** matrixPart = new int* [3];
    for (int i = 0; i < 3; i++) {
        matrixPart[i] = new int[columns];
    }

    bool flag = true;

    while (flag) {
        //get rows
        int rows = 0;
        MPI_Recv(&rows, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

        // if rows 0 => finished program
        if (rows == 0) {
            flag = false;
        }
        else {
            MPI_Recv(matrixPart[0], columns, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(matrixPart[1], columns, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
            if (rows > 2)
                MPI_Recv(matrixPart[2], columns, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

     //       std::cout << "comm test::";
            for (int i = 0; i < columns - 1; i++) {
       //         std::cout << matrixPart[0][i] << ' ';

                // finding square sub-matrixes if its there change next element
				if (
					matrixPart[1][i] == matrixPart[0][i] && matrixPart[1][i] == matrixPart[1][i + 1] && matrixPart[0][i] == matrixPart[0][i + 1]
					|| rows > 2
					&& matrixPart[1][i] == matrixPart[2][i] && matrixPart[1][i] == matrixPart[1][i + 1] && matrixPart[2][i] == matrixPart[2][i + 1]
					) {
                    matrixPart[1][i + 1] ^= 1;
				}

                //if (rows > 2) {
                //    std::cout << "1-i:: " << matrixPart[1][i] << " 1-i+1:: " << matrixPart[1][i+1] <<std::endl << "2-i:: " <<  matrixPart[2][i] << " 2-i+1:: " << matrixPart[2][i+1] << std::endl;
                //    std::cout << "_______________" << std::endl;
                //}
            }
          //  std::cout << std::endl;

            //sending back changed row
            MPI_Send(matrixPart[1], columns, MPI_INT, source, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
}
