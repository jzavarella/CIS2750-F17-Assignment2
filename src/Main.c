#include <stdio.h>

#include "CalendarParser.h"
#include "HelperFunctions.h"

void test(char* fileName, ErrorCode expectedResult);
void testValidation(Calendar* c, char* testDescription, ErrorCode expectedResult);

int main(int argc, char const *argv[]) {


  printf("----FILE ERRORS:\n");
  test(NULL, INV_FILE); // Filename is NULL
  test("", INV_FILE); // Empty String
  test("tests/doesnt_exist.ics", INV_FILE);
  test("tests/no_permissions.ics", INV_FILE); // Make sure you remove permissions
  test("tests/no_file_extension", INV_FILE);
  printf("----CALENDAR ERRORS:\n");
  test("tests/blank.ics", INV_CAL);
  test("tests/just_open_tag.ics", INV_CAL);
  test("tests/just_close_tag.ics", INV_CAL);
  test("tests/just_open_close_tags.ics", INV_CAL);
  test("tests/no_prod_id.ics", INV_CAL);
  test("tests/no_version.ics", INV_CAL);
  test("tests/no_event.ics", INV_CAL);
  printf("----VERSION ERRORS:\n");
  test("tests/duplicate_version.ics", DUP_VER);
  test("tests/malformed_version.ics", INV_VER);
  test("tests/blank_version.ics", INV_VER);
  printf("----PROD ID ERRORS:\n");
  test("tests/duplicate_prod_id.ics", DUP_PRODID);
  test("tests/invalid_prod_id.ics", INV_PRODID);
  test("tests/blank_prod_id.ics", INV_PRODID);
  printf("----CREATEDT ERRORS:\n");
  test("tests/invalid_created_t.ics", INV_CREATEDT);
  test("tests/blank_created_t.ics", INV_EVENT);
  printf("----EVENT ERRORS:\n");
  test("tests/no_created_t.ics", INV_EVENT);
  test("tests/multiple_events_one_invalid.ics", INV_EVENT);
  printf("----ALARM ERRORS:\n");
  test("tests/no_alarm_trigger.ics", INV_ALARM);
  test("tests/no_alarm_action.ics", INV_ALARM);
  printf("----OK:\n");
  test("tests/valid_one_alarm.ics", OK);
  test("tests/valid_multiple_alarms.ics", OK);
  printf("----------------------\n");
  test("tests/valid_no_alarm.ics", OK);
  test("tests/valid_no_alarm_lowercase.ics", OK);
  test("tests/testCalShort.ics", OK);
  test("tests/multiple_events.ics", OK);
  test("tests/multiple_events_long.ics", OK);
  test("tests/valid_with_newlines.ics", OK);
  test("tests/testCalLong.ics", OK);
  printf("\n\n------VALIDATION ERRORS:\n");

  Calendar* ca = NULL;
  testValidation(ca, "NULL CALENDAR", OTHER_ERROR);

  ca = (Calendar*)calloc(sizeof(Calendar), 1);
  testValidation(ca, "UNALLOCATED VERSION", INV_VER);
  ca->version = 2.0;
  testValidation(ca, "UNALLOCATED PRODID", INV_CAL);
  strcpy(ca->prodID, "");
  testValidation(ca, "BLANK PRODID", INV_CAL);
  strcpy(ca->prodID, "\\\\\\\\");
  testValidation(ca, "MALFORMED PRODID", INV_PRODID);
  strcpy(ca->prodID, "valid"); // Valid PRODID
  printf("----Events----\n");
  testValidation(ca, "NO EVENTS", INV_CAL);
  List events = initializeList(&printEventListFunction, &deleteEventListFunction, &compareEventListFunction);
  Event* event = NULL; // NULL event
  insertBack(&events, event); // Insert
  ca->events = events;
  testValidation(ca, "NULL EVENT", INV_CAL);
  event = calloc(sizeof(Event), 1);
  insertBack(&events, event);
  ca->events = events;
  testValidation(ca, "UNALLOCATED EVENT", INV_EVENT);
  strcpy(event->UID, "");
  testValidation(ca, "BLANK UID", INV_EVENT);
  strcpy(event->UID, "valid"); // Valid UID
  DateTime dt = { .date="", .time="", false}; // Blanks
  event->creationDateTime = dt;
  printf("----DATE-TIME----\n");
  testValidation(ca, "BLANK DATE-TIME", INV_CREATEDT);
  strcpy(event->creationDateTime.date, "invalid");
  testValidation(ca, "INVALID DATE", INV_CREATEDT);
  strcpy(event->creationDateTime.date, "20171029"); // Valid date
  testValidation(ca, "INVALID TIME", INV_CREATEDT);
  strcpy(event->creationDateTime.time, "101010"); // Valid time
  printf("----PROPS----\n");
  List eventProps = initializeList(&printPropertyListFunction, &deletePropertyListFunction, &comparePropertyListFunction); // Create a list to store all properties
  event->properties = eventProps;
  Property* p = calloc(sizeof(Property) + 80, 1);
  insertBack(&(event->properties), p);
  testValidation(ca, "BLANK PROP", INV_EVENT);
  strcpy(p->propName, ""); // invalid prop name
  testValidation(ca, "BLANK PROPNAME", INV_EVENT);
  strcpy(p->propName, "HELLO"); // Valid prop name
  insertBack(&(event->properties), createProperty("yo", ""));
  testValidation(ca, "VALID EVENT NO ALARMS 1", OK);
  char propLine[strlen("TEST:PROP") + 1];
  strcpy(propLine, "TEST:PROP"); // Second prop
  Property* p2 = extractPropertyFromLine(propLine);
  insertBack(&(event->properties), p2);
  printf("----ALARMS----\n");
  testValidation(ca, "VALID EVENT NO ALARMS 2", OK);
  List alarmList = initializeList(&printAlarmListFunction, &deleteAlarmListFunction, &compareAlarmListFunction);
  event->alarms = alarmList;
  List alarmProps = initializeList(&printPropertyListFunction, &deletePropertyListFunction, &comparePropertyListFunction); // Create a list to store all properties
  Alarm* a = createAlarm("", "", alarmProps);
  insertBack(&(event->alarms), a);
  testValidation(ca, "NULL ALARM", INV_ALARM);
  strcpy(a->action, "invalid"); // invalid action
  testValidation(ca, "INVALID ACTION", INV_ALARM);
  strcpy(a->action, "AUDIO"); // valid action
  testValidation(ca, "BLANK/INVALID TRIGGER", INV_ALARM);
  free(a->trigger);
  a->trigger = calloc(strlen("valid") + 1, 1);
  strcpy(a->trigger, "valid"); // valid trigger
  testValidation(ca, "VALID ALARM NO PROPS", OK);
  Property* p3 = calloc(sizeof(Property) + 80, 1); // blank property
  insertBack(&(a->properties), p3);
  testValidation(ca, "BLANK ALARM PROP", INV_ALARM);
  strcpy(p3->propName, "VALID");
  testValidation(ca, "VALID ALARM PROP 1", OK);
  Property* p4 = extractPropertyFromLine(propLine);
  strcpy(propLine, "TEST:PROP");
  insertBack(&(a->properties), p4);
  testValidation(ca, "VALID ALARM PROP 2", OK);
  List calProps = initializeList(&printPropertyListFunction, &deletePropertyListFunction, &comparePropertyListFunction); // Create a list to store all properties
  ca->properties = calProps;
  Property* p5 = calloc(sizeof(Property) + 80, 1); // blank property
  insertBack(&(ca->properties), p5);
  testValidation(ca, "BLANK CAL PROP", OTHER_ERROR);
  strcpy(p5->propName, "VALID");
  testValidation(ca, "VALID CALENDAR PROP 1", OK);
  strcpy(propLine, "TEST:PROP");
  Property* p6 = extractPropertyFromLine(propLine);
  insertBack(&(ca->properties), p6);
  testValidation(ca, "VALID CALENDAR PROP 2", OK);

  // writeCalendar("cal.ics", ca);

  // char* tou = printCalendar(ca);
  // printf("%s\n", tou);
  // free(tou);
  deleteCalendar(ca);

  printf("\n");
  Calendar* c;
  ErrorCode e = createCalendar("tests/testCalEvtPropAlm.ics", &c);


  const char* errorText = printError(e);
  printf("%s\n", errorText);
  free((char*) errorText);
  const char* out = printCalendar(c);
  printf("%s\n", out);
  free((char*) out);
  testValidation(c, "VALID", OK);
  writeCalendar("cal.ics", c);
  test("cal.ics", OK);
  deleteCalendar(c);

  // Calendar* c;
  // ErrorCode e = createCalendar("tests/testCalSimpleNoUTC.ics", &c);
  //
  //
  // const char* errorText = printError(e);
  // printf("%s\n", errorText);
  // free((char*) errorText);
  // const char* out = printCalendar(c);
  // printf("%s\n", out);
  // free((char*) out);
  // testValidation(c, "VALID", OK);
  // writeCalendar("cal.ics", c);
  // test("cal.ics", OK);
  // deleteCalendar(c);
  return 0;
}

