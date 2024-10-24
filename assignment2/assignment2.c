#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

// structure for the movies
struct movie {
    char* title;
    int year;
    char languages[5][20];
    double rating;
    struct movie*  next;
};

//Parses the current line and creates a movie struct with the data in the line 
struct movie* createMovie(char* currLine) {

    struct movie* currMovie = malloc(sizeof(struct movie));

    //initalizes the languages to empty strings
    for(int i = 0; i < 5; i++) {
        currMovie->languages[i][0] = '\0'; //set each language string to an empty string
    }
    //For use with strtok_r
    char* saveptr;
    char* langsaveptr;

    //the first token is the title
    char* token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    //the next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    //finds where the brackets are and removes them from the search for languages
    token = strtok_r(NULL, ",", &saveptr);
    char* start = strchr(token, '[');
    char* end = strchr(token, ']');
    *end = '\0';
    start++;


    //keeps track of how many languages there are
    int count = 0;

    //parses through the language line token and adding them into the language array
    char * langtoken = strtok_r(start, ";", &langsaveptr);
    while(langtoken != NULL) {
        strncpy(currMovie->languages[count], langtoken, 20);
        currMovie->languages[count][19]= '\0';
        count++;
        langtoken = strtok_r(NULL, ";,", &langsaveptr);
    }

    //for use with strtod
    char* endptr;

    //the last token is rating
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = strtod(token, &endptr);

    //sets the next node to NULL
    currMovie->next = NULL;
    
    return currMovie;
}

//frees all the data saved in the linked list
void freeMovie(struct movie* head) {
    //goes through each node in the list and clears each one of them
    while(head != NULL) {
        struct movie* temp = head;
        head = head->next;
        free(temp->title);
        free(temp);
    }
}

//return a linked list of students by parsing data from each line of the specified path
struct movie* processFile(char* filePath) {

    //Open the specficed file for reading only
    FILE* movieFile = fopen(filePath, "r");

    //setup variables 
    char* currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char* token; 

    //skips the first line 
    nread = getline(&currLine, &len, movieFile);

    //handles error or empty file
    if(nread == -1) {
        free(currLine);
        fclose(movieFile);
        return NULL;
    }

    //the head of the linked list
    struct movie* head = NULL;
    //the tail of the linked list 
    struct movie* tail = NULL; 

