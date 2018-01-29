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

    matrix[100][100] = 314;

    int loops = 0;
    for (auto c: matrix)
    {        
        std::list<size_t> l{100, 100};        
        BOOST_CHECK_EQUAL(c.first.size(), l.size());
        BOOST_CHECK(c.first == l);
        BOOST_CHECK_EQUAL(c.second, 314);
        ++loops;
    }
    BOOST_CHECK_EQUAL(loops, 1);
}
}
