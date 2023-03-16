#include "../include/server_commands.h"
#include <cstddef>
#include <exception>
#include <filesystem>

#define UNUSED(x) (void)(x)


void ServerCommands::print_users(vector<Users> vec){
   for (vector<Users>::size_type i = 0; i < vec.size(); ++i) {
      string userID = vec.at(i).get_uid();
      string password = vec.at(i).get_password();
      int status = vec.at(i).get_login_status();
      cout << "User " << i 
      << ": " << userID 
      << " " << password 
      << " Status: " << status << endl;

   }
}

// int ServerCommands::recieve_login(int socket, string user_id, string password){
//    if (socket <= 0) {
//       return -1;
//    }

//    cout << "IN SERVER:\n Parameters: Username = " << user_id << " Password = " << password << endl;
//    // char buffer[MAX_LINE];
//    vector<Users> user_vec;
//    user_vec = search_file(USERS_FILE); // loop through file and store username and password in vector
//    bool user_found = false; // flag to help know when to exit
//    ServerCommands::print_users(user_vec);
//    // for (auto user = begin(user_vec); user != end(user_vec); ++user){ // loop through vector
//    for (auto user : user_vec) {
//       if (user.get_uid().compare(user_id) == 0 && user.get_login_status() == false) {
//          if (user.get_password().compare(password) == 0) {
//                cout << "Updated Login Status: \n Before: " << user.get_login_status();
//                user.set_login_status(true); // update user login status
//                cout << "\n After: " << user.get_login_status() << endl;
//                return 1; // success 
//          }
//          else {    // else if existing password != given password
//                return 2; // return 2 -> incorrect password error
//          }
//       }
//       // else if (user.get_uid().compare(user_id) == 0 && user.get_login_status() == true) {
//       //    return 3; // return 3 -> user with given user_id already logged in
//       // }
//    }

//    if (!user_found) {
//       return -3;
//    }

//    return 1;
// }

int ServerCommands::recieve_login(int socket, string user_id, string password, Users&user_obj){
   if (socket <= 0) {
      return -1;
   }

   // cout << "IN SERVER:\n Parameters: Username = " << user_id << " Password = " << password << endl;
   // char buffer[MAX_LINE];
   vector<Users> user_vec;
   user_vec = search_file(USERS_FILE); // loop through file and store username and password in vector

   user_obj.set_uid(user_id); 
   string obj_uid = user_obj.get_uid();
   user_obj.set_password(password);
   string obj_pass = user_obj.get_password();
   // cout << "OBJ USERNAME: " << obj_uid << " PASSWORD: " << obj_pass << endl;

   bool user_found = false; // flag to help know when to exit
   // ServerCommands::print_users(user_vec);
   // for (auto user = begin(user_vec); user != end(user_vec); ++user){ // loop through vector
   for (auto user : user_vec) {
      if (user.get_uid().compare(obj_uid) == 0 && user_obj.get_login_status() == false) {
         if (user.get_password().compare(obj_pass) == 0) {
               return 1; // success 
         }
         else {    // else if existing password != given password
               return 2; // return 2 -> incorrect password error
         }
      }
      // else if (user.get_uid().compare(user_id) == 0 && user.get_login_status() == true) {
      //    return 3; // return 3 -> user with given user_id already logged in
      // }
   }

   if (!user_found) {
      return 3;
   }

   return 1;

}



int ServerCommands::recieve_newuser(int socket, string& newuser_id, string& new_passwd){
   // cout << "newuser username length: " << newuser_id.length() << "\n";  
   // cout << "Password Length: " << new_passwd.length() << endl;
   if (!socket){
      return -1;
   }
   if (newuser_id.length() < 3 || newuser_id.length() > 32){
      return -2;
   }
   if (new_passwd.length() < 4 || new_passwd.length() > 8){
      return -3; 
   }
   vector<Users>result_vec;
   result_vec = search_file(USERS_FILE);
   int compare_result = compare_users(USERS_FILE, newuser_id, new_passwd);
   if (compare_result == 1 || compare_result == 2){ // if the given user_id matches one already in the file
      return 1; 
   } else {
      Users*new_user = new Users(newuser_id, new_passwd);
      write_user_to_file(USERS_FILE, *new_user);
   }

   return 0;
}

/** searches through file and checks if the given username matches username at current line in file 
      - if no match, write the given user information to file
      - else return error
   \return:
      0 - on error
      1 - if the usernames and passwords both match
      2 - if the usernames match BUT passwords don't
      3 - if the usernames and passwords don't match
      4 - if the usernames DONT match but the passwords do
*/
int ServerCommands::compare_users(const string&filename, string& username, string& password){
   vector<Users>users_vec = search_file(filename);
   for (auto user : users_vec) {
      if (user.get_uid() == username){ 
         // cout << "[Server Commands] Failed to add user " << username << " - User Already Exists!" << endl;
         if (user.get_password() == password){ 
            cout << "passwords match" << endl;
            return 1; // return 1 if the usernames and passwords both match
         } else {
            cout << "passwords don't match" << endl;
            return 2; // return 2 if the usernames match BUT passwords don't
         }
      } 
   }
   
   // error getting users from file
   return 0; 
}

