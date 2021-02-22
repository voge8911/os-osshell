#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size);
void freeArrayOfCharArrays(char **array, size_t array_length);
void splitString(std::string text, char d, char **result);

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    // `os_path_list` supports up to 16 directories in PATH, 
    //     each with a directory name length of up to 64 characters
    char **os_path_list;
    allocateArrayOfCharArrays(&os_path_list, 16, 64);
    //char* os_path = getenv("PATH");
    //splitString(os_path, ':', os_path_list);


    // Example code for how to loop over NULL terminated list of strings
    /*
    int i = 0;
    while (os_path_list[i] != NULL)
    {
        printf("PATH[%2d]: %s\n", i, os_path_list[i]);
        i++;
    }
    */

    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    // Allocate space for input command lists
    // `command_list` supports up to 32 command line parameters, 
    //     each with a parameter string length of up to 128 characters
    char **command_list;
    allocateArrayOfCharArrays(&command_list, 32, 128);

    // Repeat:cd 
    int j = 0;
    while(1)
    {
        //  Print prompt for user input: "osshell> " (no newline)

        std::cout << "osshell> ";
        
        fgets(command_list[j], 128, stdin);
        
        //splitString(user_input, ' ', command_list); //split up userinput by spaces
        //std::cout << "Command list at j : " << command_list[j] << std::endl;
        
        if (strcmp(command_list[j],"\n") == 0)
        {
            continue;
        }
        
        printf("here 1 %s\n", command_list[j]);
        //  If command is `exit` exit loop / quit program
        if (strcmp(command_list[j],"exit") == 0){
            break; //exit the loop
        }
        //  If command is `history` print previous N commands
        if (strcmp(command_list[j],"history")==0){
            for(int i = 0; i < j; i++)
            {
                std::cout << i << ": " << command_list[i] << std::endl;
            }
        }else{
            //[]; run getenv command here
            if(getenv(command_list[0]) == NULL) {
				std::cout << command_list[0] << ": Error command not found \n";
			}else
                {
                    
                    //std::cout << execv(os_path_list, command_list);
				}
		}
        //  For all other commands, check if an executable by that name is in one of the PATH directories
        //   If yes, execute it
        //   If no, print error statement: "<command_name>: Error command not found" (do include newline)



        j++;
    }
    // Free allocated memory
    freeArrayOfCharArrays(os_path_list, 16);
    freeArrayOfCharArrays(command_list, 32);

    return 0;
    
}

/*
   array_ptr: pointer to list of strings to be allocated
   array_length: number of strings to allocate space for in the list
   item_size: length of each string to allocate space for
*/
void allocateArrayOfCharArrays(char ***array_ptr, size_t array_length, size_t item_size)
{
    int i;
    *array_ptr = new char*[array_length];
    for (i = 0; i < array_length; i++)
    {
        (*array_ptr)[i] = new char[item_size];
    }
}

/*
   array: list of strings to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        delete[] array[i];
    }
    delete[] array;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: NULL terminated list of strings (char **) - result will be stored here
*/
void splitString(std::string text, char d, char **result)
{
    int i;
    std::vector<std::string> list;
    std::stringstream ss(text);
    std::string token;
    
    while (std::getline(ss, token, d))
    {
        list.push_back(token);
    }

    for (i = 0; i < list.size(); i++)
    {
        strcpy(result[i], list[i].c_str());
    }
    result[list.size()] = NULL;
}
