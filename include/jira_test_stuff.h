#ifndef JIRA_TEST_STUFF_H
#define JIRA_TEST_STUFF_H

#include <stdlib.h>     /* getenv */
#include <string>

namespace thovel{

    class jira_test_stuff
    {
        private:
            std::string its_userid {};
            std::string its_password {};
            std::string its_endpoint {};
        public:
            jira_test_stuff()
                :its_userid {getenv ("ISSUECHECK_USERID")},
            its_password {getenv ("ISSUECHECK_PASSWORD")},
            its_endpoint {getenv ("ISSUECHECK_ENDPOINT")}
            {}
            const std::string &userid() const { return its_userid;}
            const std::string &password() const { return its_password;}
            const std::string &endpoint() const { return its_endpoint;}
    };

}

#endif // JIRA_TEST_STUFF_H
