#define BOOST_TEST_MODULE async_single_thread

#include "async_adaptor.h"
#include <sstream>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(bulk_suite)

BOOST_AUTO_TEST_CASE(test_regular)
{
    std::string ans =
        R"(bulk: 1
bulk: 2
bulk: 6
)";

    std::ostringstream oss;
    {
        auto terminalLogThreadManager = std::make_shared<ThreadManager>();

        std::shared_ptr<ThreadManager> terminal;//!!!
        terminalLogThreadManager->subscribe(std::make_shared<terminal_observer>(oss));

        async::AsyncAdaptor asyncAdaptor({terminalLogThreadManager}, 1);
        asyncAdaptor.collectData("1", 1); 
        asyncAdaptor.collectData("\n2\n", 3);
        asyncAdaptor.collectData("3", 1);
        asyncAdaptor.collectData("\n", 1);
    }

    BOOST_CHECK_EQUAL(ans, oss.str());
}

BOOST_AUTO_TEST_CASE(test_regular_2)
{
    std::string ans = R"(bulk: 1, 2
bulk: 6
)";

    std::ostringstream oss;

    {
        auto terminalLogThreadManager = std::make_shared<ThreadManager>();

        std::shared_ptr<ThreadManager> terminal;
        terminalLogThreadManager->subscribe(std::make_shared<terminal_observer>(oss));
        async::AsyncAdaptor asyncAdaptor({terminalLogThreadManager}, 2);
        asyncAdaptor.collectData("1", 1);
        asyncAdaptor.collectData("\n2\n", 3);
        asyncAdaptor.collectData("3", 1);
        asyncAdaptor.collectData("\n", 1);

    }

    BOOST_CHECK_EQUAL(ans, oss.str());
}

BOOST_AUTO_TEST_CASE(test_nested_1)
{
    std::string ans = R"(bulk: 1
bulk: 2
bulk: 6
)";

    // 1
    // {
    // 2
    // }
    // 3

    std::ostringstream oss;

    {
        auto terminalLogThreadManager = std::make_shared<ThreadManager>();

        std::shared_ptr<ThreadManager> terminal;
        terminalLogThreadManager->subscribe(std::make_shared<terminal_observer>(oss));
        async::AsyncAdaptor asyncAdaptor({terminalLogThreadManager}, 5);
        asyncAdaptor.collectData("1", 1);
        asyncAdaptor.collectData("\n{\n", 3);
        asyncAdaptor.collectData("2\n}", 3);
        asyncAdaptor.collectData("\n3", 2);
        asyncAdaptor.collectData("\n", 1);

    }
    BOOST_CHECK_EQUAL(ans, oss.str());
}

BOOST_AUTO_TEST_CASE(test_nested_2)
{
    std::string ans = R"(bulk: 1, 2
bulk: 6
)";

// 1
// 2
// {
// 3
// }

    std::ostringstream oss;

    {
        auto terminalLogThreadManager = std::make_shared<ThreadManager>();

        std::shared_ptr<ThreadManager> terminal;
        terminalLogThreadManager->subscribe(std::make_shared<terminal_observer>(oss));
        async::AsyncAdaptor asyncAdaptor({terminalLogThreadManager}, 5);

        asyncAdaptor.collectData("1", 1);
        asyncAdaptor.collectData("\n2\n{", 4);
        asyncAdaptor.collectData("\n3", 2);
        asyncAdaptor.collectData("\n", 1);
        asyncAdaptor.collectData("}\n", 2);

    }

    BOOST_CHECK_EQUAL(ans, oss.str());
}

BOOST_AUTO_TEST_SUITE_END()