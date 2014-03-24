#include "jira_issue.h"
//#include <boost/exception/all.hpp>
#include <stdexcept>
#include <cctype>
#include <sstream>

std::ostream& jira::operator<<(std::ostream& os, const issue_key& an_issue_key)
{
    if(an_issue_key.is_empty())
        return os << "";
    else
        return os << an_issue_key.its_project_key << '-' << an_issue_key.its_issue_nr;
}
std::ostream& jira::operator<<(std::ostream& os, const issue_key_list& issue_key_list)
{
    int count = 0;
    int size = issue_key_list.size();
    for(const issue_key &key : issue_key_list)
    {
        count++;
        if(count == 1)
            os << " " << key;
        else if(count < size)
            os << ", " << key;
        else
            os << " and " << key;

    }
    return os;
}

const bool jira::issue_key_has_empty_members(const issue_key_list &issue_keys)
{
    for (issue_key_list::iterator it = issue_keys.begin() ; it != issue_keys.end(); ++it)
        if(it->is_empty())
            return true;

    return false;
}
jira::issue_key::issue_key(const std::string &a_project_key, const int &an_issue_nr)
{
    std::stringstream ss {};
    for(char ch : a_project_key)
    {
        // only letters numbers and underscore
        // all letters to upper
        if(std::isalpha(ch) )
            ss << toupper(ch);

        // further only numbers and underscore allowed
        if(std::isdigit(ch) || '_')
            ss << std::toupper(ch);
    }

    this->its_project_key = ss.str();
    this->its_issue_nr = an_issue_nr;

}
/*
 * leading and trailing whitespace is stripped
 * first non whitespace must be an upper case letter
 * one dash is required.
 * after the dash, only digits are allowed until the last whitespace.
 * before the dash, only digits, uppercase letters and underscoress are allowed.
 */
jira::issue_key::issue_key(const std::string &an_issue_key)
{
    bool seen_dash = false;
    bool seen_first_alpha = false;
    bool seen_first_digit_after_dash = false;

    std::stringstream ass {};
    std::stringstream dss {};
    for(char ch : an_issue_key)
    {
        if(ch == '-' && !seen_first_alpha )
            throw std::invalid_argument(THOVEL_MESSAGE("an issue key must start with a capital"));

        if(ch == '-')
        {
            if(!seen_dash)
                seen_dash = true;
            else
                throw std::invalid_argument(THOVEL_MESSAGE("an issue id must have one dash, not more"));
        }
        else
        {
            if(!seen_dash)
            {
              if(std::isalpha(ch))
              {
                  if(!std::isupper(ch))
                      throw std::invalid_argument(THOVEL_MESSAGE("a project key must have capitals, digits or underscores (1)"));

                  if(!seen_first_alpha)
                      seen_first_alpha = true;
                  ass << (char)toupper(ch);
              }
              else if( std::isdigit(ch) )
              {
                if(seen_first_alpha)
                  ass << ch;
                else
                    throw std::invalid_argument(THOVEL_MESSAGE("The first position of the issue key must be a capital"));
              }
              else if(ch =='_')
                  ass << std::toupper(ch);
              else if(ch==' ' || ch=='\t' || ch=='\n')
                  if(seen_first_alpha)
                    throw std::invalid_argument(THOVEL_MESSAGE("no whitespace allowed in an issue key"));
                  else
                    // trim leading space
                    continue;
              else
                  throw std::invalid_argument(THOVEL_MESSAGE("a project key must have capitals, digits or underscores"));

            }
            else // seen_dash == true
            {
                if( std::isdigit(ch) )
                {
                    if(!seen_first_digit_after_dash)
                        seen_first_digit_after_dash = true;
                   dss << ch;
                }
                else if(ch==' ' || ch=='\t' || ch=='\n')
                    // whitespace
                    if(seen_first_digit_after_dash)
                        break;
                else
                    throw std::invalid_argument(THOVEL_MESSAGE("an issue number can only have digits "));

            }
        }
    }
    if(!seen_dash ||
        !seen_first_alpha ||
        !seen_first_digit_after_dash )
    {
       throw std::invalid_argument(THOVEL_MESSAGE("a jira issue must start with a capital followed by some letters ending in a dash followed by some digits"));
    }

    this->its_project_key = ass.str();
    this->its_issue_nr = std::stoi(dss.str());

}
