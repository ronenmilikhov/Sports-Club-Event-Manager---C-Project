#define _CRT_SECURE_NO_WARNINGS
#define FNAME_SIZE 10
#define LNAME_SIZE 15
#define BUFFER_SIZE 100
#define M_ERROR "Memory allocation failed. Exiting..."
#define F_ERROR "Unable to open file. Exiting..."
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct
{
	char* p2title; //event name
	char* p2location; //event location
	int year; //event year
} event;

typedef struct
{
	unsigned int id; //ID of the sportsman
	char Fname[FNAME_SIZE]; //First name of the sportsman
	char Lname[LNAME_SIZE]; //Last name of the sportsman
	char* p2club; //name of the club the sportsman belongs to
	int gen; //gender of the sportsman
	event* p2events; //pointer to a dynamic array of events
	int Nevents; //number of events the sportsman participated in
} sportsman;

int countLines(const char* file);
void FromFile2Sportsman(const char* sportsman_file, sportsman* new_sportsman, int sportsmen_size);
void FromFile2Events(const char* events_file, sportsman* new_sportsman, int sportsmen_size);
void initializeNevents(sportsman* new_sportsman, int sportsmen_size);
int addSportsman(sportsman** new_sportsman_ptr, int* sportsmen_size_ptr);
int addEvent(int id, sportsman** new_sportsman_ptr, int* sportsmen_size_ptr);
void printMenu(sportsman** new_sportsman, int* sportsmen_size);
int getSportsmanID();
void getFirstName(char* Fname);
void getLastName(char* Lname);
void getClubName(char* Club_name);
int getGender();
void getEventName(char* eventName);
void getLocation(char* location);
int getYear();
void saveSportsmanData(const sportsman* new_sportsman, int sportsmen_size);
void saveEventData(const sportsman* new_sportsman, int sportsmen_size);
int printEvents(char lastName[LNAME_SIZE], sportsman sportsmen[], int numOfSportsmen);
void getSportsmanLastName(char* lastNamePtr);
void cleanBuffer();
int countEvent(char* E, int Y, sportsman sportsmen[], int numOfSportsmen);
void printPlayersPlayed(int eventsPlayed);
void handlePrintEventResult(int result);
int checkSameEvent(int I, sportsman sportsmen[], int numOfSportsmen);
int isSameEvent(event event1, event event2);
void handleCheckSameEventResult(int result);
void bestClub(sportsman sportsmen[], int numOfSportsmen);
int compareEvents(const void* a, const void* b);
void printSortedEvents(char* C, sportsman sportsmen[], int numOfSportsmen);
void deleteEvent(char* E, int Y, sportsman** sportsmen, int numOfSportsmen);
void newClub(char* C1, char* C2, sportsman sportsmen[], int numOfSportsmen);
void addEventToList(sportsman currSportsman, event** eventList, int* numOfEvents);

void main()
{
	int sportsmen_size = countLines("SportsmanData.txt"); //Find the amount of sportsmen
	sportsman* new_sportsman = (sportsman*)malloc(sportsmen_size * sizeof(sportsman)); //allocate memory for the sportsmen array
	if (new_sportsman == NULL) //Check if the memory allocation was successful
	{
		printf(M_ERROR);
		exit(1);
	}

	//read data from files
	FromFile2Sportsman("SportsmanData.txt", new_sportsman, sportsmen_size); //populate sportsmen array with data from SportsmanData.txt
	FromFile2Events("EventData.txt", new_sportsman, sportsmen_size); //populate events data for each sportsman from EventData.txt
	//printing the menu (user interface)
	printMenu(&new_sportsman, &sportsmen_size);

	// Freeing memory allocated for events
	for (int i = 0; i < sportsmen_size; i++)
	{
		for (int j = 0; j < new_sportsman[i].Nevents; j++)
		{
			// Free memory for event title and location
			new_sportsman[i].p2events[j].p2title = NULL;
			new_sportsman[i].p2events[j].p2location = NULL;
			free(new_sportsman[i].p2events[j].p2title);
			free(new_sportsman[i].p2events[j].p2location);
		}
		// free allocated memory for every sportsman's club name
		new_sportsman[i].p2club = NULL;
		free(new_sportsman[i].p2club);
	}
	// Freeing all the allocated memory for sportsmen array
	free(new_sportsman);
}

int countLines(const char* file) //Counts the number of lines in the SportsmanData txt file
{
	FILE* read_file = fopen(file, "r"); //Open file for read
	if (read_file == NULL) // Check if the file was opened successfully
	{
		printf(F_ERROR);
		exit(1);
	}

	int count = 0; //Counter for the amount of lines
	char buffer[BUFFER_SIZE]; // buffer for storing the lines
	fgets(buffer, sizeof(buffer), read_file); //Ignores the first line (format line)

	while (fgets(buffer, sizeof(buffer), read_file) != NULL) //Starts counting all the lines
	{
		count++; //if a new line is found, updates the count by 1
	}
	fclose(read_file); //Close the file
	return count;
}

