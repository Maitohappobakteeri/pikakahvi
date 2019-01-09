
#include "db/conversions.hpp"

#ifndef SKIP_BOOSTTEST
#include <boost/test/unit_test.hpp>
#endif


BOOST_AUTO_TEST_SUITE(db_conversions)

BOOST_AUTO_TEST_CASE( escape_sql_string )
{
    BOOST_CHECK(toSqlString("abc!") == "'abc!'");
    BOOST_CHECK(toSqlString("") == "''");
    BOOST_CHECK(toSqlString("\n  \n") == "'\n  \n'");
    BOOST_CHECK(toSqlString("\"''\"") == "'\"''''\"'");
    BOOST_CHECK(toSqlString("'") == "''''");
}


BOOST_AUTO_TEST_SUITE_END()
