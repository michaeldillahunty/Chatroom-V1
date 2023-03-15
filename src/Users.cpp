#include "../include/Users.h"

Users::Users(string uid_, string passwd_) {
   uid = uid_;
   passwd = passwd_;
   login_status = false;
}

void Users::set_uid(string uid_){
   uid = uid_;
}

string Users::get_uid(){
   return uid;
}

void Users::set_password(string passwd_) { 
   passwd = passwd_; 
}

string Users::get_password() {
   return passwd;
}

void Users::set_login_status(bool status){
   login_status = status;
}

bool Users::get_login_status(){
   return login_status;
}
