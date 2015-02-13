/**
 * TETDeclaration is a header that declares a Testable External Type
 * which can is a typedef to the external type, or in a testing
 * build a typedef to a mock of the external type, with the same
 * interface.
 */

#ifdef TESTING
#include TET_TEST_PATH
#else
#include TET_EXTERNAL_PATH
#endif

namespace ares
{
#ifdef TESTING
    typedef BOOST_PP_CAT(testing::Mock, TET_NAME) TET_NAME;
#else
    typedef BWAPI::TET_NAME TET_NAME;
#endif
}