/* 
   Write user to file method ??????
*/
// bool ServerCommands::write_user_to_file(string& filename, string& username, string& password){
//    ofstream ofs(filename, ios::app);
//    if (!ofs){
//       cout << "Failed to open file for writing" << endl;
//       return true;
//    }
//    ofs << "(" << username << ", " << password << ")" << endl;
//    ofs.close();
//    cout << "Successfully Added User: " << username << endl;
//    return false;

// }

vector<Users> ServerCommands::search_file(const string& filename){
   vector<Users> error_vec;
   error_vec = {};
   std::ifstream ifs; 
   ifs.open(filename, ios::in | ios::binary);
   if (!ifs){
      cerr << "Failed to open " << filename << endl;
      return error_vec;
   }

   string item;
   if (ifs.is_open()){
      vector<Users>return_vec;
      while (getline(ifs, item)) {
         item.erase(remove(item.begin(), item.end(), '('), item.end());
         item.erase(remove(item.begin(), item.end(), ' '), item.end());
         stringstream ss(item); 
         string uid;
         getline(ss, uid, ',');
         string pass;
         getline(ss, pass, ')');
         Users*new_user = new Users(uid, pass);
         return_vec.push_back(*new_user);
      }
      return return_vec;
   }   

   return error_vec;
}

int ServerCommands::recieve_message(int socket, vector<string>message_vec, Users&user){
   // UNUSED(socket);
   // UNUSED(message);
   // return 0;
   if (socket <= 0 || message_vec.size() < 1){
      return -1; 
   }
   // cout << "(in server handler)---------USER: " << user.get_uid() << endl;
   // for (auto i = message_vec.begin() + 1; i != message_vec.end(); ++i){
   //    cout << "  OUTPUT: " << *i << endl;
   // }

   string return_msg = "> [from: " + user.get_uid() + "]: "; 
   for (auto word = message_vec.begin() + 1; word != message_vec.end(); ++word){
      return_msg = return_msg + *word + ' ';
   }
   cout << return_msg << endl;
   char ret_buff[MAX_LINE];
   strcpy(ret_buff, return_msg.c_str());
   if (send(socket, ret_buff, sizeof(ret_buff), 0) == -1){
      return -2;
   }

   // cout << "RETURN MESSAGE: " << return_msg << endl;
   return 0;


}

// int ServerCommands::recieve_message(int socket, string& message, Users*user){
//    // UNUSED(socket);
//    // UNUSED(message);
//    // return 0;
//    if (socket <= 0 || message.length() == 0){
//       return -1; 
//    }

//    char buff[MAX_LINE];
//    strcpy(buff, message.c_str());
//    if ((send(socket, buff, sizeof(buff), 0)) == -1) {
//       return -2; 
//    }
//    return 0;


// }

int ServerCommands::recieve_logout(int socket, Users& user){
   string fail_logout;
   if (socket <= 0){
      fail_logout = "[error]: Invalid socket\n";
      return -1; 
   }
   
   if (user.get_login_status() == false){ // if the user is not logged in 
      char buff[MAX_LINE];
      fail_logout = "[error]: No user is logged in\n";
      strcpy(buff, fail_logout.c_str());
      return -2; 
   } else {
      user.set_login_status(false);
      string return_msg;
      char buff[MAX_LINE];
      strcpy(buff, return_msg.c_str());
      if (send(socket, buff, sizeof(buff), 0) == -1){
         return 1; 
      } else {
         return_msg = "> [server] Successfully logged out User: " + user.get_uid();
         cout << "> Successfully logged out User: " << user.get_uid() << endl;
      }
   }
   return 0; 
}

vector<string>ServerCommands::tokenize(const string& s, char delimiter){
   vector<string>result;
   stringstream ss(s); 
   string curr; 
   while (getline(ss, curr, delimiter)){ result.push_back(curr); }

   return result; 
}

bool ServerCommands::write_user_to_file(const string& filename, Users new_user_obj){
   ofstream ofs(filename, ios::app);
   if (!ofs){
      cout << "Failed to open file for writing" << endl;
      return false;
   }
   ofs << "(" << new_user_obj.get_uid() << ", " << new_user_obj.get_password() << ")" << endl;
   ofs.close();
   cout << "Successfully Added User: " << new_user_obj.get_uid() << endl;
   return true;
}

   