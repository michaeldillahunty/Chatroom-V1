#ifndef USERS_H
#define USERS_H
#include <string>
#include <iostream>


using namespace std; 



class Users {
   string uid; 
   string passwd; 
   bool login_status;
   public:
      void set_uid(string uid_);
      string get_uid();
      void set_password(string passwd_); 
      string get_password();
      void set_login_status(bool status);
      bool get_login_status();
      Users(){}
      Users(string uid_, string passwd_);
};

#endif