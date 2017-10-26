#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "CalendarParser.h"
#include "HelperFunctions.h"

int homeScreen(int answer);
int newScreen(char* title, int (*handler)(int answer), size_t length, const char* option, ... );

int main() {
  newScreen("iCalendar Home", &homeScreen, 3, "Read iCalendar from file", "New Calendar Object", "Exit"); // Open the home screen
  return 0;
}

int newScreen(char* title, int (*handler)(int answer), size_t length, const char* option, ... ) {
  printf("%s\n", title); // Print the title of the screen
  va_list valist;
  va_start(valist, option);
  int current = 1; // Start counting the options

  while (current <= length) { // Iterate over all options
    printf("%d. %s\n", current, option); // Print them
    option = va_arg(valist, const char*); // Move to the next
    current ++;
  }
  va_end(valist); // Clear the va_list

  int answer = 0;
  char buff[100];
  int err = 0;
  do {
    if (err) { // If there is an error, display the warning
      printf("That is not valid input!\n");
    }
    printf("Select an option by typing an integer: ");
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    answer = atoi(buff); // Parse it to an integer
    err = (answer <= 0 || answer > length); // Set the error flag
  } while(err);

  return handler(answer); // Call the function that is going to handle whatever option has been selected
}

int binaryOptionHandler(int answer) {
  return answer;
}

int binaryOption(char* option1, char* option2) {
  return newScreen("How would you like to proceed?", &binaryOptionHandler, 2, option1, option2);
}