    //Read the file line by line 
    while((nread = getline(&currLine , &len, movieFile)) != -1) {

        //creates a new movie node with the current line
        struct movie* newNode = createMovie(currLine);

        //If first node, declares head and tail
        if(head == NULL) {
            head = newNode;
            tail = newNode;
        }

        //adds this node to the end of the linked list
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    free(currLine);
    fclose(movieFile);
    return head;
}

//writes the year files into the new directory 
void years(struct movie* head, char* dirName) {
    //loops through the linked list and creates/appends files for each year
    struct movie* temp = head;
    while(temp != NULL) {
        //creates/finds the name of the file that the movie needs to go to
        char filename[256];
        sprintf(filename, "%s/%d.txt", dirName, temp->year);

        //preapres the content to write into the file
        char title[256];
        sprintf(title, "%s\n", temp->title);

        //writes content into file, creating/appending if the file exists or not (found in explortation files)
        int file_descriptor = open(filename, O_RDWR | O_CREAT | O_APPEND, 0640);
        int bytes = write(file_descriptor, title, strlen(title));

        //close the file after writing
        close(file_descriptor);

        //moves to next movie
        temp = temp->next;
    }
}

//processes the files and creates a new directory 
void run(char* filename) {
    //processes and creates the linked list of movies
    printf("Now processing the chosen file named %s\n", filename);
    struct movie* head = processFile(filename);

    //seed the random number
    srand(time(NULL));

    //generate a random number between 0 and 9999
    int num = rand() % 100000;

    //creates name for the directory
    char dirName[256];
    sprintf(dirName, "deleong.movies.%d", num);
    printf("Created directory with name %s\n\n", dirName);

    //creates directory (found on exploration directories and permissions)
    mkdir(dirName, 0750);

    //calls the function to make all the files
    years(head, dirName);

    //frees the linked list of movies
    freeMovie(head);
}

//finds the largest file in the directory
void largest() {   
    //keeps track of the largest file
    int large = 0;
    char largestFile[256];

    //holds all the information of the current file 
    struct stat currStat;

    //opens the current directory (found on exploration directories)
    DIR* currDir = opendir(".");
    struct dirent* aDir;

    //goes through all the entries (found on exploration directories)
    while((aDir = readdir(currDir)) != NULL) {
        //check for .csv extension and prefix "movies_"
        if(strstr(aDir->d_name, ".csv") != NULL && strncmp(aDir->d_name, "movies_", 7) == 0) {
            //Get meta-data for the current entry (found on exploration directories)
            stat(aDir->d_name, &currStat);

            //figures out if current entry is the biggest file and remembers the file if so
            if(currStat.st_size > large) {
                large = currStat.st_size;
                strcpy(largestFile, aDir->d_name);
            }
        }
    }
    //closes the directory
    closedir(currDir);

    //processes the file
    run(largestFile);
}

//finds the smallest file in the directory
void smallest() {
    //keeps track of the smallest file
    int small = __INT_MAX__;
    char smallestFile[256];

    //holds all the information of the current file 
    struct stat currStat;

    //opens the current directory (found on exploration directories)
    DIR* currDir = opendir(".");
    struct dirent* aDir;

    //goes through all the entries (found on exploration directories)
    while((aDir = readdir(currDir)) != NULL) {
        //check for .csv extension and prefix "movies_"
        if(strstr(aDir->d_name, ".csv") != NULL && strncmp(aDir->d_name, "movies_", 7) == 0) {
            //Get meta-data for the current entry (found on exploration directories)
            stat(aDir->d_name, &currStat);

            //figures out if current entry is the smallest file and remembers the file if so
            if(currStat.st_size < small) {
                small = currStat.st_size;
                strcpy(smallestFile, aDir->d_name);
            }
        }
    }
    //closes the directory
    closedir(currDir);

    //processes the file
    run(smallestFile);
}

//option 3 if the user wants a specific file
void specific(int* path) {
    //keeps track of what file the user wants to open 
    char filename[256];
    
    //asks/gets users input
    printf("Enter the complete file name: ");
    scanf("%255s", &filename);

    //tries to open the file 
    FILE* file = fopen(filename, "r");

    //file exists 
    if(file) {
        fclose(file);
        run(filename);
    }

    //file does not exist
    else {
        *path = 0;
        printf("The file %s was not found. Try again \n", filename);
    }
}

int main(int argc, char *argv[]) {

    //keeps track of what path the user wants to take
    int option = 0;

    //makes the user pick one of the options
    while(option != 1) {
        
        //keeps track of what the user wants to do
        int path = 0;

        //prints out all the options and scans the users input
        printf("1. Select file to process \n");
        printf("2. Exit the program \n\n");
        printf("Enter a choice 1 or 2: ");
         scanf("%d", &option);

        //filters out all the options and does the corresponding action 
        switch(option) {
            
            case 1:

                while(path !=  1 && path != 2 && path != 3) {
                    //prints out all the options and scans the users input
                    printf("\nWhich file do you want to process?\n");
                    printf("Enter 1 to pick the largest file\n");
                    printf("Enter 2 to pick the smallest file\n");
                    printf("Enter 3 to specify the name of a file\n\n");
                    printf("Enter a choice from 1 to 3: ");
                    scanf("%d", &path);
                    
                    //filters out all the paths and does the corresponding action
                    switch(path) {

                        case 1:
                            largest();
                            break;

                        case 2:
                            smallest();
                            break;
                        
                        case 3:
                            specific(&path);
                            break;

                        //if the user doesn't input a valid option
                        default: 
                            printf("Please enter a valid option \n\n");
                            break;
                    }
                } 
                
                //makes the user pick a new option after the file is processed
                option = 0;
                break;

            case 2:
                return EXIT_SUCCESS;

            //if a 1 or 2 isn't inputted
            default:
                printf("Please enter a valid option \n\n");
                break;
        }   
    }
}