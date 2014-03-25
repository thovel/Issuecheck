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



    int exit_status = 0;
    jira::issue_key_list referred_keys;
    {
        using namespace message;

        if(vm.count("file") > 0)
        {
            std::string filename_message {vm["file"].as< std::string >()};
            if ( !boost::filesystem::exists( filename_message ) )
            {
                cerr << "can't find " << filename_message << std::endl;
                exit_status = 1;
            }
            else if ( !boost::filesystem::is_regular_file(filename_message) )
            {
                cerr << filename_message << " is not a regular file" << std::endl;
                exit_status = 1;
            }
            else
            {
                ifstream ifs_message {filename_message, std::ifstream::in};
                ifs_message >> referred_keys;
            }
        }
        else if(vm.count("message") > 0)
        {
            std::istringstream is_message {vm["message"].as< std::string >()};
            is_message >> referred_keys;
        }
        else
        {
            cin >> referred_keys;
        }

        if(0 == referred_keys.size())
        {
            bool not_require_existing { vm["not-require-issue-key"].as< bool >()};
            if(not_require_existing)
                cout << "your commit message refers to no JIRA issues" << std::endl;
            else
            {
                cerr << "check failed: your commit message should have at least one JIRA issue key" << std::endl;
                exit_status = 1;
            }
        }

    }

    bool require_existing { vm["require-existing"].as< bool >()};
    bool require_unresolved { vm["require-unresolved"].as< bool >()};
    bool show_issue_summary { vm["show-issue-summary"].as< bool >()};
    if(referred_keys.size() > 0 && (require_existing || require_unresolved || show_issue_summary))
    {
        jira::endpoint an_endpoint {vm["endpoint"].as<std::string>()};
        std::string a_userid {vm["userid"].as<std::string>()};
        std::string a_password {vm["password"].as<std::string>()};

        jira::issue_key_list not_found_keys {referred_keys};
        std::vector<issue_metadata> found_issues;

        fetch_issues(
                    referred_keys,
                    an_endpoint,
                    a_userid,
                    a_password,
                    not_found_keys,
                    found_issues);

        {
            if(0 == found_issues.size())
                cout << "no issues found" << std::endl;
            /*else if( 1==found_issues.size())
                cout << "issue found: ";
            else
                cout << found_issues.size() << " issues found:" << std::endl;*/

            for(jira::issue_metadata  issue : found_issues)
                cout << issue << std::endl;
        }

        if(require_existing)
        {
            if(0 == not_found_keys.size())
            {
                // all is well;
                ; // do nothing
            }
            else if(1 == not_found_keys.size())
            {
                cerr << "check failed: key not found" << std::endl;
                exit_status = 1;
            }
            else
                //not_found_keys.size() > 1
            {
                cerr << "check failed: " << not_found_keys.size() << " keys not found" << std::endl;
                exit_status = 1;
            }

        }

        {
            int count_resolved = 0;
            for(jira::issue_metadata  issue : found_issues)
                if(issue.is_resolved)
                    count_resolved++;

            if(count_resolved > 0)
            {
                if(require_unresolved)
                {
                    if(1 == count_resolved)
                        cerr << "check failed: an issue is resolved while it must be unresolved" << std::endl;
                    else
                        cerr << "check failed: " << count_resolved << " issues are resolved while they must be unresolved" << std::endl;
                    exit_status = 1;
                }
                /*else
                {
                    if(1 == count_resolved)
                        cout << "warning: an issue is resolved so it might not make sense to refer to it" << std::endl;
                    else
                        cout << "warning: " << count_resolved << " issues are resolved so it might not make sense to refer to them" << std::endl;
                }*/
            }
        }

    }
    else
    {
        if(referred_keys.size() > 0)
            cout << "your commit message refers to" << referred_keys << std::endl;
    }


    if(vm.count("allowed-project-keys") > 0)
    {
        jira::issue_key_list illegal_keys;
        std::vector<std::string> allowed_project_keys { vm["allowed-project-keys"].as< std::vector< std::string> >() };
        std::stringstream ss_required_project_keys {};
        int nr_allowed_project_key=0;
        for(const std::string &allowed_project_key : allowed_project_keys)
        {
            nr_allowed_project_key++;
            if(1 != nr_allowed_project_key)
                ss_required_project_keys << " or ";
            ss_required_project_keys <<  allowed_project_key;
        }
        std::string s_required_project_keys {ss_required_project_keys.str()};
        for(const jira::issue_key &referred_issue_key : referred_keys)
        {
            bool found=false;
            for(const std::string &allowed_project_key : allowed_project_keys)
                if(allowed_project_key == referred_issue_key.project_key())
                    found=true;

            if(!found)
                illegal_keys.insert(referred_issue_key);
        }

        if(illegal_keys.size() == 1)
        {
            cerr << "check failed: " << illegal_keys << " must be in project " << s_required_project_keys << std::endl;
            exit_status = 1;
        }
        else if(illegal_keys.size() > 1)
        {
            cerr << "check failed: " << illegal_keys << " must all be in project " << s_required_project_keys << std::endl; ;
            exit_status = 1;
        }
    }


    return exit_status;
}
