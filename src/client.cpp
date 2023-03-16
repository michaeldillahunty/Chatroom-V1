#include "../include/client_commands.h"
#include <cstdlib>
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

   char message_buff[MAX_LINE];
   if (recv(client_sock_fd, message_buff, sizeof(message_buff), 0) <= 0){
      cout << "[error] failed to retrieve initial message from socket" << endl;
      return -1;
   } else {
      message_buff[sizeof(message_buff) - 1] = '\0';
      cout << "[Initial Message]: " << message_buff << "\n";
   }


   char input_arr[MAX_LINE];
   while(1){
      cout << "\n> ";
      cin.getline(input_arr, MAX_LINE, '\n');
      
      string input_copy = input_arr;
      // char*tokens = input_arr; 

      vector<string> tokens_vec;
      tokens_vec = cc.tokenize(input_arr, ' '); // tokenize input string using white space as delimiter
      
      // for (vector<string>::size_type i = 0; i < tokens_vec.size(); i++){
      //    cout << "at " << i << " " << tokens_vec.at(i) << "\n";
      // }

      // char recv_response[MAX_LINE];
      // int recieved = recv(client_sock_fd, recv_response, sizeof(recv_response), 0);
      // if (recieved != 1){
      //    cout << "Failed to recieve message from server" << endl;
      // }

      try {
         
         /****************** 
            COMMAND: login
         ******************/
         if (tokens_vec.at(0) == "login") {
            if (tokens_vec.size() != 3){
               throw invalid_argument("[ERROR] command <login> requires 3 arguments in the format <login> <username> <password>");
            }
            // cout << recv_response << endl;
            int client_login = cc.login(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
            if (client_login == -1){
               cout << "[error] Could not connect client socket to server" << endl;
            } 

            char response_buff[MAX_LINE];
            int recieve_response = recv(client_sock_fd, response_buff, sizeof(response_buff), 0);
            if (recieve_response == -1){
               cout << "Failed to Recieve Response from Server" << endl;
            } else {
               cout << response_buff;
            }

         /****************** 
            COMMAND: newuser
         ******************/
         } else if (tokens_vec.at(0) == "newuser") {
            if (tokens_vec.size() != 3) {
               throw invalid_argument("[ERROR] command <newuser> requires 3 arguments in the format <command> <username> <password>");
            }
            int client_newuser = cc.create_newuser(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
            if (client_newuser == -1) {
               cout << "[ERROR] Invalid command usage - try again\n";
            } else if (client_newuser == -2) {
               cout << "[ERROR] Invalid UserID Length - should be between 3-32 characters\n";
            } else if (client_newuser == -3) {
               cout << "[ERROR] Invalid Password Length - should be between 4-8 characters\n";
            } else {
               cout << "> New user " << tokens_vec.at(1) << " created - Please login.\n";
            }
            // else send command message to server

         /****************** 
            COMMAND: message
         ******************/
         } else if (tokens_vec.at(0) == "send") {
            // cout << "INPUT COPY : " << input_copy << endl;
            if (tokens_vec.size() <= 1) {
               cout << "[error] invalid command usage - expected format: <send> 'your message to send'" << endl;
            }
            int client_send = cc.send_message(client_sock_fd, input_copy);
            if (client_send == -1){
               cout << "[error] Failed to send message - Invalid socket" << endl;
            } else if (client_send == -2){
               cout << "[error]: Failed to send message" << endl;
            } else if (client_send == 0) {
               char response_buff[MAX_LINE];
               int rec_response = recv(client_sock_fd, response_buff, sizeof(response_buff), 0);
               if (rec_response <= 0){
                  cout << "[error]: Failed to retrieve response from server" << endl;
               }
               cout << "[server] " << response_buff << endl;
            }



         /****************** 
            COMMAND: logout
         ******************/
         } else if (tokens_vec.at(0) == "logout") {
            cout << "> Logging out..." << endl;
            // int client_logout = cc.logout(client_sock_fd);
            char temp_buffer[MAX_LINE];
            strcpy(temp_buffer, tokens_vec.at(0).c_str());
            cout << "Logout temp_buffer = " << temp_buffer << endl;
            if (send(client_sock_fd, temp_buffer, sizeof(temp_buffer), 0) < 0) {
               cout << "An error occured trying to logout." << endl;
               return -1;
            }
            // send(client_sock_fd, temp_buffer, sizeof(temp_buffer), 0);
            return 0;      

         } else {
            throw invalid_argument("[ERROR] Invalid Command\n  available commands {login, newuser, message, logout}\n");
         }
      } catch(invalid_argument& e) {
         cout << e.what() << endl;
      } catch(exception& e) {
         cout << e.what() << endl;
      }


      

   }
   close(client_sock_fd);
   return 0; 
}