void FromFile2Sportsman(const char* sportsman_file, sportsman* new_sportsman, int sportsmen_size) //Function to read sportsman data from SportsmanData file and populate the sportsmen array
{
	FILE* read_sportsman_file = fopen(sportsman_file, "r"); //Open file for read
	if (read_sportsman_file == NULL) //Check if the file was opened successfully
	{
		printf(F_ERROR);
		exit(1);
	}

	char buffer[BUFFER_SIZE];// buffer to hold data read from the file
	fgets(buffer, sizeof(buffer), read_sportsman_file); // Reads the format line from the file ("Ignores" the format line)

	for (int i = 0; i < sportsmen_size; i++) // Loop through each line in the file
	{
		fgets(buffer, sizeof(buffer), read_sportsman_file); //Read the first line into the buffer (The first sportsman)

		sscanf(buffer, "%u;%[^;];%[^;];%[^;];%d", &new_sportsman[i].id, new_sportsman[i].Fname, new_sportsman[i].Lname, buffer, &new_sportsman[i].gen); // Paste the sportsman data from the buffer

		new_sportsman[i].p2club = (char*)calloc(strlen(buffer) + 1, sizeof(char)); // Allocate memory for club name and copy data
		if (new_sportsman[i].p2club == NULL) //Check if memory allocation was successful
		{
			printf(M_ERROR);
			exit(1);
		}
		strcpy(new_sportsman[i].p2club, buffer); //Copy the content from the buffer and paste in the p2club field
	}
	fclose(read_sportsman_file); //Close the file
}

void FromFile2Events(const char* events_file, sportsman* new_sportsman, int sportsmen_size) //Function to read event data from the EventData file and fill the list of events for each sportsman
{
	FILE* read_events_file = fopen(events_file, "r"); //Open file for read
	if (read_events_file == NULL) //Check if file was opened successfully
	{
		printf(F_ERROR);
		exit(1);
	}

	initializeNevents(new_sportsman, sportsmen_size); //Calling the function in order to initialize the Nevents of each of the sportsman in the array to 0 

	char buffer[BUFFER_SIZE]; // Buffer to hold data read from the file
	fgets(buffer, sizeof(buffer), read_events_file); //Reads the format line from the file ("Ignores" the format line)

	while (fgets(buffer, sizeof(buffer), read_events_file) != NULL) //Loop through each line in the file
	{
		int id, year; //Variables to store the Sportsman ID and year of the event
		char event_name[BUFFER_SIZE], location[BUFFER_SIZE]; //Strings to store the name and location of the event

		sscanf(buffer, "%d,%[^,],%[^,],%d", &id, event_name, location, &year); // Paste the event data from the buffer

		event_name[sizeof(event_name) - 1] = '\0'; //Null terminate event_name
		location[sizeof(location) - 1] = '\0'; //Null terminate location

		for (int i = 0; i < sportsmen_size; i++) //find the corresponding sportsman
		{
			if (new_sportsman[i].id == id) //Check if ID matches Sportsman ID in events file
			{
				int num_events = new_sportsman[i].Nevents; //Get the current number of events

				if (num_events == 0) // If this is the first event for the sportsman, allocate memory for p2events
				{
					new_sportsman[i].p2events = calloc(1, sizeof(event));
					if (new_sportsman[i].p2events == NULL) //Check if memory allocation was successful
					{
						printf(M_ERROR);
						exit(1);
					}
				}
				else
				{
					// Reallocate memory for p2events to accommodate the new event
					event* temp = realloc(new_sportsman[i].p2events, (num_events + 1) * sizeof(event));
					if (temp == NULL) //Check if memory allocation was successful
					{
						printf(M_ERROR);
						exit(1);
					}
					new_sportsman[i].p2events = temp; //Update the pointer p2events to the new allocated memory
				}
				//Allocate memory for p2title and copy event_name
				new_sportsman[i].p2events[num_events].p2title = calloc(strlen(event_name) + 1, sizeof(char)); //Allocate memory for p2title
				if (new_sportsman[i].p2events[num_events].p2title == NULL) //Check if the memory was allocated successfully
				{
					printf(M_ERROR);
					exit(1);
				}
				strcpy(new_sportsman[i].p2events[num_events].p2title, event_name); //Copy event_name into p2title

				//Allocate memory for p2location and copy location
				new_sportsman[i].p2events[num_events].p2location = calloc(strlen(location) + 1, sizeof(char)); //Allocate memory for p2location
				if (new_sportsman[i].p2events[num_events].p2location == NULL) //Check if the memory was allocated successfully
				{
					printf(M_ERROR);
					exit(1);
				}
				strcpy(new_sportsman[i].p2events[num_events].p2location, location); //Copy location into p2location

				new_sportsman[i].p2events[num_events].year = year; // Store the year

				new_sportsman[i].Nevents++;// Increase the number of events for the sportsman

				break; // Once found the corresponding sportsman, exit the loop
			}
		}
	}
	fclose(read_events_file); //Close the file
}

