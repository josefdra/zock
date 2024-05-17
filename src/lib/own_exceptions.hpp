#ifndef OWN_EXCEPTIONS_H_
#define OWN_EXCEPTIONS_H_

#include <exception>
#include <string>

class TimeoutException : public std::exception
{
private:
    std::string message;

public:
    TimeoutException(const std::string &msg) : message(msg) {}

    virtual const char *what() const throw()
    {
        return message.c_str();
    }
};

#endif // OWN_EXCEPTIONS_H_