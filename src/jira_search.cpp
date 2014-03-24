#include "jirasearch.h"

using namespace boost::network;
using namespace boost::network::http;
using namespace std;

void JiraSearch::get(std::string& ret)
{
    client::request request_("http://vg.no/");
    request_ << header("Connection", "close");
    boost::network::http::client client2_;
    client::response response_ = client2_.get(request_);
    ret = body(response_);
}