int addSportsman(sportsman** new_sportsman_ptr, int* sportsmen_size_ptr) //Function to add a new sportsman to the array
{
	sportsman* new_sportsman = *new_sportsman_ptr; //pointer to the sportsmen array
	int sportsmen_size = *sportsmen_size_ptr; //size of the sportsmen array

	//Variables to store details of the new sportsman
	int id; // Holds the ID of the sportsman
	char Fname[BUFFER_SIZE]; // Holds the first name of the sportsman
	char Lname[BUFFER_SIZE]; // Holds the last name of the sportsman
	char Club_name[BUFFER_SIZE]; // Holds the club name the sportsman belongs to
	int gender; // Holds the gender of the sportsman
	int i; //Index variable for loops

	id = getSportsmanID(); //get the ID of the new sportsman from the user

	for (i = 0; i < sportsmen_size; i++) // Check if the sportsman already exists in the array by the ID
	{
		if (new_sportsman[i].id == id)
		{
			printf("Sportsman with ID %u already exists in the array.\n", id);
			return 0; // Returns 0 if unable to add the sportsman
		}
	}

	getFirstName(Fname); //get the first name of the new sportsman from the user
	getLastName(Lname); //get the last name of the new sportsman from the user

	for (i = 0; i < sportsmen_size; i++) // Check if the sportsman already exists in the array by the Last name
	{
		if (_stricmp(new_sportsman[i].Lname, Lname) == 0)
		{
			printf("Sportsman with last name %s already exists in the array.\n", Lname);
			return 0;
		}
	}

	getClubName(Club_name); //get the club name of the new sportsman from the user
	gender = getGender(); //get the gender of the new sportsman from the user

	sportsman* temp = realloc(new_sportsman, (sportsmen_size + 1) * sizeof(sportsman)); // Increase the size of the sportsmen's array by 1
	if (temp == NULL)
	{
		printf(M_ERROR);
		exit(1);
	}
	new_sportsman = temp;

	// Assign the details of the new sportsman to the last element in the array
	new_sportsman[sportsmen_size].id = id;
	strncpy(new_sportsman[sportsmen_size].Fname, Fname, FNAME_SIZE); //Null terminate the first name
	new_sportsman[sportsmen_size].Fname[FNAME_SIZE - 1] = '\0';
	strncpy(new_sportsman[sportsmen_size].Lname, Lname, LNAME_SIZE); //Null terminate the last name
	new_sportsman[sportsmen_size].Lname[LNAME_SIZE - 1] = '\0';
	new_sportsman[sportsmen_size].p2club = _strdup(Club_name); // Allocate memory and copy club name
	new_sportsman[sportsmen_size].gen = gender; //Assign the gender
	new_sportsman[sportsmen_size].p2events = NULL; // Initialize events pointer to NULL
	new_sportsman[sportsmen_size].Nevents = 0; // Initialize number of events to 0

	*new_sportsman_ptr = new_sportsman; // Update the original pointer
	*sportsmen_size_ptr = sportsmen_size + 1; // Update the size
	printf("New sportsman was added successfully.\n");
	return 1; // Return 1 indicating success
}

int addEvent(int id, sportsman** new_sportsman_ptr, int* sportsmen_size_ptr) //Function to add a new event for a sportsman
{
	sportsman* new_sportsman = *new_sportsman_ptr; //pointer to the sportsmen array
	int sportsmen_size = *sportsmen_size_ptr; //size of the sportsmen array

	char event_name[BUFFER_SIZE]; // Holds the name of the event
	char location[BUFFER_SIZE]; // Holds the location of the event
	int year; // Holds the year of the event

	// Check if the sportsman with the given ID exists
	int found_index = -1;
	for (int i = 0; i < sportsmen_size; i++)
	{
		if (new_sportsman[i].id == id)
		{
			found_index = i;
			break;
		}
	}

	// If the sportsman with the given ID doesn't exist, exit the program
	if (found_index == -1)
	{
		printf("Sportsman with ID %u not found.\n", id);
		return 0;
	}

	// Get event details from the user
	getEventName(event_name); //get the name of the event from the user
	getLocation(location); //get the location of the event from the user
	year = getYear(); //get the year of the event from the user

	// Check if the event already exists for the sportsman
	for (int i = 0; i < sportsmen_size; i++)
	{
		if (new_sportsman[i].id == id)
		{
			if (new_sportsman[i].Nevents == 0)
			{
				// If the sportsman has no events, initialize p2events pointer
				new_sportsman[i].p2events = NULL;
			}
			for (int j = 0; j < new_sportsman[i].Nevents; j++)
			{
				// Compare event details
				if (_stricmp(new_sportsman[i].p2events[j].p2title, event_name) == 0 &&
					_stricmp(new_sportsman[i].p2events[j].p2location, location) == 0 &&
					new_sportsman[i].p2events[j].year == year)
				{
					printf("Event already exists for this sportsman, unable to add the event.\n");
					return 0; // Event already exists, return 0
				}
			}
			// Event does not exist, add it to the list of events
			int num_events = new_sportsman[i].Nevents;

			// Reallocate memory for p2events to accommodate the new event
			event* temp = realloc(new_sportsman[i].p2events, (num_events + 1) * sizeof(event));
			if (temp == NULL)
			{
				printf(M_ERROR);
				exit(1);
			}
			new_sportsman[i].p2events = temp;

			// Allocate memory for event title and location
			new_sportsman[i].p2events[num_events].p2title = malloc(strlen(event_name) + 1);
			if (new_sportsman[i].p2events[num_events].p2title == NULL)
			{
				printf(M_ERROR);
				exit(1);
			}
			new_sportsman[i].p2events[num_events].p2location = malloc(strlen(location) + 1);
			if (new_sportsman[i].p2events[num_events].p2location == NULL)
			{
				printf(M_ERROR);
				exit(1);
			}

			// Copy event details
			strcpy(new_sportsman[i].p2events[num_events].p2title, event_name);
			strcpy(new_sportsman[i].p2events[num_events].p2location, location);
			new_sportsman[i].p2events[num_events].year = year;

			new_sportsman[i].Nevents++; // Increase the number of events for the sportsman

			*new_sportsman_ptr = new_sportsman; // Update the original pointer
			*sportsmen_size_ptr = sportsmen_size; // Update the size
			printf("Event added successfully.\n");
			return 1; // Event added successfully, return 1
		}
	}
}

