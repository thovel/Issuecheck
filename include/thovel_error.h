#ifndef THOVEL_ERROR_H
#define THOVEL_ERROR_H

#include <string>

namespace thovel{

inline std::string &&assemble_error(const char *message, const char * file, const char *func, const int line){
    std::string *ret {new std::string()};
    ret->append(message);
    ret->append(" [");
    ret->append(file);
    ret->append(" ");
    ret->append(func);
    ret->append("() l:");
    ret->append(std::to_string(line));
    ret->append("]");
    return std::move(*ret);
}
}
#define THOVEL_MESSAGE(message) thovel::assemble_error(message ,__FILE__ , __FUNCTION__ , __LINE__)


#endif // THOVEL_ERROR_H
