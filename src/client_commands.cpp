#include "../include/client_commands.h"

#define UNUSED(x) (void)(x)

int ClientCommands::login(int socket, string& user_id, string& password){
   if (socket <= 0){
      return -1;
   }
   if (user_id.length() < 3 || user_id.length() > 32){
      return -2; // return -2 for invalid user id 
   }

   if (password.length() < 4 || password.length() > 8){
      return -3; // return -3 for invalid password
   }

   char buffer[MAX_LINE];
   string message = "login " + user_id + " " + password;
   strcpy(buffer, message.c_str());
   // cout << "IN CLIENT COMMANDS: " << user_id << " & " << password << endl;
   

   return send(socket, buffer, sizeof(buffer), 0);
}

int ClientCommands::create_newuser(int socket, string& user_id, string& password){
   // cout << "in client commands -- username: " << user_id << " password: " << password << endl;
   // cout << user_id.length() << " & " << password.length() << endl;
   if (socket > 0 && user_id.length() > 0 && password.length() > 0){ // check that the parameters exist
      if (user_id.length() < 3 || user_id.length() > 32){
         return -2; // return -2 for invalid user id 
      }

      if (password.length() < 4 || password.length() > 8){
         return -3; // return -3 for invalid password
      }
      // string return_str = "New user '" + user_id + "' created";
      string return_str = "newuser " + user_id + " " + password;
      char buff[MAX_LINE];
      strcpy(buff, return_str.c_str());
      
      // cout << return_str;
      return send(socket, buff, sizeof(buff), 0);
   }
   return -1; // -1 on param fail
}

int ClientCommands::send_message(int socket, string&message){ 
   if (socket <= 0) {
      return -1; 
   }

   // cout << "IN HANDLER FUNCTION - message: " << message << endl;
   char buff[MAX_LINE];
   strcpy(buff, message.c_str());
   if (send(socket, buff, sizeof(buff), 0) == -1){
      return -2;
   }
   return 0;
}

int ClientCommands::logout(int socket){
   if (socket < 0){
      return -1;
   }
   return 0;
}

/* Reference for tokenizing a string into a vector
   - https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring 
*/
vector<string>ClientCommands::tokenize(const string& s, char delimiter){
   vector<string>result;
   stringstream ss(s); 
   string curr; 
   while (getline(ss, curr, delimiter)){ result.push_back(curr); }

   return result; 
}