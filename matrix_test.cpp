#define BOOST_TEST_MODULE matrix_test

#include <boost/test/unit_test.hpp>

#include "matrixlib.h"

/**
 * @brief Unit tests for N-dimensional matrix library.
 */
BOOST_AUTO_TEST_SUITE(matrix_test)

BOOST_AUTO_TEST_CASE(test_empty)
{
    Matrix<int, 2, -1> matrix;

    BOOST_CHECK_EQUAL(matrix.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_iterator)
{
    Matrix<int, 2, -1> matrix;

    matrix[100][101] = 314;
    matrix[102][103] = 315;

    int loops = 0;
    for (auto c: matrix)
    {
        if (loops == 0)
        {
            std::vector<size_t> l{100, 101};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 314);
        }
        else if (loops == 1)
        {
            std::vector<size_t> l{102, 103};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 315);
        }
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

BOOST_AUTO_TEST_CASE(test_3_dimensions_matrix)
{
    Matrix<int, 3, -1> matrix;

    matrix[100][101][102] = 314;
    matrix[103][104][105] = 315;

    int loops = 0;
    for (auto c: matrix)
    {
        if (loops == 0)
        {
            std::vector<size_t> l{100, 101, 102};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 314);
        }
        else if (loops == 1)
        {
            std::vector<size_t> l{103, 104, 105};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 315);
        }
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

}
