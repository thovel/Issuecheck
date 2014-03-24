//#include "jira_query.h"
#include "jira_issue.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;
using namespace jira;


TEST(jira_issue, positive)
{
    issue_key issue {"BLA-90"};
    ASSERT_NO_THROW();
    cout << issue << endl;

}

TEST(jira_issue, empty)
{
    issue_key empty_issue {};
    issue_key issue {"BLA-90"};
    ASSERT_TRUE(empty_issue.is_empty());
    ASSERT_FALSE(issue.is_empty());
    stringstream ss {};
    ss << empty_issue;
    ASSERT_STREQ(ss.str().c_str(),"");
    cout << issue << " (" << empty_issue << ")" << endl;

}


TEST(jira_issue, trim)
{
    issue_key issue1 {"  BLA-99"};
    issue_key issue2 {"BLA-89   "};
    issue_key issue3 {"   BLA-81   "};
    ASSERT_NO_THROW();
    cout << issue1 << issue2 << issue3 << endl;
}
TEST(jira_issue, projectkey_with_number)
{
    issue_key issue {"BL6-99"};
    ASSERT_NO_THROW();
    cout << issue << endl;
}
TEST(jira_issue, projectkey_ignore_stuff_after_whitespace)
{
        issue_key issue {"LL6-5 8"};
        ASSERT_NO_THROW();
        cout << "issue " << "LL6-5 8" << " becomes " << issue << endl;
}
TEST(jira_issue, projectkey_with_lowercase)
{
    string what {};
    try {
        issue_key issue {"bL6-99"};
        ASSERT_ANY_THROW();
    }
    catch(std::invalid_argument ex)
    {
        cout << ex.what() << endl;
    }

}
TEST(jira_issue, projectkey_first_letter)
{
    string what {};
    try {
        issue_key issue {"7L6-99"};
        ASSERT_ANY_THROW();
    }
    catch(invalid_argument ex)
    {
        cout << ex.what() << endl;
    }
}
TEST(jira_issue, projectkey_must_have_number_after_dash)
{
    string what {};
    try {
        issue_key issue {"LL6-"};
        ASSERT_ANY_THROW();
    }
    catch(invalid_argument ex)
    {
        cout << ex.what() << endl;
    }
}
TEST(jira_issue, projectkey_no_space_before_dash)
{
    string what {};
    try {
        issue_key issue {"L L6-56"};
        ASSERT_ANY_THROW();
    }
    catch(invalid_argument ex)
    {
        cout << ex.what() << endl;
    }

}
TEST(jira_issue, projectkey_one_dash)
{
    string what {};
    try {
        issue_key issue {"LL-6-56"};
        ASSERT_ANY_THROW();
    }
    catch(invalid_argument ex)
    {
        cout << ex.what() << endl;
    }

}




