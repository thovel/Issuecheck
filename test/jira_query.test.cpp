//#include "jira_query.h"
#include "jira_query.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace jira;


TEST(jira_query, positive)
{
    issue_query query_ { "BLA-90" };
    ASSERT_TRUE(query_.size() < 2);
    cout << (query &)query_ << endl;

}

TEST(jira_query, distinct)
{
    issue_query query_ { {"BLA-90", "BLA-90 ", "  BLA-90 "} };
    ASSERT_TRUE(query_.size() < 2);
    cout << (query &)query_ << endl;

}

TEST(jira_query, list)
{
    issue_query query_ { {"BLA-99", "BLA-92 ", "  BLA-93 "} };
    ASSERT_TRUE(query_.size() == 3);
    stringstream ss;
    ss << (query &)query_ << endl;
    int ori = ss.str().find_first_of("OR");
    ASSERT_FALSE(ori < 0);
    int ori2 = ss.str().find_first_of("OR", ori);
    ASSERT_FALSE(ori2 < 0);
    cout << (query &)query_ << endl;

}
