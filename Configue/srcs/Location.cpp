#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"



std::string Location::get_path(){
    return this->path;
}

std::string Location::get_fastcgi_pass(){
    return this->fastcgi_pass;
}

std::string Location::get_location_root(){
    return this->root;
}

std::string Location::get_expires(){
    return this->expires;
}

void Location::set_path(std::string value){
    this->path = value;
}

void Location::set_fastcgi_pass(std::string value){
    this->fastcgi_pass = value;
}

void Location::set_location_root(std::string value){
    this->root = value;
}

void Location::set_expires(std::string value){
    this->expires = value;
}