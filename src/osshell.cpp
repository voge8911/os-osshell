#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FILE_NAME "history.txt"

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
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);

    // Initialize and open history.txt
    std::vector<std::string> file_lines;
    std::string line;
    std::fstream in_file(FILE_NAME);
    if (!in_file)
    {
        fprintf(stderr, "Error: cannot open file \"%s\"\n", FILE_NAME);
    }
    // read lines from history.txt
    while (std::getline(in_file, line))
    {
        file_lines.push_back(line);
    }
    in_file.close();

    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    // Allocate space for input command lists
    // `command_list` supports up to 32 command line parameters, 
    //     each with a parameter string length of up to 128 characters
    char **command_list;
    char **arg_list;
    char **hist_arg_list;
    allocateArrayOfCharArrays(&command_list, 32, 128);

    struct stat sb;
    int size = 0;
    // put previous commands from file onto command_list
    while (size < file_lines.size())
    {
        strcpy(command_list[size], file_lines[size].c_str());
        size++;
    }
    while(1)
    {
        // support 10 arguments each up to 128 characters long
        allocateArrayOfCharArrays(&arg_list, 10, 128);
        // support 10 arguments for history commands each up to 128 characters long
        allocateArrayOfCharArrays(&hist_arg_list, 10, 128);
        //  Print prompt for user input: "osshell> " (no newline)
        std::string input;
        printf("osshell> ");
        // get user input
        std::getline(std::cin, input);
        // put input into history list
        splitString(input, ' ',hist_arg_list);
        // store first characater of input in variable
        std::string firstChar = input.substr(0, 1);
        //if input is nothing or only whitespace reprompt user
        if (input.empty() || hist_arg_list[0] == NULL)
        {
            continue;
        }
        // Check if the first char of input is a `.` or a `/`
        else if (firstChar.compare(".") == 0 || firstChar.compare("/") == 0)
        {   
            // copy user input to list of commands
            strcpy(command_list[size], input.c_str());
            std::string new_input = input;
            // If input is in the form `./bin/etc...` remove the period so it can be executed
            if (firstChar.compare(".") == 0)
            {
                //remove all periods from input so it can be executed
                while (firstChar.compare(".") == 0)
                {
                    new_input = input.substr(1, input.length()).c_str();
                    firstChar = new_input.substr(0, 1);
                }
            }
            splitString(new_input, ' ', arg_list);
            // Check if command is a PATH to an executable file
            // Run the excecutable if it is, otherwise print error
            if (stat(arg_list[0], &sb) != -1)
            {   
                int pid = fork();
                //child process
                if (pid == 0)
                {
                   execv(arg_list[0], arg_list); 
                }
                //parent process
                else
                {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }
            else
            {
                std::cout << command_list[size] << ": Error command not found \n";
            }
        }
        //  If command is `history` print previous N commands
        else if (strcmp(hist_arg_list[0],"history") == 0 && hist_arg_list[1] == NULL) 
        {
            // copy user input to list of commands
            strcpy(command_list[size], input.c_str());
            for(int i = 0; i < size; i++)
            {
                printf("  %d: %s\n", i + 1, command_list[i]);
            }
        }
        // If input is `history` + ` ` + `argument` check for valid argument
        else if (strcmp(hist_arg_list[0],"history") == 0)
        {
            // If `argument` = `clear` then clear history and don't save to command list
            if (strcmp(hist_arg_list[1],"clear") == 0)
            {
                // set size back to 0
                size = 0;
                // erase contents of history file
                FILE *file;
                file = fopen(FILE_NAME, "w");
                fclose(file);
                continue;
            }
            // If there is some other argument, check if its an integer > 0
            else if (1)
            {   // copy user input to list of commands
                strcpy(command_list[size], input.c_str());
                bool is_character = false;
                int arg;
                char *i;
                
                for (i = hist_arg_list[1]; *i; i++)
                {   // Attempt `argument` to integer conversion
                    try
                    {
                        arg = std::stoi(i);
                    }
                    catch (const std::invalid_argument e)
                    {
                        std::cout << "Error: history expects an integer > 0 (or 'clear')" << "\n";
                        is_character = true;
                        break;
                    }
                }
                // if `argument` is not a character
                if (!is_character)
                {   // convert to integer
                    arg = std::stoi(hist_arg_list[1]);
                    int k;
                    // if `argument` is an integer > 0 print that many of the most recent commands
                    if (arg > 0)
                    {   // do not print the current `history` + `arg` command
                        // if arg is greater than current size print all history
                        if (arg > size)
                        {
                            k = 0;
                        } 
                        else
                        {
                            k = (size - arg);
                        }
                        while (k < size)
                        {
                            printf("  %d: %s\n", k+1, command_list[k]);
                            k++;
                        }
                    }
                    else
                    {
                        std::cout << "Error: history expects an integer > 0 (or 'clear')" << "\n";
                    }
                }
            }
        }
        //  If command is `exit` exit loop / quit program
        else if (strcmp(hist_arg_list[0],"exit") == 0 && hist_arg_list[1] == NULL)
        {   // copy user input to list of commands
            strcpy(command_list[size], input.c_str());
            // write commands to history.txt before exit
            in_file.open(FILE_NAME);
            for (int i = 0; i <= size; i++)
            {
                in_file << command_list[i] << "\n";
            }
            in_file.close();
            printf("\n");
            break; 
        }
        //   For all other commands, check if an executable by that name is in one of the PATH directories
        //   If yes, execute it
        //   If no, print error statement: "<command_name>: Error command not found" (do include newline)
        else
        {   // copy user input to list of commands
            strcpy(command_list[size], input.c_str());
            splitString(input, ' ', arg_list);
            
            char *exec;
            bool is_file = false;
            int i = 0;
            while (os_path_list[i] != NULL)
            {   
                exec = new char[128];
                sprintf(exec, "%s/%s", os_path_list[i], arg_list[0]);
                if (stat(exec, &sb) != -1)
                {
                    is_file = true;
                    break;
                }
                i++;
            }
            if (is_file) 
            {   // fork
                int pid = fork();
                //child process
                if (pid == 0)
                {
                   execv(exec, arg_list); 
                }
                //parent process
                else
                {
                    int status;
                    waitpid(pid, &status, 0);
                }
			}
            else
            {
                std::cout << command_list[size] << ": Error command not found\n";
			}
		}
        // increment counter
        size++;
        // Free allocated memory
        freeArrayOfCharArrays(arg_list, 10);
        freeArrayOfCharArrays(hist_arg_list, 10);
    }
    // Free allocated memory
    freeArrayOfCharArrays(os_path_list, 16);
    freeArrayOfCharArrays(command_list, 32);
    freeArrayOfCharArrays(arg_list, 10);
    freeArrayOfCharArrays(hist_arg_list, 10);
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
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::vector<std::string> list;
    std::string token;
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    list.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    list.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        list.push_back(token);
    }

    for (i = 0; i < list.size(); i++)
    {
        strcpy(result[i], list[i].c_str());
    }
    result[list.size()] = NULL;
}
