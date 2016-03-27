#include "geiger/geiger.h"

#include <gtest/gtest.h>

struct Basic : public ::testing::Test
{
    static void SetUpTestCase()
    {
        geiger::init();
    }

    Basic()
    {
        m_suite.on_test_complete([this](const std::string&, const geiger::test_report&) { ++m_on_test_complete_calls; });
        m_suite.on_complete([this](const geiger::suite_report&) { ++m_on_complete_calls; });
    }

protected:
    geiger::suite<> m_suite;
    int m_on_test_complete_calls = 0;
    int m_on_complete_calls = 0;
};

TEST_F(Basic, NoTest)
{
    m_suite.run();

    ASSERT_EQ(0, m_on_test_complete_calls);
    ASSERT_EQ(1, m_on_complete_calls);
}

TEST_F(Basic, OneTest_Lambda)
{
    m_suite.add("foo", []() { });
    m_suite.run();

    ASSERT_EQ(1, m_on_test_complete_calls);
    ASSERT_EQ(1, m_on_complete_calls);
}

TEST_F(Basic, TwoTests_Lambda)
{
    m_suite.add("foo", []() { });
    m_suite.add("bar", []() { });
    m_suite.run();

    ASSERT_EQ(2, m_on_test_complete_calls);
    ASSERT_EQ(1, m_on_complete_calls);
}

TEST_F(Basic, TwoTests_RunTwice)
{
    m_suite.add("foo", []() { });
    m_suite.add("bar", []() { });
    m_suite.run();
    m_suite.run();

    ASSERT_EQ(4, m_on_test_complete_calls);
    ASSERT_EQ(2, m_on_complete_calls);
}

TEST_F(Basic, OneTest_CorrectName)
{
    m_suite.add("foo", []() { });
    m_suite.on_test_complete([this](const std::string& name, const geiger::test_report&)
    {
        ASSERT_EQ("foo", name);

    });
    m_suite.run();
}

TEST_F(Basic, OneTest_NoPAPICounter)
{
    m_suite.add("foo", []() { });
    m_suite.on_test_complete([this](const std::string&, const geiger::test_report& report)
    {
        ASSERT_EQ(0, report.papi_counters().size());

    });
    m_suite.run();
}

TEST_F(Basic, TwoTests_CorrectOrderReports)
{
    m_suite.add("foo", []() { });
    m_suite.add("bar", []() { });
    m_suite.on_test_complete([this](const std::string& name, const geiger::test_report&)
    {
        static int i = 0;

        if (i++ == 0)
            ASSERT_EQ("foo", name);
        else
            ASSERT_EQ("bar", name);

    });
    m_suite.run();
}
