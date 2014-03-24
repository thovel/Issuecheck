#ifndef JIRA_ISSUE_H
#define JIRA_ISSUE_H

#include <string>
#include <set>
#include <vector>
#include <ostream>
#include <thovel_error.h>

namespace jira {

class issue_key;

class issue_key_list;

class issue_key
{
    private:
        std::string its_project_key {};
        int its_issue_nr {};
    public:
        issue_key(const jira::issue_key &an_issue_key)
            :its_project_key {an_issue_key.its_project_key},
             its_issue_nr { an_issue_key.its_issue_nr}
        {
        }
        issue_key()
        {

        }
        issue_key(const std::string &a_project_key, const int &an_issue_nr);
        issue_key(const std::string &an_issue_key);
        issue_key(const char *an_issue_key)
            :issue_key(std::string(an_issue_key))
        {

        }

        const bool is_empty() const
        {
            return its_project_key.length() == 0;
        }
        const std::string &project_key() const
        {
            return its_project_key;
        }
        const int issue_nr() const
        {
            return its_issue_nr;
        }


        friend std::ostream& operator<<(std::ostream& os, const issue_key& an_issue_key);
        friend bool operator==(const issue_key& lhs, const issue_key& rhs);
        friend bool operator<(const issue_key& lhs, const issue_key& rhs);
};



struct issue_metadata
{
    issue_key key;
    std::string type_name;
    std::string status_id;
    std::string status_name;
    std::string resolution_name;
    bool is_resolved;
    std::string summary;

};

inline std::ostream& operator<<(std::ostream& os, const issue_metadata& m)
{
    os << m.key;

    if(m.is_resolved)
        os << " " << m.resolution_name ;
    else
        os << " " << m.status_name ;


    os << " " << m.type_name
       << " \"" << m.summary << "\"";

}

class issue_key_list : public std::set<issue_key>
{
    public:
        issue_key_list(){}
        issue_key_list(const std::initializer_list<issue_key> &list):std::set<issue_key>(list){}
        issue_key_list(const issue_key_list & an_issue_key_list):std::set<issue_key>(an_issue_key_list){}
        virtual ~issue_key_list(){}
};


std::ostream& operator<<(std::ostream& os, const issue_key& an_issue_key);
std::ostream& operator<<(std::ostream& os, const issue_key_list& issue_key_list);


inline bool operator<(const issue_key& lhs, const issue_key& rhs){ if(lhs.its_project_key == rhs.its_project_key) return lhs.its_issue_nr < rhs.its_issue_nr; else return lhs.its_project_key < rhs.its_project_key;}
inline bool operator==(const issue_key& lhs, const issue_key& rhs){ return lhs.its_issue_nr == rhs.its_issue_nr && lhs.its_project_key == rhs.its_project_key;}
inline bool operator!=(const issue_key& lhs, const issue_key& rhs){return !(lhs == rhs);}

const bool issue_key_has_empty_members(const issue_key_list &issue_keys) ;
const void assert_issue_key_has_no_empty_members(const issue_key_list &issue_keys) ;


}
#endif // JIRA_ISSUE_H
