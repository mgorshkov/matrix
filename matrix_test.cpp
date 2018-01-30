#define BOOST_TEST_MODULE matrix_test

#include <boost/test/unit_test.hpp>

#include "matrixlib.h"

BOOST_AUTO_TEST_SUITE(matrix_test)

BOOST_AUTO_TEST_CASE(test_empty)
{
    Matrix<int, 2, -1> matrix;

    BOOST_CHECK_EQUAL(matrix.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_get_element)
{
    Matrix<int, 2, -1> matrix;

    auto a = matrix[0][0];
    BOOST_CHECK_EQUAL(a, -1);
    BOOST_CHECK_EQUAL(matrix.size(), 0);
    matrix[100][100] = 314;
    BOOST_CHECK_EQUAL(matrix[100][100], 314);
    BOOST_CHECK_EQUAL(matrix.size(), 1);
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
            std::list<size_t> l{100, 101};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 314);
        }
        else if (loops == 1)
        {
            std::list<size_t> l{102, 103};
            BOOST_CHECK(c.first == l);
            BOOST_CHECK_EQUAL(c.second, 315);
        }
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 2);
}

BOOST_AUTO_TEST_CASE(test_1_dimension_matrix)
{
    Matrix<int, 1, 0> matrix;
    matrix[0] = 1000;
    matrix[1] = 1001;
    matrix[2] = 0;

    BOOST_CHECK_EQUAL(matrix.size(), 2);

    int loops = 0;
    for (auto occupied : matrix)
    {
        if (loops == 0)
            BOOST_CHECK_EQUAL(*occupied->value, 1000);
        else if (loops == 1)
            BOOST_CHECK_EQUAL(*occupied->value, 1001);
        else if (loops == 2)
            BOOST_CHECK_EQUAL(*occupied->value, 0);
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 3);
}

}
