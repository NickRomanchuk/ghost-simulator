#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <curses.h>
#include <time.h>

#define C_TRUE          1
#define C_FALSE         0
#define MAX_STR         64
#define BOREDOM_MAX     50
#define FEAR_MAX        10
#define HUNTER_WAIT     50000
#define GHOST_WAIT      20000
#define NUM_HUNTERS     4
#define NUM_GHOSTS      1
#define NUM_GHOST_EV    3
#define LOGGING         C_TRUE

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum GhostActions  { NOTHING, LEAVE_EVIDENCE, MOVE_ROOMS, GA_COUNT };
enum HunterActions { COLLECTING, MOVING, REVIEWING, HA_COUNT };
enum EvidenceType  { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass    { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

/* These rename the structures that we'll be creating.*/
typedef struct RoomList     RoomList;
typedef struct RoomNode     RoomNode;
typedef struct Room         RoomType;
typedef struct HunterArray  HunterArray;
typedef struct Hunter       HunterType;
typedef struct EvidenceList EvidenceList;
typedef struct EvidenceNode EvidenceNode;
typedef struct Ghost        GhostType;
typedef struct House        HouseType; 

struct EvidenceNode {
    EvidenceType         data;         // enumerated evidence type
    struct EvidenceNode* next;         // pointer to next node
};

struct EvidenceList { 
    EvidenceNode* head;                // first node in evidence linked list
    EvidenceNode* tail;                // last node in evidence linked list
    int           sufficentEv;         // flag to see if other threads should exit
    sem_t         sem;                 // semaphore
};

struct Hunter {
    RoomType*     room;             // pointer to room they are currently in
    EvidenceType  equipment;        // enumerated type representing type of evidence they can collect
    char          name[MAX_STR];    // name of hunter
    EvidenceList* evidence;         // pointer to shared collection of evicence (i.e., in house)
    int           fear;             // counter for fear
    int           boredom;          // counter for boredom
};

struct HunterArray {
    HunterType* elements[NUM_HUNTERS]; // collection of pointers to hunters
    int         size;                  // size of the array of pointers
};

struct Ghost {
    GhostClass type;                // enumerate type representing what kind of ghost it is
    RoomType*  room;                // pointer to the room the ghost is in
    int        boredom;             // boredom timer
};

struct RoomList { 
    RoomNode* head;                 //  first node in room linkedlist
    RoomNode* tail;                 //  last node in room linkedlist
    int size;
};

struct RoomNode {
    RoomType*        data;          // Pointer to the room  
    struct RoomNode* next;          // Pointer to the next element in the linked list
};

struct Room {
    char         name[MAX_STR];     // room name
    RoomList     connectedRooms;    // linked list connected rooms
    EvidenceList evidence;          // linked list evidence
    HunterArray  hunters;           // collection of pointers to hunters in room
    GhostType*   ghost;             // pointer to the ghost if in room
    sem_t        sem;               // semaphore
};

struct House {
    HunterArray  hunters;           // collection of pointers to all the hunters
    RoomList     rooms;             // linked list of all rooms in house
    EvidenceList evidence;          // all the shared evidence the hunters have collected
    GhostType*   ghost;             // pointer to ghost in house
};

//House Functions
void initHouse(HouseType*);
void populateRooms(HouseType*);
void cleanUp(HouseType*);

// Room Functions
RoomType* createRoom(char*);
void initRoomList(RoomList*);
void connectRooms(RoomType*, RoomType*);
void addRoom(RoomList*, RoomType*);
void cleanRoomData(RoomList*);
void cleanRoomList(RoomList*);
RoomType* randomRoom(RoomList*, int);

//Evidence Functions
void initEvidenceList(EvidenceList*);
void addEvidence(EvidenceList*, enum EvidenceType);
int removeEvidence(EvidenceList*, enum EvidenceType);
enum EvidenceType pickEvidence(enum GhostClass);
void cleanEvidenceList(EvidenceList*);

//Ghost Functions
void initGhost(HouseType*, GhostType**);
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void *runGhost(void*);
int ghostWithHunter(GhostType*);
enum GhostActions randomGhostAction();
void moveGhostRooms(GhostType*);
void leaveEvidence(GhostType*);

//Hunter Functions
void initHunterArray(HunterArray*);
void initHunter(RoomType*, enum EvidenceType, EvidenceList*, char*, HunterType**);
void addHunter(HunterArray*, HunterType*);
void *runHunter(void*);
enum HunterActions randomHunterAction();
void collectEvidence(HunterType*);
void moveHunterRooms(HunterType*);
void removeHunter(HunterArray*, HunterType*);
void reviewEvidence(HunterType*);
int sufficientEvidence(EvidenceList*);
void cleanHunters(HunterArray*);

// Utilitiy helpers
int randInt(int,int);
float randFloat(float, float);
EvidenceType stringToEvidence(char*);
void printResults(HouseType*);
void printGhost(HouseType*);
void ghostToString(enum GhostClass, char*);
void printHunters(HouseType*);
void printEvidence(HouseType*, enum EvidenceType*);
void evidenceToString(enum EvidenceType, char*);
int huntersWin(HouseType*, enum EvidenceType*);

// Logging Utilities
void l_hunterInit(char*, enum EvidenceType);
void l_hunterMove(char*, char*);
void l_hunterReview(char*, enum LoggerDetails);
void l_hunterCollect(char*, enum EvidenceType, char*);
void l_hunterExit(char*, enum LoggerDetails);
void l_ghostInit(enum GhostClass, char*);
void l_ghostMove(char*);
void l_ghostEvidence(enum EvidenceType, char*);
void l_ghostExit(enum LoggerDetails);