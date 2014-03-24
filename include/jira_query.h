#ifndef JIRA_QUERY_H
#define JIRA_QUERY_H

//#include <boost/network/uri/encode.hpp>
//#include <boost/network/uri/uri.hpp>
#include <string>
#include <array>
#include <jira_issue.h>

namespace jira {

class query
{
    public:
        virtual void print_to(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const jira::query &a_query);

};
std::ostream& operator<<(std::ostream& os, const jira::query &a_query);

class issue_query : public query, private issue_key_list
{
    public:
        issue_query(const issue_key &an_issue_key)
        {
            if(an_issue_key.is_empty())
                throw std::invalid_argument("new issue_query(const issue_key &an_issue_key) : an_issue_key should return false for is_empty()");
            issue_key_list::insert(an_issue_key);
        }
        issue_query(const issue_key_list &an_issue_key_list)
        {
            if(issue_key_has_empty_members(an_issue_key_list))
                throw std::invalid_argument("new issue_query(const issue_key_list &an_issue_key_list) : all issues in an_issue_key_list should return false for is_empty()");
            for(auto key : an_issue_key_list)
            {
                issue_key_list::insert(key);
            }
        }
        virtual ~issue_query(){}

        std::pair<iterator, bool>
        insert(const issue_key& item)
        {
            if(item.is_empty())
                throw std::invalid_argument("new issue_query(const issue_key_list &an_issue_key_list) : all issues in an_issue_key_list should return false for is_empty()");
            return issue_key_list::insert(item);
        }

        using issue_key_list::size;
        using issue_key_list::const_iterator;
        using issue_key_list::cbegin;
        using issue_key_list::cend;
        using issue_key_list::const_reverse_iterator;
        using issue_key_list::crbegin;
        using issue_key_list::crend;

        //friend std::ostream& operator<<(std::ostream& os, const query& a_query);
        virtual void print_to(std::ostream& os) const;

};

std::ostream& operator<<(std::ostream& os, const issue_query& an_issue_query);

}

#endif // JIRA_QUERY_H
