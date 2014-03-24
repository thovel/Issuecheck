#ifndef JIRA_UTIL_H
#define JIRA_UTIL_H

#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/ptree.hpp>
#include <jira_query.h>
#include <string>
#include <ostream>
#include <sstream>

namespace jira {

class endpoint
{
private:
    endpoint(){}
    boost::network::uri::uri its_endpoint_uri{};

public:
    endpoint(const char *a_string)
        :endpoint(boost::network::uri::uri(std::string(a_string)))
    {}
    endpoint(const std::string &a_string)
        :endpoint(boost::network::uri::uri(a_string))
    {}
    endpoint(const boost::network::uri::uri &a_uri);
    const boost::network::uri::uri &uri() const
    {
        return its_endpoint_uri;
    }

};

boost::network::uri::uri make_search_url(
        const endpoint &an_endpoint,
        const query &a_query);


void base64(std::ostream &os, const std::string &origin);


template < class T >
inline T make_request(
        const boost::network::uri::uri url,
        const std::string userid,
        const std::string password)
{
    T request {url};

    std::stringstream ssorigin;
    ssorigin << userid << ":" << password;
    std::stringstream sstarget;
    sstarget << "Basic ";
    base64(sstarget, ssorigin.str());

    request << boost::network::header("Authorization", sstarget.str());
    request << boost::network::header("Content-Type", "application/json");
    return request;
}

void fetch_issues(
        const jira::issue_key_list &keys_of_requested_issues,
        const jira::endpoint &an_endpoint,
        const std::string &a_userid,
        const std::string &a_password,
        jira::issue_key_list &not_found_keys,
        std::vector<issue_metadata> &found_issues);

void printTree (std::ostream &os, boost::property_tree::ptree &pt, int level);

}

#endif // JIRA_UTIL_H
