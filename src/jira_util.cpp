#include <iostream>
#include <sstream>
#include <iostream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/network/uri/uri_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "jira_issue.h"
#include "jira_query.h"
#include "jira_util.h"

using namespace boost::network;
using namespace boost::property_tree;

jira::endpoint::endpoint(const uri::uri &a_uri)
{
    its_endpoint_uri = a_uri;
}

uri::uri jira::make_search_url(const endpoint &an_endpoint, const query &a_query)
{
    std::stringstream ss;
    ss << a_query;

    uri::uri a_uri { an_endpoint.uri() };
    a_uri << uri::path("/search") << uri::query("jql",uri::encoded(ss.str()));
    return a_uri;
}

void jira::base64(std::ostream &os, const std::string &origin)
{
    using namespace boost::archive::iterators;

    typedef
    base64_from_binary<
        transform_width<const char *, 6, 8>
        > base64_text;

    std::copy(
        base64_text(origin.c_str()),
        base64_text(origin.c_str() + origin.size()),
        boost::archive::iterators::ostream_iterator<char>(os)
        );

}
void jira::fetch_issues(
        const jira::issue_key_list &keys_of_requested_issues,
        const endpoint &an_endpoint,
        const std::string &a_userid,
        const std::string &a_password,
        jira::issue_key_list &not_found_keys,
        std::vector<issue_metadata> &found_issues)
{

    issue_query query_ {keys_of_requested_issues};
    uri::uri a_url {make_search_url(an_endpoint, query_)};

    using my_client = http::basic_client<http::tags::http_default_8bit_tcp_resolve, 1,1> ;
    my_client client;
    my_client::request request {
        make_request<my_client::request>(
                    a_url,
                    a_userid,
                    a_password)
    };

    try {
        auto response = client.get(request);

        std::stringstream ssjson;
        ssjson << body(response) ;

        //std::cout << ssjson.str() << endl;

        // Read json into a property tree.
        ptree pt;
        std::istringstream is (ssjson.str());

        read_json (is, pt);
        auto error_messages = pt.get_child_optional("errorMessages");
        if(error_messages)
        {
            std::cout <<
                 "error while requesting " << keys_of_requested_issues <<
                 " like so: " << a_url << std::endl;
            jira::printTree(std::cout, error_messages.get(), 0);
            std::cout << std::endl;
        }
        else
        {

            //printTree(pt, 0);

            for(ptree::value_type &v : pt.get_child("issues"))
            {
                bool is_resolved = true;
                std::string resolution_name {"--"};
                std::initializer_list<boost::optional<std::string > > resolution_names {v.second.get_optional<std::string>("fields.resolution.name")};
                for(boost::optional<std::string > name : resolution_names)
                {
                    if(!name)
                        is_resolved = false;
                    else
                        resolution_name = name.get();
                }

                jira::issue_metadata m
                {
                    issue_key(v.second.get<std::string>("key")),
                            v.second.get<std::string>("fields.issuetype.name"),
                            v.second.get<std::string>("fields.status.id"),
                            v.second.get<std::string>("fields.status.name"),
                            resolution_name,
                            is_resolved,
                            v.second.get<std::string>("fields.summary"),
                };
                auto iterFound = keys_of_requested_issues.find(m.key);
                if(keys_of_requested_issues.end() != iterFound)
                {
                    found_issues.push_back(m);
                    auto iterFoundinNotFound = not_found_keys.find(m.key);
                    if(not_found_keys.end() != iterFound)
                        not_found_keys.erase(iterFoundinNotFound);
                }
                else
                    std::cout << "huh? Jira returned an issue that I did not ask for!!!" << std::endl;
            }
        }
    }
    catch(std::exception& e)
    {
        std::cout << "something went wrong communicating to " << a_url.host() << std::endl;
        std::cout << "error: " << e.what() << std::endl;
    }

}

namespace jira {
std::string indent(int level);
}
std::string jira::indent(int level)
{
  std::string s;
  for (int i=0; i<level; i++)
      s += "  ";
  return s;
}

void jira::printTree (std::ostream &os, ptree &pt, int level)
{
  if (pt.empty())
  {
    os << "\""<< pt.data()<< "\"";
  }
  else
  {
    if (level)
        os << std::endl;

    os << indent(level) << "{" << std::endl;
    for (ptree::iterator pos = pt.begin(); pos != pt.end();)
    {
      os << indent(level+1) << "\"" << pos->first << "\": ";

      jira::printTree(os,pos->second, level + 1);

      ++pos;
      if (pos != pt.end()) {
        os << ",";
      }
      os << std::endl;
    }
    os << indent(level) << " }";
  }
  return;
}
