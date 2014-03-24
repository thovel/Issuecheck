#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/network/uri/uri_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <gtest/gtest.h>

#include "jira_issue.h"
#include "jira_query.h"
#include "jira_util.h"

#include "jira_test_stuff.h"

using namespace std;

using namespace boost::property_tree;
using namespace boost::network;

using namespace jira;


TEST(jira_util, positive)
{
    thovel::jira_test_stuff stuff;
    endpoint an_endpoint {stuff.endpoint()};

    issue_query query_ { {"BLA-36","BLA-35"} };
    uri::uri a_url {make_search_url(an_endpoint, query_)};

    std::stringstream ss;

    using my_client = http::basic_client<http::tags::http_default_8bit_tcp_resolve, 1,1> ;
    my_client client;
    my_client::request request {make_request<my_client::request>(a_url,stuff.userid(),stuff.userid())};

    auto response = client.get(request);

    stringstream ssjson;
    ssjson << body(response) ;

    //std::cout << ssjson.str() << endl;

    // Read json.
    ptree pt;
    std::istringstream is (ssjson.str());
    read_json (is, pt);

    //printTree(pt, 0);
    cout << endl;
    cout << endl;
    for(ptree::value_type &v : pt.get_child("issues"))
    {
        std::string issue_status_id = v.second.get<std::string>("fields.status.id");

        if(issue_status_id=="5")
        {

            cerr
                << " "
                << v.second.get<std::string>("fields.issuetype.name")
                << " "
                << v.second.get<std::string>("key")
                << " \""
                << v.second.get<std::string>("fields.summary")
                << "\" "
                << v.second.get_optional<std::string>("fields.resolution.name")
                << endl;
        }
        else
        {
            cout
                << " "
                << v.second.get<std::string>("fields.issuetype.name")
                << " "
                << v.second.get<std::string>("key")
                << " \""
                << v.second.get<std::string>("fields.summary")
                << "\" "
                << v.second.get<std::string>("fields.status.name")
                << endl;
        }
    }
    cout << endl;

}

