#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#define SERVER_PORT 14725
#define MAX_PENDING 5
#define MAX_LINE 256

#include "../include/Users.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iterator>
#include <cstring>
#include <exception>
#include <stdexcept>

#define MAX_LINE 256
#define USERS_FILE "../users.txt"

using namespace std;

class ServerCommands {
   public: 
      // int recieve_login(int socket, string user_id, string password);
      int recieve_login(int socket, string user_id, string password, Users&user_obj);
      int recieve_newuser(int socket, string& newuser_id, string& new_passwd);
      // int recieve_message(int socket, string& message);
      // int recieve_message(int socket, string& message, Users*user);
      int recieve_message(int socket, vector<string>message_vec, Users&user);
      // int recieve_logout(int socket);
      int recieve_logout(int socket, Users& user);

      /* 
         HELPER FUNCTIONS
      */
      int add_user(string& uid, string& passwd);
      // bool compare_users(const string&filename, string& username, string& password);
      int compare_users(const string&filename, string& username, string& password);
      vector<Users>search_file(const string& filename);
      bool write_user_to_file(const string& filename, Users users_obj);
      vector<string>tokenize(const string& s, char delimiter);
      void print_users(vector<Users> vec);

};


#endif
