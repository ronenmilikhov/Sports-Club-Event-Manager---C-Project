# Sports Club Event Manager (C)

A menu-driven console app that manages athletes (“sportsmen”) and their competition events using plain-text files. It showcases dynamic memory management, structs, validation, and file I/O in C.

## Features
- **Load & save** data from `SportsmanData.txt` and `EventData.txt`
- **Add** a sportsman (ID, first/last name, club, gender)
- **Add** an event to a sportsman (title, location, year)
- **List** all events for a sportsman (by last name)
- **Count/print** all sportsmen who played a given event in a given year
- **Find duplicates**: check if a sportsman shares events with others
- **Best club**: club with the most total event participations
- **Print club events** sorted (title, location, year)
- **Bulk edit**: rename a club across all sportsmen
- **Delete** an event (by name+year) from **all** sportsmen

## How it works (at a glance)
- Core structs:  
  ```c
  typedef struct {
    char* p2title;     // event name
    char* p2location;  // event location
    int   year;        // event year
  } event;

  typedef struct {
    unsigned int id;
    char* p2Fname;
    char* p2Lname;
    char* p2club;
    int   gen;         // 0 = male, 1 = female
    int   Nevents;
    event* p2events;   // dynamic array
  } sportsman;
