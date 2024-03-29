#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     500
#define GHOST_WAIT      500
#define NUM_HUNTERS     4
#define NUM_GHOSTS      1
#define NUM_GHOST_EV    3
#define FEAR_MAX        10
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum GhostActions  { NOTHING, LEAVE_EVIDENCE, MOVE_ROOMS, GA_COUNT };
enum HunterActions { COLLECTING, MOVING, REVIEWING, HA_COUNT };
enum EvidenceType  { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass    { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

/* These rename the structures that we'll be creating.*/
typedef struct House        HouseType; 
typedef struct Hunter       HunterType;
typedef struct Room         RoomType;
typedef struct Ghost        GhostType;
typedef struct HunterArray  HunterArray;
typedef struct RoomNode     RoomNode;
typedef struct RoomList     RoomList;
typedef struct EvidenceNode EvidenceNode;
typedef struct EvidenceList EvidenceList;

struct RoomList { 
    RoomNode* head;                 //  first node in room linkedlist
    RoomNode* tail;                 //  last node in room linkedlist
    int size;
};

struct EvidenceList { 
    EvidenceNode* head;                // first node in evidence linked list
    EvidenceNode* tail;                // last node in evidence linked list
    sem_t sem;                         // semaphore
};

struct HunterArray {
    HunterType* elements[NUM_HUNTERS]; // collection of pointers to hunters
    int         size;                  // size of the array of pointers
};

struct Room {
    char         name[MAX_STR];     // room name
    RoomList     connectedRooms;    // linked list connected rooms
    EvidenceList evidence;          // linked list evidence
    HunterArray  hunters;           // collection of pointers to hunters in room
    GhostType*   ghost;             // pointer to the ghost if in room
    sem_t        sem;               // semaphore
};

struct RoomNode {
    RoomType*        data;          // Pointer to the room  
    struct RoomNode* next;          // Pointer to the next element in the linked list
};

struct House {
    HunterArray  hunters;           // collection of pointers to all the hunters
    RoomList     rooms;             // linked list of all rooms in house
    EvidenceList evidence;          // all the shared evidence the hunters have collected
    GhostType*   ghost;             // pointer to ghost in house
};

struct Ghost {
    GhostClass type;                // enumerate type representing what kind of ghost it is
    RoomType*  room;                // pointer to the room the ghost is in
    int        boredom;             // boredom timer
};

struct Hunter {
    RoomType*     room;             // pointer to room they are currently in
    EvidenceType  equipment;        // enumerated type representing type of evidence they can collect
    char          name[MAX_STR];    // name of hunter
    EvidenceList* evidence;         // pointer to shared collection of evicence (i.e., in house)
    int           fear;             // counter for fear
    int           boredom;          // counter for boredom
};

struct EvidenceNode {
    EvidenceType         data;         // enumerated evidence type
    struct EvidenceNode* next;         // pointer to next node
};

// Room Functions
RoomType* createRoom(char* name);
void initRoomList(RoomList *list);
RoomType* randomRoom(RoomList* rooms, int startIndex);
void connectRooms(RoomType* room1, RoomType* room2);
void addRoom(RoomList* list, RoomType *room);
void addRoomEvidence(EvidenceList* list, EvidenceType evidence);
void cleanRoomData(RoomList* list);
void cleanRoomList(RoomList* list);

//House Functions
void initHouse(HouseType* house);
void populateRooms(HouseType* house);
void cleanUp(HouseType* house);

//Evidence Functions
void initEvidenceList(EvidenceList *list);
void addEvidence(EvidenceList *list, EvidenceType evidence);
int removeEvidence(EvidenceList* list, enum EvidenceType equipment);
void cleanEvidenceList(EvidenceList* list);

//Hunter Functions
void *runHunter(void *ptr);
void initHunterArray(HunterArray* arr);
void initHunter(RoomType* room, EvidenceType equipment, EvidenceList* evidence, char* name, HunterType** hunter);
void addHunter(HunterArray* arr, HunterType* hunter);
enum HunterActions randomHunterAction();
void collectEvidence(HunterType* hunter);
int sufficientEvidence(EvidenceList* list);
void moveHunterRooms(HunterType* hunter);
void removeHunter(HunterArray* arr, HunterType* hunter);
void cleanHunters(HunterArray hunters);

//Ghost Functions
void *runGhost(void *ghostArg);
void initGhost(HouseType* house, GhostType** ghost);
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
int ghostWithHunter(GhostType* ghost);
enum GhostActions randomGhostAction(int stopIndex);
void moveGhostRooms(GhostType* ghost);
void leaveEvidence(GhostType* ghost);
enum EvidenceType pickEvidence(enum GhostClass class);

// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter
EvidenceType stringToEvidence(char* str);
void endSimulation(HouseType* house);
int printHunters(HouseType* house);
void printEvidence(HouseType* house);
void printResult(HouseType* house);
void printGhostType(GhostType* ghost);

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);