void printMenu(sportsman** new_sportsman, int* sportsmen_size) //Function to print the menu (user interface)
{
	int choice; //Holds the user's choice from the menu
	int sportsman_added = 0; // indicates whether a sportsman was added during the session
	int event_added = 0; // indicates whether an event was added during the session
	char sportsmanLastName[LNAME_SIZE];
	char* eventName = malloc(BUFFER_SIZE);
	char* clubName = malloc(BUFFER_SIZE);
	char* clubName2 = malloc(BUFFER_SIZE);
	int eventsPlayed = 0;
	int id = 0;
	int eventYear;
	do {
		printf("Choose an option:\n");
		printf("1. Add sportsman\n");
		printf("2. Add event for a sportsman\n");
		printf("3. Print events of a sportsman\n");
		printf("4. Print sportsmen that played in the event\n");
		printf("5. Print the club with the most events\n");
		printf("6. Check participation of a sportsman in events with others\n");
		printf("7. Print Club Events\n");
		printf("8. Delete an event for all sportsmen\n");
		printf("9. Create a new club\n");
		printf("0. End the program\n");
		printf("Enter your choice: ");
		scanf("%d", &choice); //get the user's choice from the menu
		switch (choice) {
		case 1:
			while (getchar() != '\n'); // Clear the input buffer
			sportsman_added = addSportsman(new_sportsman, sportsmen_size); // Add a sportsman (1 - success, 0 - failure)
			break;
		case 2:
			while (getchar() != '\n'); // Clear the input buffer
			int sportsman_id = getSportsmanID(); //get the ID of the sportsman from the user
			event_added = addEvent(sportsman_id, new_sportsman, sportsmen_size); // Add an event (1 - success, 0 - failure)
			break;
		case 3:
			cleanBuffer();
			getLastName(sportsmanLastName);
			handlePrintEventResult(printEvents(sportsmanLastName, *new_sportsman, *sportsmen_size));
			break;
		case 4:
			cleanBuffer();
			getEventName(eventName);
			eventYear = getYear();
			printPlayersPlayed(countEvent(eventName, eventYear, *new_sportsman, *sportsmen_size));
			break;
		case 5:
			bestClub(*new_sportsman, *sportsmen_size);
			break;
		case 6:
			cleanBuffer();
			id = getSportsmanID();
			handleCheckSameEventResult(checkSameEvent(id, *new_sportsman, *sportsmen_size));
			break;
		case 7:
			cleanBuffer();
			getClubName(clubName);
			printSortedEvents(clubName, *new_sportsman, *sportsmen_size);
			break;
		case 8:
			cleanBuffer();
			getEventName(eventName);
			eventYear = getYear();
			deleteEvent(eventName, eventYear, new_sportsman, *sportsmen_size);
			printf("Events deleted\n");
			event_added = 1;
			break;
		case 9:
			cleanBuffer();
			printf("Please enter 2 clubs:\n");
			getClubName(clubName);
			getClubName(clubName2);
			newClub(clubName, clubName2, *new_sportsman, *sportsmen_size);
			break;
		case 0:
			if (sportsman_added) //if a sportsman was added, save the sportsman data before ending the program
			{
				saveSportsmanData(*new_sportsman, *sportsmen_size);
			}
			if (event_added) //if an event was added, save the event data before ending the program
			{
				saveEventData(*new_sportsman, *sportsmen_size);
			}
			printf("Ending the program.\n");
			break;
		default:
			printf("Invalid choice. Please enter a valid option.\n");
			while (getchar() != '\n'); // Clear the input buffer
		}
	} while (choice != 0); //Continue to display the menu until the user chooses to end the program
}

void initializeNevents(sportsman* new_sportsman, int sportsmen_size) //Initialize the Nevents for each sportsman
{
	for (int i = 0; i < sportsmen_size; i++) //Loop through each sportsman in the array and initialize their Nevents to 0
	{
		new_sportsman[i].Nevents = 0;
	}
}

int getSportsmanID() //Function to ask the user for the sportsman's ID
{
	char input[BUFFER_SIZE]; //buffer to store user input
	int id = 0; //variable to store the sportsman's ID

	//Ask the user to enter a sportsman ID until a valid ID is provided
	do {
		printf("Enter ID of sportsman (positive number): ");
		fgets(input, sizeof(input), stdin); //read user input from standard input

		// Remove trailing newline character
		input[strcspn(input, "\n")] = '\0';

		// Check if input is empty or contains only whitespace characters
		int isEmpty = 1; //flag to indicate if input is empty
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (!isspace(input[i])) //if a non-whitespace character is found, set isEmpty to false
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty) // if input is empty, ask the user to enter a non-empty value
		{
			printf("ID cannot be empty.\n");
			continue;
		}

		// Check if input contains spaces between digits
		int hasSpaces = 0; //flag to indicate if input contains spaces
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (isspace(input[i])) //if a space character is found, set hasSpaces to true
			{
				hasSpaces = 1;
				break;
			}
		}

		if (hasSpaces) //if the input contains spaces, ask the user to enter a value without spaces
		{
			printf("Invalid input. Please enter a number without spaces.\n");
			continue;
		}

		// Convert input to integer
		id = atoi(input);

		// Check if ID is positive
		if (id <= 0)
		{
			printf("Invalid input. Please enter a positive ID number.\n");
		}

	} while (id <= 0); //Repeat the loop until a valid positive ID is entered

	return id; //return the valid sportsman ID
}

