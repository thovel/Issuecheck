#include <string>
#include <sstream>
#include <ostream>
#include <boost/regex.hpp>

#include "jira_issue.h"

#include "message_util.h"

namespace po = boost::program_options;

namespace message {

    struct environment_parser_name_mapper {
      std::string operator()(std::string &environment_key) const;
    };

}

void message::init_options_description(po::options_description &desc)
{
    desc.add_options()
        ("help,h", "produce help message")
        ("endpoint,e", po::value<std::string>(), "absolute url to jira rest endpoint ending with ...rest/api/2 (no slash)")
        ("userid,u", po::value<std::string>(), "a JIRA userid")
        ("password,p", po::value<std::string>(), "a JIRA password")
        ("not-require-issue-key,N", po::bool_switch(), "do not require that at least one JIRA issue key is found in the message")
        ("require-existing,E", po::bool_switch(), "require all JIRA issue keys to represent existing JIRA issues (and get issue from server to find out)")
        ("require-unresolved,U", po::bool_switch(), "require all JIRA issues to be unresolved (and get issue from server to find out)")
        ("show-issue-summary,S", po::bool_switch(), "show issue summary (and get issue from server to get them)")
        ("allowed-project-keys,P", po::value< std::vector< std::string> >()->multitoken(), "alow only JIRA issue keys that have one of [arg [arg]] as its JIRA project key")
        ("file,f", po::value<std::string>(), "use the contents of file [arg] as the message and ignore standard input")
        ("message,m", po::value<std::string>(), "use [arg] as the message and ignore standard input")
        ("list-long-options", "List all the long options");

}
void message::compgen_wordlist_longoptions(std::ostream &os, const po::options_description &desc)
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

std::string message::environment_parser_name_mapper::operator()(std::string &environment_key) const
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

boost::function1<std::string, std::string> message::init_environment_name_mapper()
{
    boost::function1<std::string, std::string> name_mapper;
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
std::istream& message::operator>>(std::istream& is, jira::issue_key_list& an_issue_key_list)
{
    std::stringstream input {};
    std::string input_line;
    while(is) {
      getline(is, input_line);
      input << input_line << std::endl;
    }
    boost::regex regex_issue_key { "[A-Z][A-Z0-9_]*?-[0-9]*" };

    std::string input_string = input.str();
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
