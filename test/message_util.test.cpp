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
#include "message_util.h"
#include "jira_test_stuff.h"

using namespace std;

namespace po = boost::program_options;
using namespace boost::property_tree;
using namespace boost::network;

using namespace jira;



TEST(message_util, positive)
{
    thovel::jira_test_stuff stuff;

    po::options_description desc("Allowed options");
    message::init_options_description(desc);

    const int ac {13};

    const char *av[ac];
    av[0] = "command";
    av[1] = "--endpoint";
    av[2] = stuff.endpoint().c_str();
    av[3] = "--userid";
    av[4] = stuff.userid().c_str();
    av[5] = "--password";
    av[6] = stuff.password().c_str();
    av[7] = "--require-existing";
    av[8] = "true";
    av[9] = "--require-unresolved";
    av[10] = "true";
    av[11] = "--require-project-key";
    av[12] = "WFCOL";


    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    /*cout << "endpoint: " << vm["endpoint"].as<std::string>() << "\n";
    cout << "userid: " << vm["userid"].as<std::string>() << "\n";
    cout << "password: " << vm["password"].as<std::string>() << "\n";
    cout << "require-existing: " << vm["require-existing"].as<bool>() << "\n";
    cout << "require-unresolved: " << vm["require-unresolved"].as<bool>() << "\n";
    cout << "require-project-key: " << vm["require-project-key"].as<std::string>() << "\n";*/


    jira::issue_key_list an_issue_key_list;
    stringstream ss {"dit is een leuk verhaaltjes WFCOL-35 kjasdf0 9234nsdfg WFCOL-36 23w4908 sdflkj\n"};
    {
        using namespace message;
        ss >> an_issue_key_list;
    }

    for(auto &issue_key : an_issue_key_list)
        cout << issue_key;

    endpoint an_endpoint {vm["endpoint"].as<std::string>()};
    issue_query query_ {an_issue_key_list};
    uri::uri a_url {make_search_url(an_endpoint, query_)};

    std::stringstream ssurl;
    ssurl << a_url ;
    ASSERT_EQ("https://somejiraserver/rest/api/2/search?jql=key%3DBLA-35%20OR%20key%3DBLA-36",
                  ssurl.str());

    using my_client = http::basic_client<http::tags::http_default_8bit_tcp_resolve, 1,1> ;
    my_client client;
    my_client::request request {
        make_request<my_client::request>(
            a_url,
            vm["userid"].as<std::string>(),
            vm["password"].as<std::string>())
    };

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

