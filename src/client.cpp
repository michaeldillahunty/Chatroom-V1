#include "../include/client_commands.h"
#include <exception>
#include <stdexcept>

int main(){
   ClientCommands cc; 

   cout << "------------------- [Client] Chatroom V1 -------------------" << endl;
   int client_sock_fd;
   if ((client_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      cout << ("[error] failed to create socket") << endl;
      return -1; 
   }

   struct sockaddr_in server_addrs;
   server_addrs.sin_family = AF_INET;
   server_addrs.sin_port = htons(SERVER_PORT);
   server_addrs.sin_addr.s_addr = INADDR_ANY;

   if (connect(client_sock_fd, (struct sockaddr*)&server_addrs, sizeof(server_addrs)) == -1){
      cout << "[ERROR] Failed to connect client to socket" << endl;
      return -1;
   }

   char initial_msg[MAX_LINE];
   if (recv(client_sock_fd, initial_msg, sizeof(initial_msg), 0) <= 0){
      cout << "[error] failed to retrieve initial message from socket" << endl;
      return -1;
   } else {
      initial_msg[sizeof(initial_msg) - 1] = '\0';
      cout << "[Initial Message]: " << initial_msg << "\n";
   }

   bool w1 = false;
   char input_arr[MAX_LINE];
   while(!w1){
      cout << "\n> ";
      cin.getline(input_arr, MAX_LINE, '\n');
      
      string input_copy = input_arr;
      // char*tokens = input_arr; 

      vector<string> tokens_vec;
      tokens_vec = cc.tokenize(input_arr, ' '); // tokenize input string using white space as delimiter
      
      for (vector<string>::size_type i = 0; i < tokens_vec.size(); i++){
         cout << "at " << i << " " << tokens_vec.at(i) << "\n";
      }

      try {
         if (tokens_vec.at(0) == "login") {
            if (tokens_vec.size() != 3){
               throw invalid_argument("[ERROR] command <login> requires 3 arguments in the format <login> <username> <password>");
            }
            int client_login = cc.login(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
            if (client_login == -1){
               cout << "LOGIN ERROR" << endl;
            }
            char response_buff[MAX_LINE];
            int recieve_response = recv(client_sock_fd, response_buff, sizeof(response_buff), 0);
            if (recieve_response == -1){
               cout << "Failed to Recieve Response from Server" << endl;
            } else {
               cout << response_buff;
            }

         } else if (tokens_vec.at(0) == "newuser") {
            if (tokens_vec.size() != 3) {
               throw invalid_argument("[ERROR] command <newuser> requires 3 arguments in the format <command> <username> <password>");
            }
            int client_newuser = cc.create_newuser(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
            if (client_newuser == -1) {

            }

         } else if (tokens_vec.at(0) == "message") {

         } else if (tokens_vec.at(0) == "logout") {

         } else {
            throw invalid_argument("[ERROR] Invalid Command\n  available commands {login, newuser, message, logout}\n");
         }
      } catch(invalid_argument& e) {
         cout << e.what() << endl;
      } catch(exception& e) {
         cout << e.what() << endl;
      }


      

   }

   return 1; 
}


