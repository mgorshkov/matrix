#include <iostream>
#include <tuple>

#include "matrixlib.h"

int main(int, char const **)
{
    try
    {
        Matrix<int, 2, 0> matrix;
	    for (size_t i = 0; i < 10; ++i)
	    {
            matrix[i][i] = i;
	    	matrix[i][9 - i] = 9 - i;
	    }
	    for (size_t i = 1; i <= 8; ++i)
	    {
		    for (size_t j = 1; j <= 8; ++j)
		    {
                std::cout << matrix[i][j] << ' ';
	    	}
	    	std::cout << std::endl;
	    }
	    std::cout << "Occupied: " << matrix.size() << std::endl;
        for (auto occupied : matrix)
	    {
            std::list<int> l;
			int v;
            std::tie(l, v) = occupied;
            for (auto index : l)
                std::cout << index << ' ';
            std::cout << v << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
