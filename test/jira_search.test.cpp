// thing.test.cpp
//#include "jirasearch.cpp"
#include <boost/network/protocol/http/client.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(SimpleCPPNETLIB, positive) {

    boost::network::http::client* aClient = new boost::network::http::client();
    std::cout << body(aClient->get(boost::network::http::client::request("http://vg.no/"))) << std::endl;
    delete aClient;

}

/*TEST(JiraSearchTest, positive) {

    std::string response;
    {
        JiraSearch aSearch;
        aSearch.get(response);
    }
    cout << response;

}*/

