#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

//function that runs the first option
void years(struct movie* head) {

    //keeps track of the user input
    int year;

    //keeps track if a movie was found in that year
    int valid = 0;

    //asks and reads what year the user wants
    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year);

    //goes through the whole linked trying to find any movie that was released in that year
    struct movie* temp = head;
    while(temp != NULL) {
        if(temp->year == year) {
            printf("%s \n", temp->title);
            valid = 1;
        }

        temp = temp->next;
    }   

    //gives message if no movie was found
    if(valid == 0) {
        printf("No data about movies released in the year %d \n", year);
    }

    printf("\n");
}

//function that runs the second option
void rated(struct movie* head, int count) {
    
    //keeps track of how many spots in the array is filled
    int num = 1;

    //sets up an array that holds the highest rated movie of a certain year
    struct movie* temp = head;
    struct movie* arr[count];
    arr[0] = temp;
    temp = temp->next;

    //runs through the whole linked list sorting through and placing the highest rating movie of each year
    while(temp != NULL) {
        //keeps track of if a movie of the same year is in the array
        int valid = 0;

        //looks through every spot in the array to see if there is a movie from the same year
        for(int i = 0; i < num; i++) {

            //if there is then check to see which movie rating is higher and switch if necessary
            if(temp->year == arr[i]->year) {
                valid = 1;
                if(temp->rating > arr[i]->rating) {
                    arr[i] = temp;
                }
            }
        }

        //if there is no movie from the same year in the array then add this movie
        if(valid == 0) {
            arr[num] = temp;
            num++;
        }

        temp = temp->next;
    }

    //prints out all the movies
    for(int i = 0; i < num; i++) {
        printf("%d %.1f %s \n", arr[i]->year, arr[i]->rating, arr[i]->title);
    }

    printf("\n");
}

//function that runs the third option
void language(struct movie* head) {
    
    //keeps track of the user's input
    char language[20];

    //keeps track if a movie was found in the user's language
    int valid = 0;

    //asks and reads what language the user wants
    printf("Enter the language for which you want to see movies: ");
    scanf("%19s", language);

    //loops through the whole linked list looking for if the user's language is in the array of languages for each movie 
    struct movie* temp = head;
    while(temp != NULL) {
        for(int i = 0; i < 5; i++) {
            //checks to see if the language entry is empty
            if(strlen(temp->languages[i]) == 0) {
                break;
            }

            //if language was found in the array
            if(strcmp(temp->languages[i], language) == 0) {
                valid = 1;
                printf("%d %s\n", temp->year, temp->title);
                break;
            }
        }

        temp = temp->next;
    }

    //gives message if no movie with this language was found
    if(valid == 0) {
        printf("No data about movies released in %s \n", language);
    }
    printf("\n");
}

//runs the whole program 
void run(struct movie* head, int count) {
    
    //keeps track of what option the user wants to do
    int option = 0;

    while(option != 4) {
        //prints the options and asks which one they want to do
        printf("1. Show movies released in the specified year \n");
        printf("2. Show highest rated movie for each year \n");
        printf("3. Show the title and year of release of all movies in a specific language \n");
        printf("4. Exit from the program \n\n");   
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &option);

        //sorts the users input into running whichever function or telling them it was an invalid input
        if(option == 1) {
            years(head);
        }

        else if(option == 2) {
            rated(head, count);
        }

        else if(option == 3) {
            language(head);
        }

        else if(option != 4) {
            printf("You entered an incorrect choice. Try again.\n\n");
        }
    }
}

int main(int argc, char *argv[]) {
    
    //if the user doesn't run with the file
    if(argc < 2) {
        printf("You must provide the name of the file to process\n");
        return EXIT_FAILURE;
    }

    struct movie* head = processFile(argv[1]);  

    //figures out how many movies there are in the file 
    struct movie* temp = head;
    int count = 0;
    while(temp != NULL) {
        count++;
        temp = temp->next;
    }

    printf("Processed file %s and parsed data for %d movies\n\n", argv[1], count);

    //runs the whole program
    run(head, count);

    freeMovie(head);
}