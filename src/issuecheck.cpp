#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/network/uri/uri_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <stdlib.h>     /* getenv */

#include <gtest/gtest.h>

#include "jira_issue.h"
#include "jira_query.h"
#include "jira_util.h"
#include "message_util.h"

using namespace std;

namespace po = boost::program_options;
using namespace boost::property_tree;
using namespace boost::network;

using namespace jira;

int main(int argc, char *argv[]) {

    po::options_description desc("Allowed options");
    message::init_options_description(desc);
    boost::function1<std::string, std::string> name_mapper {message::init_environment_name_mapper()};

    po::variables_map vm;
    po::store(po::parse_environment(desc, name_mapper), vm);

    boost::filesystem::path config_file_path {message::init_config_file_path()};
    if ( exists(config_file_path) && is_regular_file(config_file_path))
    {
        ifstream ifs_config_file {config_file_path.c_str(), std::ifstream::in};
        po::store(po::parse_config_file(ifs_config_file,desc,true), vm);
    }

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    message::validity validity;
    message::program_option_validity(validity, vm);
    if(!validity.is_valid())
    {
        cout << validity.tips.str() << "\n";
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("help"))
    {
         cout << desc << "\n";
         return 1;
    }
    if (vm.count("list-long-options"))
    {
        message::compgen_wordlist_longoptions(cout, desc);
        cout <<  std::endl;
        return 1;
    }

    return message::check(vm);
}