void testValidation(Calendar* c, char* testDescription, ErrorCode expectedResult) {
  ErrorCode e = validateCalendar(c);
  const char* expectedErrorText = printError(expectedResult);
  const char* errorText = printError(e);
  if (e != expectedResult) {
    printf("**FAIL**: (%s) %s was expected but recieved %s\n", testDescription, expectedErrorText, errorText);
  } else {
    printf("PASS: (%s) %s was expected\n", testDescription, expectedErrorText);
  }

  free((char*) expectedErrorText);
  free((char*) errorText);
}

void test(char* fileName, ErrorCode expectedResult) {
  Calendar* c;
  ErrorCode e = createCalendar(fileName, &c);

  const char* expectedErrorText = printError(expectedResult);
  const char* errorText = printError(e);
  if (e != expectedResult) {
    printf("**FAIL**: %s %s was expected but recieved %s\n", fileName, expectedErrorText, errorText);
  } else {
    if (e == OK) {
      char name[100];
      strcpy(name, "result/");
      strcat(name, fileName);
      ErrorCode writeErr = writeCalendar(name, c);
      if (writeErr != OK) {
        const char* writeErrS = printError(writeErr);
        printf("**FAIL**: %s failed write error with error %s\n", fileName, writeErrS);
        free((char*) writeErrS);
      } else {
        // deleteCalendar(c);
        ErrorCode valError = createCalendar(name, &c);
        if (valError != OK) {
          printf("**FAIL**: %s failed write error2\n", fileName);
        } else {
          printf("PASS: %s %s was expected and passed write error\n", fileName, expectedErrorText);
        }
      }
    } else {
      printf("PASS: %s %s was expected\n", fileName, expectedErrorText);
    }
  }
  free((char*) expectedErrorText);
  free((char*) errorText);
  if (c) {
    // deleteCalendar(c);
  }
}
