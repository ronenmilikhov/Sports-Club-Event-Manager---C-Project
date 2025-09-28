# Sports Club Event Manager (C)

A console program that manages athletes (“sportsmen”) and their competition events using plain-text files for storage. It loads initial data from SportsmanData.txt and EventData.txt, lets you add athletes, add events, list a player’s events, count how many players participated in a given event/year, detect duplicate events, show the “best” club by number of events, print a club’s events sorted, rename a club across all athletes, and delete events.
Under the hood it demonstrates dynamic memory management, structs with nested dynamic arrays, robust user input validation, and file I/O in C.

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
