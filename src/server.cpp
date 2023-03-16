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

   


   // bool logged_in = false; 

/***** while 1 *****/
   while(1){
      cout << "[Server] > Server-Client Connection Successfully Established!\n"
      << "     [Server Connected on Port #" << SERVER_PORT << "]"
      << "\n------------------------------------------------------------" << endl;
      int client_sock_fd;
      if ((client_sock_fd = accept(server_sock_fd, NULL, NULL)) == -1) {
         cout << "[ERROR] Failed to accept client connection" << endl;
         continue;
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
      // logged_in = false;
/* while 2*/      
      while(1){
         ssize_t recieved = recv(client_sock_fd, buffer, sizeof(buffer), 0); 
         if (recieved < 0){
            cout << "[ERROR] Connection with client lost\n> Closing socket...." << endl;
            close(client_sock_fd);
            break;
            // continue;
         }

         /* 
            Copying message from client into a string obj
         */
         stringstream copy_ss; 
         string message_str;
         copy_ss << buffer;
         message_str = copy_ss.str();
         // cout << "message_str: " << message_str << endl;

         ServerCommands sc;
         vector<string>tokens_vec; 
         tokens_vec = sc.tokenize(message_str, ' '); 

         
         // for (vector<string>::size_type i = 0; i < tokens_vec.size(); i++){
         //    cout << "at " << i << " " << tokens_vec.at(i) << "\n";
         // }


         try {
   /************************************ 
            COMMAND: send
   ************************************/
            if (tokens_vec.at(0) == "login") {
               // string user_id = curr_user.get_uid();
               string user_id = tokens_vec.at(1);
               cout << user_id << endl;
               string login_fail_msg;
               int server_login = sc.recieve_login(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2), curr_user);
               // cout << "--- IN SERVER MAIN: user status = " << curr_user.get_login_status() << endl;
               if (curr_user.get_login_status() == false) {
                  if (server_login == -1){
                     login_fail_msg = "[ERROR] Login Failed - Failed to connect user";
                     char temp_buff[MAX_LINE];
                     strcpy(temp_buff, login_fail_msg.c_str());
                     send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                     cout << login_fail_msg << endl;
                     // throw invalid_argument(login_fail_msg);
                  } else if (server_login == 2) {
                     login_fail_msg = "[ERROR] Login Failed - Password is incorrect - try again";
                     char temp_buff[MAX_LINE];
                     strcpy(temp_buff, login_fail_msg.c_str());
                     send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                     cout << login_fail_msg << endl;
                     // throw invalid_argument(login_fail_msg);
                  } else if (server_login == 3) {
                     login_fail_msg = "[ERROR] Login Failed - User does not exist";
                     char temp_buff[MAX_LINE];
                     strcpy(temp_buff, login_fail_msg.c_str());
                     send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                     cout << login_fail_msg << endl;
                     // throw CustomException(login_fail_msg.c_str());
                  } 
                  else {
                     // cout << "STATUS 2 BEFORE: " << curr_user.get_login_status() << endl;
                     curr_user.set_login_status(true);
                     // cout << "STATUS 2 AFTER: " << curr_user.get_login_status() << endl;
                     string login_success = "[server]: Successfully logged-in " + tokens_vec.at(1) + "!";
                     cout << login_success << endl;
                     strcpy(buffer, login_success.c_str());
                     send(client_sock_fd, buffer, sizeof(buffer), 0);

                  }
                  // cout << "Login Success! User " << tokens_vec.at(1) << " signed in" << endl;
               } else { // 
                  // cout << "STATUS 3 STATUS = TRUE : " << curr_user.get_login_status() << endl;
                  string success_msg_tst = "User already logged in\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, success_msg_tst.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
               }


   /************************************ 
            COMMAND: newuser
   ************************************/
            } else if (tokens_vec.at(0) == "newuser") {
               string newuser_fail_msg;
               int server_newuser = sc.recieve_newuser(client_sock_fd, tokens_vec.at(1), tokens_vec.at(2));
               if (server_newuser == -1) {
                  newuser_fail_msg = "> [ERROR] Invalid arguments - (expected input format: <newuser> <username> <password>)\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, newuser_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  cout << newuser_fail_msg << endl;
                  // throw invalid_argument();
               } else if (server_newuser == -2) {
                  newuser_fail_msg = "> [ERROR] Invalid username length : expected between 3-32 characters\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, newuser_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  cout << newuser_fail_msg << endl;
                  // throw invalid_argument();
               } else if (server_newuser == -3) {
                  newuser_fail_msg = "> [ERROR] Invalid username length : expected between 3-32 characters\n";
                  char temp_buff[MAX_LINE];
                  newuser_fail_msg = "> [ERROR] Invalid password length : expected between 4-8 characters\n"; 
                  strcpy(temp_buff, newuser_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  cout << newuser_fail_msg << endl;
                  
                  // throw invalid_argument();
               } else if (server_newuser == 1) {
                  string user_id_error = tokens_vec.at(1).c_str();
                  newuser_fail_msg = "[ERROR] Failed to create new user (User '" + user_id_error + "' already exists)\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, newuser_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  cout << newuser_fail_msg;

                  // throw CustomException(error_msg1.c_str());
               } 
               char temp_buff[MAX_LINE];
               string newuser_message = "[server] > successfully created new user (" + curr_user.get_uid() + ") - Please Login\n";
               strcpy(temp_buff, newuser_message.c_str());
               send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
               
   /************************************ 
            COMMAND: send
   ************************************/
            } else if (tokens_vec.at(0) == "send") {
               string send_fail_msg;
               
               if (tokens_vec.size() <= 1){
                  send_fail_msg = "[server] > [Error]: Invalid command usage\n";
                  char temp_buff[MAX_LINE];
                  strcpy(temp_buff, send_fail_msg.c_str());
                  send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  cout << send_fail_msg; 
               }

               // MUST BE LOGGED IN TO SEND A MESSAGE
               if (curr_user.get_login_status() == true) {  /* IF THE USER IS LOGGED IN */
                  int server_send = sc.recieve_message(client_sock_fd, tokens_vec, curr_user);
                  if (server_send == 0){
                     cout << "> Successfully returned message to client" << endl;
                     continue; 
                  } else if (server_send == -1) {
                     send_fail_msg = "[error]: Invalid socket - could not send message\n";
                     char temp_buff[MAX_LINE];
                     strcpy(temp_buff, send_fail_msg.c_str());
                     send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);

                  } else if (server_send == -2) {
                     send_fail_msg = "[error]: failed to send return message to client\n";
                     char temp_buff[MAX_LINE];
                     strcpy(temp_buff, send_fail_msg.c_str());
                     send(client_sock_fd, temp_buff, sizeof(temp_buff), 0);
                  }
               } else { /* ELSE IF THE USER ISN'T LOGGED IN */
                  string failed_send = "[ERROR] Must be logged in to send a message!";
                  strcpy(buffer, failed_send.c_str());
                  send(client_sock_fd, buffer, sizeof(buffer), 0);
               }

            /****************** 
               COMMAND: logout
            ******************/
            } else if (tokens_vec.at(0).compare("logout")) {
               int server_logout = sc.recieve_logout(client_sock_fd, curr_user);
               if (server_logout == -1) {
                  return server_logout;
               } else if (server_logout == -2) {
                  cout << "> No users logged in - nothing to log out";
                  return -1;
               } else if (server_logout == 1) {
                  cout << "> [server]: Failed to send return message to server: " << curr_user.get_uid() << endl;
                  return 1;
               } else if (server_logout == 0) {
                  cout << "> Logging (user: " << curr_user.get_uid() << ") out" << endl;
                  return 0;
               }
            }
            
         } catch (CustomException &e){ // handle thrown exceptions for custom errors
            cout << e.what() << endl;
         } catch (invalid_argument& e) { // handle invalid argument exceptions for invalid commands / usage 
            cout << e.what() << endl;
         } catch (exception& e) {  // handle 'out_of_range' vector exception
            /*
               This catch is printing 'vector' 
               -> don't print out-of-range exceptions
            */
            // cout << e.what() << endl;
         }
         // return 0;
      }

   }
   return 0; 
}


