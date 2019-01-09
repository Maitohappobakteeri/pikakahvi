#define BOOST_TEST_LOG_LEVEL all
#define BOOST_TEST_SHOW_PROGRESS yes
#define BOOST_TEST_MODULE common-unit-test
#include <boost/test/unit_test.hpp>

#define  SKIP_BOOSTTEST
#include "repository/taskrepository.cpp"
#include "db/conversions.cpp"
