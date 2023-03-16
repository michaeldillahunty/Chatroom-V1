#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

#define SERVER_PORT 14725
#define MAX_LINE 256

using namespace std;
class ClientCommands {
   public: 
      int login(int socket, string& user_id, string& password);
      int create_newuser(int socket, string& username, string& password);
      int send_message(int socket, string& message);
      int logout(int socket);

      /* 
         helper functions
      */
      vector<string>tokenize(const string& s, char delimiter); 
};
#endif