void readFromFileScreen() {
  char* file;
  char buff[512];
  Calendar* c = NULL;
  ICalErrorCode e = OK;
  do {
    if (e != OK) { // If there is an error, display the warning
      const char* errorText = printError(e); // Get a human readable error message
      printf("Unable to parse the file %s because we recieved the error: %s.\n", file, errorText); // Display the error
      free((char*)errorText); // Free the error
      free(file); // Free the file
      if (binaryOption("Enter a new file name", "Exit") == 1) { // Offer the user the option to enter a new file name or exit
        printf("Please enter a new file name: "); // Ask for new file name
      } else {
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    } else {
      printf("Enter a file to read: "); // Ask the user to enter a file
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    file = calloc(strlen(buff) + 1, 1); // Allocate memory for file name
    strcpy(file, buff); // Copy the buffer
    file[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    e = createCalendar(file, &c); // Parse the calendar
  } while(e != OK); // If there was an error, continue
  char* toString = printCalendar(c);
  printf("%s\n", toString);
  free(toString);
  deleteCalendar(c); // Free calendar before returning
  free(file); // Free filename before returning
}

void createCalendarObject() {
  Calendar* c = calloc(sizeof(Calendar), 1);
  List events = initializeList(&printEventListFunction, &deleteEventListFunction, &compareEventListFunction);
  c->events = events; // Assign the empty event list

  List calProps = initializeList(&printPropertyListFunction, &deletePropertyListFunction, &comparePropertyListFunction);
  c->properties = calProps;

  int valid = 1;
  char buff[512]; // Buffer for user input

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a product ID: ");
    } else {
      printf("'%s' is not a valid product ID\n", userInput);
      if (binaryOption("Enter a new product ID", "Exit") == 1) { // Offer the user the option to enter a new product id or exit
        printf("Please enter a new product ID: "); // Ask for new product id
      } else {
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = matchTEXTField(userInput);
    if (valid) {
      strcpy(c->prodID, userInput);
    }
  } while(!valid);

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a version: ");
    } else {
      printf("'%s' is not a valid version\n", userInput);
      if (binaryOption("Enter a new version", "Exit") == 1) { // Offer the user the option to enter a version or exit
        printf("Please enter a new version: "); // Ask for new version
      } else {
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = match(userInput, "^[[:digit:]]+(\\.[[:digit:]]+)*$");
    if (valid) {
      c->version = atof(userInput);
    }
  } while(!valid);

  Event* event = newEmptyEvent(); // Make an empty event

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a event UID: ");
    } else {
      printf("'%s' is not a valid event UID\n", userInput);
      if (binaryOption("Enter a new event UID", "Exit") == 1) { // Offer the user the option to enter a event UID or exit
        printf("Please enter a new event UID: "); // Ask for new event UID
      } else {
        events.deleteData(event);
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = matchTEXTField(userInput);
    if (valid) {
      strcpy(event->UID, userInput);
    }
  } while(!valid);

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a date: ");
    } else {
      printf("'%s' is not a valid date\n", userInput);
      if (binaryOption("Enter a new date", "Exit") == 1) { // Offer the user the option to enter a date or exit
        printf("Please enter a new date: "); // Ask for new event UID
      } else {
        events.deleteData(event);
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = matchDATEField(userInput);
    if (valid) {
      createTime(event, userInput);
    }
  } while(!valid);

  List alarms = event->alarms;
  Alarm* alarm = calloc(sizeof(Alarm), 1);
  alarm->properties = initializeList(&printPropertyListFunction, &deletePropertyListFunction, &comparePropertyListFunction);

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a alarm ACTION: ");
    } else {
      printf("'%s' is not a valid alarm ACTION\n", userInput);
      if (binaryOption("Enter a new alarm ACTION", "Exit") == 1) { // Offer the user the option to enter a event UID or exit
        printf("Please enter a new ACTION: "); // Ask for new event UID
      } else {
        alarms.deleteData(alarm);
        events.deleteData(event);
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = match(userInput, "^(AUDIO|DISPLAY|EMAIL)$");
    if (valid) {
      strcpy(alarm->action, userInput);
    }
  } while(!valid);

  do {
    char userInput[512];
    if (valid) {
      printf("Enter a alarm TRIGGER: ");
    } else {
      printf("'%s' is not a valid alarm TRIGGER\n", userInput);
      if (binaryOption("Enter a new alarm TRIGGER", "Exit") == 1) { // Offer the user the option to enter a event UID or exit
        printf("Please enter a new TRIGGER: "); // Ask for new event UID
      } else {
        alarms.deleteData(alarm);
        events.deleteData(event);
        deleteCalendar(c);
        printf("Goodbye :)\n"); // Bye :)
        return; // Break out of this function
      }
    }
    fgets(buff, sizeof(buff), stdin); // Get the user's input
    strcpy(userInput, buff); // Copy the buffer
    userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
    valid = matchDURATIONField(userInput);
    if (valid) {
      alarm->trigger = calloc(strlen(userInput) * sizeof(char) + 1, 1);
      strcpy(alarm->trigger, userInput);
    }
  } while(!valid);
  insertBack(&alarms, alarm);
  event->alarms = alarms;
  insertBack(&events, event);
  c->events = events;


  char userInput[512];
  int fileError = 0;
  do {
    do {
      if (valid) {
        printf("Enter a file to write to: ");
      } else {
        printf("'%s' already exists!\n", userInput);
        if (binaryOption("Enter a new file name", "Overwrite this file") == 1) { // Offer the user the option to enter a date or exit
          printf("Please enter a new file: "); // Ask for new event UID
        } else {
          break;
        }
      }
      fgets(buff, sizeof(buff), stdin); // Get the user's input
      strcpy(userInput, buff); // Copy the buffer
      userInput[strlen(buff) - 1] = '\0'; // remove the new line char and replace it with null terminator
      valid = !fileExists(userInput); // If the file exists then we need to ask the user waht they want to do

    } while(!valid);

    ICalErrorCode writeError = writeCalendar(userInput, c);
    if (writeError != OK) {
      const char* error = printError(writeError);
      printf("Failed to write file '%s' because of error: %s\n", userInput, error);
      free((char*) error);
      if (writeError != INV_FILE) {
        break; // There was an error other than invalid file so we break out
      }
      fileError = 1;
    } else {
      printf("Wrote %s successfully!\n", userInput);
      fileError = 0;
    }
  } while (fileError);

  deleteCalendar(c);
}

int homeScreen(int answer) {
  switch (answer) {
    case 1:
      readFromFileScreen();
      newScreen("iCalendar Home", &homeScreen, 3, "Read iCalendar from file", "New Calendar Object", "Exit"); // Open the home screen
      break;
    case 2:
      createCalendarObject();
      newScreen("iCalendar Home", &homeScreen, 3, "Read iCalendar from file", "New Calendar Object", "Exit"); // Open the home screen
      break;
    case 3:
      break; // Exit the application
    default:
      printf("%d is an unimplemented screen\n", answer);
  }
  return 1;
}
