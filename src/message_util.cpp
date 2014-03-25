#include <string>
#include <sstream>
#include <ostream>
#include <fstream>
#include <boost/regex.hpp>

#include "jira_issue.h"
#include "jira_util.h"

#include "message_util.h"

namespace po = boost::program_options;

using namespace std;

namespace message {

    struct environment_parser_name_mapper {
      string operator()(string &environment_key) const;
    };

}

void message::init_options_description(po::options_description &desc)
{
    desc.add_options()
        ("help,h", "produce help message")
        ("endpoint,e", po::value<string>(), "absolute url to jira rest endpoint ending with ...rest/api/2 (no slash)")
        ("userid,u", po::value<string>(), "a JIRA userid")
        ("password,p", po::value<string>(), "a JIRA password")
        ("not-require-issue-key,N", po::bool_switch(), "do not require that at least one JIRA issue key is found in the message")
        ("require-existing,E", po::bool_switch(), "require all JIRA issue keys to represent existing JIRA issues (and get issue from server to find out)")
        ("require-unresolved,U", po::bool_switch(), "require all JIRA issues to be unresolved (and get issue from server to find out)")
        ("show-issue-summary,S", po::bool_switch(), "show issue summary (and get issue from server to get them)")
        ("allowed-project-keys,P", po::value< vector< string> >()->multitoken(), "alow only JIRA issue keys that have one of [arg [arg]] as its JIRA project key")
        ("file,f", po::value<string>(), "use the contents of file [arg] as the message and ignore standard input")
        ("message,m", po::value<string>(), "use [arg] as the message and ignore standard input")
        ("list-long-options", "List all the long options");

}
void message::compgen_wordlist_longoptions(ostream &os, const po::options_description &desc)
{
    bool first=true;
    for (const boost::shared_ptr<po::option_description> &option : desc.options())
    {
        if(!first)
            os << ' ';
        else
            first = false;

        os << option->canonical_display_name(1);
    }
}

string message::environment_parser_name_mapper::operator()(string &environment_key) const
{
    if(environment_key == "ISSUECHECK_USERID")
    {
        return "userid";
    }
    else if(environment_key == "ISSUECHECK_PASSWORD")
    {
        return "password";
    }
    else if(environment_key == "ISSUECHECK_ENDPOINT")
    {
        return "endpoint";
    }
    else
        return "";
}

boost::function1<string, string> message::init_environment_name_mapper()
{
    boost::function1<string, string> name_mapper;
    name_mapper = message::environment_parser_name_mapper();
    return name_mapper;
}
const boost::filesystem::path message::init_config_file_path()
{
    char *home_folder {getenv ("HOME")};
    if(nullptr == home_folder)
        home_folder = getenv ("USERPROFILE");

    if(nullptr != home_folder)
    {
        boost::filesystem::path home_path {home_folder};
        if(is_directory(home_path))
        {
            boost::filesystem::path config_file_path {home_folder};
            config_file_path += home_path.preferred_separator;
            config_file_path +=".issuecheck";
            return config_file_path;
        }
    }
    return boost::filesystem::path {""};
}

void message::init_positional_options_description(po::positional_options_description &pos_desc)
{
    pos_desc.add("input-file", -1);
}
void message::program_option_validity(message::validity &validity_, const po::variables_map &vm)
{

    if(vm.count("help"))
    {
        validity_.valid();
        return;
    }

    if (vm.count("list-long-options"))
    {
        validity_.valid();
        return;
    }

    bool require_existing = vm["require-existing"].as< bool >();
    bool require_unresolved = vm["require-unresolved"].as< bool >();
    bool show_issue_summary = vm["show-issue-summary"].as< bool >();


    bool fetch_jira_issue = false;
    if(require_existing || require_unresolved || show_issue_summary)
    {
        validity_.tips << "Access to JIRA server needed\n";
        fetch_jira_issue = true;
    }

    if(fetch_jira_issue && 0==vm.count("endpoint"))
    {
        validity_.tips << "missing endpoint\n";
        validity_.invalid();
    }
    if(fetch_jira_issue && 0==vm.count("userid"))
    {
        validity_.tips << "missing userid\n";
        validity_.invalid();
    }
    if(fetch_jira_issue && 0==vm.count("password"))
    {
        validity_.tips << "missing password\n";
        validity_.invalid();
    }

}
istream& message::operator>>(istream& is, jira::issue_key_list& an_issue_key_list)
{
    stringstream input {};
    string input_line;
    while(is) {
      getline(is, input_line);
      input << input_line << endl;
    }
    boost::regex regex_issue_key { "[A-Z][A-Z0-9_]*?-[0-9]*" };

    string input_string = input.str();
    auto words_begin =
        boost::sregex_iterator(
                input_string.begin(), input_string.end(), regex_issue_key);
    auto words_end = boost::sregex_iterator();

    const int N = 6;
    for (boost::sregex_iterator i = words_begin; i != words_end; ++i) {
        boost::smatch match = *i;
        an_issue_key_list.insert(match.str());
    }


}

