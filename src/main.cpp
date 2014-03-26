#include <iostream>
#include <fstream>

#include "message_util.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char *argv[]) {

    po::options_description desc("Allowed options");
    message::init_options_description(desc);
    boost::function1<string, string> name_mapper {message::init_environment_name_mapper()};

    po::variables_map program_options;
    po::store(po::parse_environment(desc, name_mapper), program_options);

    boost::filesystem::path config_file_path {message::init_config_file_path()};
    if ( exists(config_file_path) && is_regular_file(config_file_path))
    {
        ifstream ifs_config_file {config_file_path.c_str(), ifstream::in};
        po::store(po::parse_config_file(ifs_config_file,desc,true), program_options);
    }

    po::store(po::parse_command_line(argc, argv, desc), program_options);
    po::notify(program_options);

    message::validity validity;
    message::program_option_validity(validity, program_options);
    if(!validity.is_valid())
    {
        cout << validity.tips.str() << "\n";
        cout << desc << "\n";
        return 1;
    }

    if (program_options.count("help"))
    {
         cout << desc << "\n";
         return 1;
    }
    if (program_options.count("list-long-options"))
    {
        message::compgen_wordlist_longoptions(cout, desc);
        cout <<  endl;
        return 1;
    }

    if(program_options.count("file") > 0)
    {
        string filename_message {program_options["file"].as< string >()};
        if ( !boost::filesystem::exists( filename_message ) )
        {
            cerr << "can't find " << filename_message << endl;
            return 1;
        }
        else if ( !boost::filesystem::is_regular_file(filename_message) )
        {
            cerr << filename_message << " is not a regular file" << endl;
            return 1;
        }
        else
        {
            ifstream ifs_message {filename_message};
            return message::check(ifs_message, program_options);
        }
    }
    else if(program_options.count("message") > 0)
    {
        istringstream is_message {program_options["message"].as< string >()};
        return message::check(is_message, program_options);
    }
    else
    {
        return message::check(cin, program_options);
    }


}


