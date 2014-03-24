#ifndef COMMIT_MESSAGE_H
#define COMMIT_MESSAGE_H

#include <sstream>

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/config.hpp>
#include <boost/program_options/environment_iterator.hpp>
#include <boost/program_options/eof_iterator.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/version.hpp>
#include <boost/function/function1.hpp>


#include "jira_issue.h"

namespace message {

void init_options_description(boost::program_options::options_description &desc);
boost::function1<std::string, std::string> init_environment_name_mapper();

std::istream& operator>>(std::istream& is, jira::issue_key_list& an_issue_key_list);

class validity
{
    private:
        bool its_valid = true;
    public:
        std::stringstream tips{};
        void invalid(){its_valid=false;}
        void valid(){;/*if not valid, do not make valid*/}
        bool is_valid(){return its_valid;}
};

void program_option_validity(validity &validity_,const boost::program_options::variables_map &vm);
void init_positional_options_description(boost::program_options::positional_options_description &pos_desc);

}
#endif // COMMIT_MESSAGE_H
