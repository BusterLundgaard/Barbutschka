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

class IllegalMultipleRequest : public std::exception {
private:
    std::string message;
public: 
    explicit IllegalMultipleRequest(std::string component_name, int16_t id) : message("Tried to request multiple of non-multiple component of type " + component_name + " to entity with id " + std::to_string(id) + ".") {};
};

class IllegalMultipleAdd : public std::exception {
private:
    std::string message; 
public: 
    explicit IllegalMultipleAdd(int16_t id) : message("Tried to add the component with comp-id " + std::to_string(id) + " which has already been added!") {};
};

class IllegalMaxComponents : public std::exception {
private:
std::string message;
public:
    explicit IllegalMaxComponents(std::string component_name, int max) : message("Tried to add more than " + std::to_string(max) + " components of type " + component_name + ", that is above the max allowed elements for this component!") {};
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

class RemoveNonExistingElement : public std::exception {
private:
    std::string message; 
public: 
    explicit RemoveNonExistingElement(int16_t comp_id) : message("Cannot remove non-existing component with id " + std::to_string(comp_id) + ".") {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif
