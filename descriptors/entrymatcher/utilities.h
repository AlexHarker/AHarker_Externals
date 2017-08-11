
#ifndef UTILITIES_H
#define UTILITIES_H

#include <ext.h>
#include <ext_obex.h>

enum TestType
{
    TEST_NONE,
    TEST_MATCH,
    TEST_LESS_THAN,
    TEST_GREATER_THAN,
    TEST_LESS_THAN_EQ,
    TEST_GREATER_THAN_EQ,
    TEST_DISTANCE,
    TEST_SCALE,
    TEST_WITHIN,
    TEST_DISTANCE_RATIO,
    TEST_SCALE_RATIO,
    TEST_WITHIN_RATIO
};

void init_test_symbols();
TestType entrymatcher_test_types(t_atom *argv);

#endif
