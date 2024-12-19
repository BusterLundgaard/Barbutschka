#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class IllegalMultipleComponents : public std::exception {
private:
    std::string message;
public:
    explicit IllegalMultipleComponents(std::string component_name, int16_t id) : message("Tried to add multiple of non-multiple component of type " + component_name + " to entity with id " + std::to_string(id) + ".") {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

class NullPtrException : public std::exception {
private:
    std::string message;
public:
    explicit NullPtrException(std::string msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif
