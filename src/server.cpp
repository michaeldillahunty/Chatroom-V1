#include "../include/server_commands.h"
#include "../include/Users.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <sys/socket.h>
#include <string.h>

/** Notes 
 * Login: 
 * - Verify userid and password -> if not decline and send error message to client
 *    - send confirmation to client 
 * 
 * newuser: 
 * - UserID and password need to be case-sensitive 
 *    - reject request if userID already exists 
 * - Store userid and passwords in .txt file (on server side)
 * 
 * send:
 * - precede message from client with userID -> send back to client 
 *    - message size <= 256 chars
 * 
 * logout:
 * - close connection between client
 *    -> STAY RUNNING to allow for other clients to connect
 * 
 */

 struct CustomException : public std::exception {
   string message;
   CustomException(const char* msg) : message(msg) {}

   const char* what() const noexcept override {
      return message.c_str();
   }
};

int main(){
   cout << "\n------------------- [Server] Chatroom V1 -------------------" << endl;
   vector<Users>users_vec;
   string filename = USERS_FILE;

   int server_sock_fd;
   if ((server_sock_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0){
      cout << "[ERROR] Failed to create server socket" << endl;
      return -1;
   }

   // Reference: https://hea-www.harvard.edu/~fine/Tech/addrinuse.html
   // Purpose: If the server was closed first, would have to wait a few seconds before rebinding the address
   int optval = 1;
   if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))){
      cout << "[ERROR] Unable to reuse address" << endl;
      return -1; 
   }

   struct sockaddr_in server_addrs;
   server_addrs.sin_family = AF_INET;
   server_addrs.sin_port = htons(SERVER_PORT);
   server_addrs.sin_addr.s_addr = INADDR_ANY;

   if (::bind(server_sock_fd, (struct sockaddr*)&server_addrs, sizeof(server_addrs)) != 0){
      cout << "[ERROR] Failed to bind server to socket" << endl;
      return -1;
   }

   if (listen(server_sock_fd, MAX_PENDING) < 0){
      cout << "[ERROR] Error listening for socket connections" << endl;
      return -1;
   }

   

//   bool status = false; // if the current user is logged in or not
   bool logged_in = false; 

/***** while 1 *****/
   while(1){
      cout << "[Server] > Server-Client Connection Successfully Established!\n"
            << "     [Server Connected on Port #" << SERVER_PORT << "]"
            << "\n------------------------------------------------------------" << endl;
      int client_sock_fd;
      if ((client_sock_fd = accept(server_sock_fd, NULL, NULL)) < 0) {
         cout << "[ERROR] Failed to accept client connection" << endl;
         return -1; 
      }
      cout << "Successfully Connected to Client" << endl; 
      
      // Users*curr_user = nullptr;
      // curr_user = new Users();
      // if (curr_user == nullptr) {
      //    cout << "[ERROR] error getting user\n";
      // }

      char test_msg[MAX_LINE] = "Successfully connected to server!";
      if (send(client_sock_fd, test_msg, sizeof(test_msg), 0) < 0){
         cout << "[ERROR] Failed to send initial test message from server->client" << endl;
         return -1;
      }

      Users curr_user;  // create a user, don't allocate anything yet 
      curr_user.set_login_status(false);
      char buffer[MAX_LINE];
      logged_in = false;
/* while 2*/      
      while(1){
         ssize_t recieved = recv(client_sock_fd, buffer, sizeof(buffer), 0); 
         if (recieved <= 0){
            cout << "[ERROR] Connection with client lost\n> Closing socket...." << endl;
            close(client_sock_fd);
            break;
         }
         // cout << "[from: " << curr_user->get_uid() << "]: " << buffer << endl;
         cout << "[From Client]: " << buffer << endl;

         /* 
            Copying message from client into a string obj
         */
         stringstream copy_ss; 
         string message_str;
         copy_ss << buffer;
         message_str = copy_ss.str();
         cout << "message_str: " << message_str << endl;

         ServerCommands sc;
         vector<string>tokens_vec; 
         tokens_vec = sc.tokenize(message_str, ' '); 

         
         for (vector<string>::size_type i = 0; i < tokens_vec.size(); i++){
            cout << "at " << i << " " << tokens_vec.at(i) << "\n";
         }

         /* commands */   
         try {
            string login_fail_msg;
            if (tokens_vec.at(0) == "login") {
               // if (tokens_vec.size() != 3) {
               //    return -1; 
               // }
               int server_login = sc.recieve_login(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2), curr_user);
               cout << "--- IN SERVER MAIN: user status = " << curr_user.get_login_status() << endl;
               if (curr_user.get_login_status() == false) {
               if (server_login == -1){
                  login_fail_msg = "[ERROR] Login Failed - Failed to connect user";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, login_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  throw invalid_argument(login_fail_msg);
               } else if (server_login == 2) {
                  login_fail_msg = "[ERROR] Login Failed - Password is incorrect - try again";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, login_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  throw invalid_argument(login_fail_msg);
               } else if (server_login == 3) {
                  login_fail_msg = "[ERROR] Login Failed - User does not exist";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, login_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  throw CustomException(login_fail_msg.c_str());
               } else {
                  cout << "STATUS 2: " << curr_user.get_login_status() << endl;
                  curr_user.set_login_status(true);
                  string login_success = "[server]: Successfully logged-in " + tokens_vec.at(1) + "!";
                  strcpy(buffer, login_success.c_str());
                  cout << "BUFFER LOGIN MESSAGE: " << buffer << endl;
                  send(client_sock_fd, buffer, sizeof(buffer), 0);
               }
               cout << "Login Success! User " << tokens_vec.at(1) << " signed in" << endl;
               } else {
                  string success_msg_tst = "User already logged in\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, success_msg_tst.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);

               }
            } else if (tokens_vec.at(0) == "newuser") {
               int server_newuser = sc.recieve_newuser(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
               if (server_newuser == -1) {
                  throw invalid_argument("[ERROR] Invalid arguments\n(expected format: <newuser> <username> <password>)\n");
               } else if (server_newuser == -2) {
                  throw invalid_argument("[ERROR] Invalid username length : expected between 3-32 characters\n");
               } else if (server_newuser == -3) {
                  throw invalid_argument("[ERROR] Invalid password length : expected between 4-8 characters\n");
               } else if (server_newuser == 1) {
                  string user_id_error = tokens_vec.at(1).c_str();
                  string error_msg1 = "[ERROR] Failed to create new user (User '" + user_id_error + "' already exists)";
                  throw CustomException(error_msg1.c_str());
               } 
               
               

            } else if (tokens_vec.at(0) == "message") {
               // MUST BE LOGGED IN TO SEND A MESSAGE
               if (logged_in) { 
                  /* 
                  
                     DO SOMETHING

                  */
               } else { // IF NOT LOGGED IN
                  string failed_send = "[ERROR] Must be logged in to send a message!";
                  strcpy(buffer, failed_send.c_str());
                  send(client_sock_fd, buffer, sizeof(buffer), 0);
               }

            } else if (tokens_vec.at(0) == "logout") {
               // MUST BE LOGGED IN TO LOG OUT 
               if (!logged_in){
                  string failed_send = "[ERROR] Can't do that! Must be logged in to log out!";
                  strcpy(buffer, failed_send.c_str());
                  send(client_sock_fd, buffer, sizeof(buffer), 0);
               } else {
                  /*

                     DO SOMEHING

                  */
               }

            } else {

            }
            
         } catch (CustomException &e){ // handle thrown exceptions for custom errors
            cout << e.what() << endl;
         } catch (invalid_argument& e) { // handle invalid argument exceptions for invalid commands / usage 
            cout << e.what() << endl;
         } catch (exception& e) {  // handle other possible excpetions that may have been missed such as vector errors
            cout << e.what() << endl;
         } 

      }

   }

   return EXIT_SUCCESS; 
}