void getFirstName(char* Fname) //Ask the user for the sportsman's first name
{
	while (1)
	{
		printf("Enter first name of sportsman (will read maximum 9 characters): ");
		fgets(Fname, BUFFER_SIZE, stdin); // Getting the Fname from the user

		// Remove newline character if present
		if (Fname[strlen(Fname) - 1] == '\n')
			Fname[strlen(Fname) - 1] = '\0';
		else
			while (getchar() != '\n'); // Clear input buffer

		// Check if input is empty or consists only of backspaces
		int isEmpty = 1;
		for (int i = 0; Fname[i] != '\0'; i++)
		{
			if (!isspace(Fname[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("First name cannot be empty.\n");
			continue;
		}

		// Check input validity
		int valid = 1;
		if (strlen(Fname) > FNAME_SIZE - 1)
		{
			printf("First name must be maximum 9 characters.\n");
			valid = 0;
		}
		else
		{
			for (int i = 0; Fname[i] != '\0'; i++)
			{
				// Check if the character is a letter or a space
				if (!isalpha(Fname[i]) && Fname[i] != ' ')
				{
					printf("First name cannot contain special characters or numbers.\n");
					valid = 0;
					break;
				}
			}
		}

		if (valid)
		{
			break; // Input is valid, exit the loop
		}
	}
}

void getLastName(char* Lname) //Ask the user for the sportsman's last name
{
	while (1)
	{
		printf("Enter last name of sportsman (will read maximum 14 characters): ");
		fgets(Lname, BUFFER_SIZE, stdin); // Getting the Lname from the user

		// Remove newline character if present
		if (Lname[strlen(Lname) - 1] == '\n')
			Lname[strlen(Lname) - 1] = '\0';
		else
			while (getchar() != '\n'); // Clear input buffer

		// Check if input is empty or consists only of backspaces
		int isEmpty = 1;
		for (int i = 0; Lname[i] != '\0'; i++)
		{
			if (!isspace(Lname[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("Last name cannot be empty.\n");
			continue;
		}

		// Check input validity
		int valid = 1;
		if (strlen(Lname) > LNAME_SIZE - 1)
		{
			printf("Last name must be maximum 14 characters.\n");
			valid = 0;
		}
		else
		{
			for (int i = 0; Lname[i] != '\0'; i++)
			{
				// Check if the character is a letter or a space
				if (!isalpha(Lname[i]) && Lname[i] != ' ')
				{
					printf("Last name cannot contain special characters or numbers.\n");
					valid = 0;
					break;
				}
			}
		}

		if (valid)
		{
			break; // Input is valid, exit the loop
		}
	}
}

void getClubName(char* Club_name) //Ask the user for the sportsman's club name
{
	while (1)
	{
		printf("Enter the club name the sportsman belongs to: ");
		fgets(Club_name, BUFFER_SIZE, stdin);

		// Remove newline character if present
		Club_name[strcspn(Club_name, "\n")] = '\0';

		// Check if the input is empty or consists of only whitespace characters
		int isEmpty = 1;
		for (int i = 0; Club_name[i] != '\0'; i++)
		{
			if (!isspace(Club_name[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		// Check if the input contains only alphabetic characters or spaces
		int isValid = 1;
		for (int i = 0; Club_name[i] != '\0'; i++)
		{
			if (!isalpha(Club_name[i]) && Club_name[i] != ' ')
			{
				isValid = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("Club name cannot be empty.\n");
		}
		else if (isValid)
		{
			break; // Input is valid, exit the loop
		}
		else
		{
			printf("Invalid input. Club name should contain only alphabetic characters.\n");
		}
	}
}

int getGender() //Ask the user for the sportsman's gender
{
	int gender;
	char input[BUFFER_SIZE];
	while (1)
	{
		printf("Enter the gender of the sportsman (0 for male, 1 for female): ");
		fgets(input, sizeof(input), stdin);

		// Remove trailing newline character
		input[strcspn(input, "\n")] = '\0';

		// Check if input is empty or contains only whitespace characters
		int isEmpty = 1;
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (!isspace(input[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("Gender cannot be empty.\n");
			continue;
		}

		// Check if input contains only digits
		int isDigit = 1;
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (!isdigit(input[i]))
			{
				isDigit = 0;
				break;
			}
		}

		if (!isDigit)
		{
			printf("Invalid input. Please enter a numeric value (0 or 1).\n");
			continue;
		}

		// Convert input to integer
		gender = atoi(input);

		// Check if input is within valid range
		if (gender != 0 && gender != 1)
		{
			printf("Invalid input. Please enter 0 or 1.\n");
			continue;
		}

		break; // Exit loop if input is valid
	}
	return gender;
}

void getEventName(char* eventName) //Ask the user for the event name
{
	while (1)
	{
		printf("Enter the name of the event: ");
		fgets(eventName, BUFFER_SIZE, stdin);

		// Remove newline character
		if (eventName[strlen(eventName) - 1] == '\n')
			eventName[strlen(eventName) - 1] = '\0';
		else
			while (getchar() != '\n'); // Clear input buffer

		// Check if input is empty or consists only of backspaces
		int isEmpty = 1;
		for (int i = 0; eventName[i] != '\0'; i++)
		{
			if (!isspace(eventName[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("Event name cannot be empty.\n");
			continue;
		}

		// Validate input
		int isValid = 1;
		for (int i = 0; eventName[i] != '\0'; i++)
		{
			if (!isalpha(eventName[i]) && eventName[i] != ' ')
			{
				isValid = 0;
				break;
			}
		}

		// If input is invalid, ask again
		if (!isValid)
		{
			printf("Invalid input. Event name should contain only alphabetic characters and spaces.\n");
			continue;
		}
		else
		{
			break; // Input is valid, exit the loop
		}
	}
}

void getLocation(char* location) //Ask the user for the event location
{
	while (1)
	{
		printf("Enter the location of the event: ");
		fgets(location, BUFFER_SIZE, stdin);

		// Remove newline character
		if (location[strlen(location) - 1] == '\n')
			location[strlen(location) - 1] = '\0';
		else
			while (getchar() != '\n'); // Clear input buffer

		// Check if input is empty or consists only of backspaces
		int isEmpty = 1;
		for (int i = 0; location[i] != '\0'; i++)
		{
			if (!isspace(location[i]))
			{
				isEmpty = 0;
				break;
			}
		}

		if (isEmpty)
		{
			printf("Location cannot be empty.\n");
			continue;
		}

		// Validate input
		int isValid = 1;
		for (int i = 0; location[i] != '\0'; i++)
		{
			if (!isalpha(location[i]) && location[i] != ' ')
			{
				isValid = 0;
				break;
			}
		}

		// If input is invalid, ask again
		if (!isValid)
		{
			printf("Invalid input. Location should contain only alphabetic characters and spaces.\n");
			continue;
		}
		else
		{
			break; // Input is valid, exit the loop
		}
	}
}

int getYear() //Ask the user for the event year
{
	int year;
	char input[BUFFER_SIZE];
	int validInput;

	do {
		printf("Enter the year of the event (2000-2023): ");
		fgets(input, sizeof(input), stdin);

		// Remove newline character if present
		if (input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}

		// Check if input contains only digits
		validInput = 1;
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (!isdigit(input[i]))
			{
				validInput = 0;
				break;
			}
		}

		if (!validInput)
		{
			printf("Invalid input. Please enter a valid year.\n");
			continue;
		}

		// Convert input to integer
		year = atoi(input);

		// Check if year is within the valid range
		if (year < 2000 || year > 2023)
		{
			printf("Invalid year. Please enter a year between 2000 and 2023.\n");
			validInput = 0;
		}

	} while (!validInput);

	return year;
}

void saveSportsmanData(const sportsman* new_sportsman, int sportsmen_size) //Function to save sportsman data to the file
{
	FILE* sportsman_file = fopen("SportsmanData.txt", "r+"); // Open the file
	if (sportsman_file == NULL) //check if file was opened successfully
	{
		printf(F_ERROR);
		exit(1);
	}

	// Write the format line to the file
	fprintf(sportsman_file, "format:sportsman_id;first_name;last_name;club_name;gender\n");

	// Loop through each sportsman and append their details to the file
	for (int i = 0; i < sportsmen_size; i++)
	{
		// Append details of the current sportsman
		fprintf(sportsman_file, "%u;%s;%s;%s;%d\n", new_sportsman[i].id, new_sportsman[i].Fname, new_sportsman[i].Lname, new_sportsman[i].p2club, new_sportsman[i].gen);
	}

	fclose(sportsman_file); //close the file
}

void saveEventData(const sportsman* new_sportsman, int sportsmen_size) //Function to save event data to the file
{
	FILE* events_file = fopen("EventData.txt", "r+"); // Open the file
	if (events_file == NULL) //Check if file was opened successfully
	{
		printf(F_ERROR);
		exit(1);
	}

	// Write the format line to the file
	fprintf(events_file, "format:sportsman_id,event_name,location,year\n");

	// Iterate through each sportsman
	for (int i = 0; i < sportsmen_size; i++)
	{
		// Iterate through each event of the current sportsman
		for (int j = 0; j < new_sportsman[i].Nevents; j++)
		{
			// Write event details to the file
			fprintf(events_file, "%u,%s,%s,%d\n", new_sportsman[i].id,
				new_sportsman[i].p2events[j].p2title,
				new_sportsman[i].p2events[j].p2location,
				new_sportsman[i].p2events[j].year);
		}
	}

	fclose(events_file); //Close the file
}

/*
Function prints all the events for the requested sportsman
returns 0 if there are no events for the requested player
returns -1 if the player does not exist in the list
returns 1 otherwise
*/
int printEvents(char lastName[LNAME_SIZE], sportsman sportsmen[], int numOfSportsmen)
{
	int foundFlag = 0;
	sportsman currSportsman;
	//goes over all the sportsmen
	for (int i = 0; i < numOfSportsmen; i++)
	{
		currSportsman = sportsmen[i];
		//checks if the current sportsman's last name is the same as the requested one
		if (strcmp(currSportsman.Lname, lastName) == 0)
		{
			if (currSportsman.p2events == NULL) //checks if there are no events for this sportsman
				return 0;
			//going over all the events of the requested player

			for (int j = 0; j < currSportsman.Nevents; j++)
			{
				printf("Event name: %s, Event year: %d\n", currSportsman.p2events[j].p2title, currSportsman.p2events[j].year);
			}

			return 1;
		}
	}
	//if the player wasn't found in the list
	return -1;
}


void cleanBuffer()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
	//going over the \n and clearing the buffer
}

/*
Function counts the amount of players that played in the requested event in the requested year
*/
int countEvent(char* E, int Y, sportsman sportsmen[], int numOfSportsmen)
{
	int count = 0;
	sportsman currSportsman;
	//going over all the players
	for (int i = 0; i < numOfSportsmen; i++)
	{
		currSportsman = sportsmen[i];
		//going over all events of the current sportsman
		for (int j = 0; j < currSportsman.Nevents; j++)
		{
			if (strcmp(currSportsman.p2events[j].p2title, E) == 0 && currSportsman.p2events[j].year == Y)
			{
				count++;
				break;
			}
		}
	}
	return count;
}

void printPlayersPlayed(int eventsPlayed)
{
	switch (eventsPlayed)
	{
	case 1:
		printf("1 player played in the event\n");
		break;
	case 0:
		printf("No player played in the event\n");
		break;
	default:
		printf("%d Players played in the event\n", eventsPlayed);
		break;
	}
}

void handlePrintEventResult(int result)
{
	switch (result)
	{
	case -1:
		printf("\nSportsman does not exist\n\n");
		break;
	case 0:
		printf("Sportsman's event list is empty\n\n");
		break;
	case 1:
		printf("Printed the event list of the sportsman\n\n");
		break;
	}
}

/*
* checks if selected player has similar events with other sportsmen, if he does it prints the names of them.
*/
int checkSameEvent(int I, sportsman sportsmen[], int numOfSportsmen)
{
	int returnFlag = 0;
	int foundEvent = 0;
	sportsman selectedPlayer;
	sportsman currPlayer;
	//Finding the selected sportsman
	for (int i = 0; i < numOfSportsmen; i++)
	{
		if (I == sportsmen[i].id)
		{
			selectedPlayer = sportsmen[i];
			break;
		}
	}

	//going over all other sportsman and checking which has similiar events with the selected sportsman
	for (int i = 0; i < numOfSportsmen; i++)
	{
		//skips the selected sportsman
		if (sportsmen[i].id != I)
		{
			currPlayer = sportsmen[i];
			foundEvent = 0;
			//compares events of selected sportsman and current sportsman
			for (int j = 0; j < selectedPlayer.Nevents; j++)
			{

				for (int k = 0; k < currPlayer.Nevents; k++)
				{
					if (isSameEvent(currPlayer.p2events[k], selectedPlayer.p2events[j]))
					{
						foundEvent += 1;
						break;
					}
				}
			}
			if (foundEvent == selectedPlayer.Nevents)
			{
				returnFlag = 1;
				printf("First name: %s, Last name: %s\n", currPlayer.Fname, currPlayer.Lname);
			}
		}
	}
	return returnFlag;

}

//Function checks if the 2 events are the same
int isSameEvent(event event1, event event2)
{
	if (event1.year == event2.year &&
		strcmp(event1.p2title, event2.p2title) == 0 &&
		strcmp(event1.p2location, event2.p2location) == 0)
		return 1;
	return 0;
}

void handleCheckSameEventResult(int result)
{
	if (result)
		printf("At least 1 sportsman participated in all events as selected sportsman\n");
	else
		printf("There isn't a sportsman that participated in all events with selected sportsman\n");
}


void bestClub(sportsman sportsmen[], int numOfSportsmen)
{
	sportsman currSportsman;
	char* C = NULL;
	int maxEventCount = 0;
	int eventCount = 0;
	//goes over all sportsman
	for (int i = 0; i < numOfSportsmen; i++)
	{
		currSportsman = sportsmen[i];
		//checks if the best club is already remembered or if there isn't a best club yet
		if (C == NULL || strcmp(C, currSportsman.p2club) != 0)
		{
			//counts all the events of the current club
			for (int j = 0; j < numOfSportsmen; j++)
			{
				if (strcmp(currSportsman.p2club, sportsmen[j].p2club) == 0)
				{
					eventCount += sportsmen[j].Nevents;
				}
			}
			//checks if the current club is better than the best club
			if (maxEventCount < eventCount)
			{
				maxEventCount = eventCount;
				C = currSportsman.p2club;
			}
			eventCount = 0;
		}
	}

	printf("The best club is %s, with %d events\n", C, maxEventCount);
}


void printSortedEvents(char* C, sportsman sportsmen[], int numOfSportsmen)
{
	int numOfEvents = 0;
	int alreadyAdded = 0;
	event* totalEvents = NULL;
	sportsman currSportsman;
	//goes over all the sportsmen
	for (int i = 0; i < numOfSportsmen; i++)
	{
		currSportsman = sportsmen[i];
		//continues only if the sportsman belongs to the selected club
		if (strcmp(currSportsman.p2club, C) != 0)
			continue;

		//goes over all the events of the selected player
		for (int j = 0; j < currSportsman.Nevents; j++)
		{

			//goes over all the events already added to the list and checks if it already exists
			for (int k = 0; k < numOfEvents; k++)
			{
				if (strcmp(totalEvents[k].p2title, currSportsman.p2events[j].p2title) == 0
					&& totalEvents[k].year == currSportsman.p2events[j].year)
				{
					alreadyAdded = 1;
					break;
				}
			}

			//if the event already exists in the list of events
			if (!alreadyAdded)
			{
				if (totalEvents == NULL)
				{
					totalEvents = (event*)malloc((numOfEvents + 1) * sizeof(event));
					if (totalEvents == NULL)
					{
						printf("Error!");
						return -1;
					}
				}
				else
				{
					totalEvents = (event*)realloc(totalEvents, (numOfEvents + 1) * sizeof(event));
					if (totalEvents == NULL)
					{
						printf("Error!");
						return -1;
					}
				}
				totalEvents[numOfEvents] = currSportsman.p2events[j];
				numOfEvents++;
			}
			alreadyAdded = 0;
		}


	}

	if (numOfEvents == 0)
	{
		printf("Club does not exist!\n");
		return;
	}
	//sort the list by year and print it
	qsort(totalEvents, numOfEvents, sizeof(event), compareEvents);
	for (int i = 0; i < numOfEvents; i++)
	{
		printf("Event name: %s, Year: %d\n", totalEvents[i].p2title, totalEvents[i].year);
	}
	free(totalEvents);

}

//Function used for sorting the event list with qsort
int compareEvents(const void* a, const void* b)
{
	const event* eventA = (const event*)a;
	const event* eventB = (const event*)b;
	return eventA->year - eventB->year;
}



/*
* Function that goes over all the sportsmen and removes the selected event by name and year
*/
void deleteEvent(char* E, int Y, sportsman** sportsmen, int numOfSportsmen)
{
	sportsman* currSportsman;
	//goes over all the sportsmen
	for (int i = 0; i < numOfSportsmen; i++)
	{

		for (int j = 0; j < (*sportsmen + i)->Nevents; j++)
		{
			//checks if events match
			if (strcmp(E, (*sportsmen + i)->p2events[j].p2title) == 0 && Y == (*sportsmen + i)->p2events[j].year)
			{
				//removes the events from the list
				for (int k = j; k < (*sportsmen + i)->Nevents - 1; k++)
				{
					(*sportsmen + i)->p2events[k] = (*sportsmen + i)->p2events[k + 1];
				}
				(*sportsmen + i)->Nevents--;

				break;
			}
		}
	}

}


void newClub(char* C1, char* C2, sportsman sportsmen[], int numOfSportsmen)
{

	FILE* clubFile = fopen("Club.txt", "w+");
	if (clubFile == NULL)
	{
		printf("Unable to open file");
		return -1;
	}
	int numOfevents = 0;
	sportsman currSportsman;
	event* eventList = NULL;
	//goes over all the sportsmen
	for (int i = 0; i < numOfSportsmen; i++)
	{
		currSportsman = sportsmen[i];
		//if the current sportsman's club is one of the 2 clubs selected
		if (strcmp(currSportsman.p2club, C1) == 0 || strcmp(currSportsman.p2club, C2) == 0)
		{
			addEventToList(currSportsman, &eventList, &numOfevents);
		}
	}
	//freeing the events
	for (int i = 0; i < numOfevents; i++)
	{
		free(eventList[i].p2title);
		free(eventList[i].p2location);
	}
	free(eventList);
	fclose(clubFile);
}

//function adds events of the current sportsman to the list of events if it isn't already on the event list
void addEventToList(sportsman currSportsman, event** eventList, int* numOfEvents)
{
	int alreadyAdded = 1;
	for (int i = 0; i < currSportsman.Nevents; i++)
	{
		if (*eventList)
		{
			//goes over all the events already added to the list and checks if it already exists
			for (int j = 0; j < *numOfEvents; j++)
			{
				if (strcmp(currSportsman.p2events[i].p2title, (*eventList)[j].p2title) == 0)
				{
					alreadyAdded = 1;
					break;
				}
			}
		}
		//if the event already exists in the list of events
		if (!alreadyAdded)
		{
			if (*eventList == NULL)
			{
				//create new event list if doesnt exist
				*eventList = (event*)malloc((*numOfEvents + 1) * sizeof(event));
				if (*eventList == NULL)
				{
					printf("Error!");
					return -1;
				}
			}
			else
			{
				//add memory to created list
				*eventList = (event*)realloc(*eventList, (*numOfEvents + 1) * sizeof(event));
				if (*eventList == NULL)
				{
					printf("Error!");
					return -1;
				}
			}
			//copy the event to the list
			(*eventList)[*numOfEvents] = currSportsman.p2events[i];
			(*numOfEvents)++;
		}
		alreadyAdded = 0;
	}
}