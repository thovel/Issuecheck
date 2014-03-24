#include "jira_query.h"
#include "sstream"

void jira::issue_query::print_to(std::ostream& os) const
//void jira::issue_query::print_to(std::ostream& os) const
{
    bool first=true;
    for (issue_query::iterator it = this->begin() ; it != this->end(); ++it)
    {
        if(!first)
            os << " OR ";
        else
            first=false;

        os << "key=" << *it;
    }

}
std::ostream& jira::operator<<(std::ostream& os, const jira::query &a_query)
{
    a_query.print_to(os);
    return os;
}