int message::check(const boost::program_options::variables_map &vm)
{
    int exit_status {0};

    jira::issue_key_list referred_keys;
    {
        using namespace message;

        if(vm.count("file") > 0)
        {
            string filename_message {vm["file"].as< string >()};
            if ( !boost::filesystem::exists( filename_message ) )
            {
                cerr << "can't find " << filename_message << endl;
                exit_status = 1;
            }
            else if ( !boost::filesystem::is_regular_file(filename_message) )
            {
                cerr << filename_message << " is not a regular file" << endl;
                exit_status = 1;
            }
            else
            {
                ifstream ifs_message {filename_message};
                ifs_message >> referred_keys;
            }
        }
        else if(vm.count("message") > 0)
        {
            istringstream is_message {vm["message"].as< string >()};
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
                cout << "your commit message refers to no JIRA issues" << endl;
            else
            {
                cerr << "check failed: your commit message should have at least one JIRA issue key" << endl;
                exit_status = 1;
            }
        }

    }

    if(referred_keys.size() > 0)
    {
        bool require_existing { vm["require-existing"].as< bool >()};
        bool require_unresolved { vm["require-unresolved"].as< bool >()};
        bool show_issue_summary { vm["show-issue-summary"].as< bool >()};

        if(!require_existing && !require_unresolved && !show_issue_summary)
            cout << "your commit message refers to" << referred_keys << endl;

        if(require_existing || require_unresolved || show_issue_summary)
        {
            jira::endpoint an_endpoint {vm["endpoint"].as<string>()};
            string a_userid {vm["userid"].as<string>()};
            string a_password {vm["password"].as<string>()};

            jira::issue_key_list not_found_keys {referred_keys};
            vector<jira::issue_metadata> found_issues;

            fetch_issues(
                        referred_keys,
                        an_endpoint,
                        a_userid,
                        a_password,
                        not_found_keys,
                        found_issues);

            {
                if(0 == found_issues.size())
                    cout << "no issues found" << endl;

                for(jira::issue_metadata  issue : found_issues)
                    cout << issue << endl;
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
                    cerr << "check failed: key not found" << endl;
                    exit_status = 1;
                }
                else
                    //not_found_keys.size() > 1
                {
                    cerr << "check failed: " << not_found_keys.size() << " keys not found" << endl;
                    exit_status = 1;
                }

            }

            {
                int count_resolved = 0;
                for(const jira::issue_metadata &issue : found_issues)
                    if(issue.is_resolved)
                        count_resolved++;

                if(count_resolved > 0 && require_unresolved)
                {
                    if(1 == count_resolved)
                        cerr << "check failed: an issue is resolved while it must be unresolved" << endl;
                    else
                        cerr << "check failed: " << count_resolved << " issues are resolved while they must be unresolved" << endl;
                    exit_status = 1;
                }
            }
        }
    }

    if(vm.count("allowed-project-keys") > 0)
    {
        jira::issue_key_list illegal_keys;
        vector<string> allowed_project_keys { vm["allowed-project-keys"].as< vector< string> >() };
        stringstream ss_required_project_keys {};
        int nr_allowed_project_key=0;
        for(const string &allowed_project_key : allowed_project_keys)
        {
            nr_allowed_project_key++;
            if(1 != nr_allowed_project_key)
                ss_required_project_keys << " or ";
            ss_required_project_keys <<  allowed_project_key;
        }
        string s_required_project_keys {ss_required_project_keys.str()};
        for(const jira::issue_key &referred_issue_key : referred_keys)
        {
            bool found=false;
            for(const string &allowed_project_key : allowed_project_keys)
                if(allowed_project_key == referred_issue_key.project_key())
                    found=true;

            if(!found)
                illegal_keys.insert(referred_issue_key);
        }

        if(illegal_keys.size() == 1)
        {
            cerr << "check failed: " << illegal_keys << " must be in project " << s_required_project_keys << endl;
            exit_status = 1;
        }
        else if(illegal_keys.size() > 1)
        {
            cerr << "check failed: " << illegal_keys << " must all be in project " << s_required_project_keys << endl; ;
            exit_status = 1;
        }
    }



}
