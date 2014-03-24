#ifndef JIRASEARCH_H
#define JIRASEARCH_H

#include <boost/network/protocol/http/client.hpp>
#include <string>

class JiraSearch
{
    private:
        boost::network::http::client client;

    public:
        JiraSearch(){}
        JiraSearch(const JiraSearch &jiraSearchInstance){}
        void get(std::string& ret);

};

#endif // JIRASEARCH_H
