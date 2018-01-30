#include <iostream>
#include <tuple>

#include "matrixlib.h"

int main(int, char const **)
{
    try
    {
        Matrix<int, 2, -1> matrix;

        matrix[100][101] = 314;
        matrix[102][103] = 315;
/*        for (size_t i = 0; i < 10; ++i)
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
        }*/
        std::cout << "Occupied: " << matrix.size() << std::endl;
/*        for (auto occupied : matrix)
        {
            for (auto index : occupied.first)
                std::cout << index << ' ';
            std::cout << *occupied.second << std::endl;
        }*/

        int loops = 0;
        for (auto c: matrix)
        {
            if (loops == 0)
            {
                std::list<size_t> l{100, 101};
                assert(c.first == l);
                assert(c.second == 314);
            }
            else if (loops == 1)
            {
                std::list<size_t> l{102, 103};
                assert(c.first == l);
                assert(c.second == 315);
            }
            ++loops;
        }
        assert(loops == 2);

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
