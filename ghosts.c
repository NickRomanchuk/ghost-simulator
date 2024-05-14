#include "defs.h"

/*  Function: void initGhost(HouseType* house, GhostType** ghost)
    Purpose: Initalizes a ghost type structure, allocates space in the heap, then assigns a random 
            room (other than van), assigns a random ghost type, and sets boredom to 0. Logs that
            ghost has been initialized
*/
void initGhost(HouseType* house, GhostType** ghost) {
    //Allocate space for the ghost on the heap
    *ghost = malloc(sizeof(GhostType));

    // Initalize all the fields of the ghost
    (*ghost)->room = randomRoom(&(house->rooms), 1);
    (*ghost)->room->ghost = *ghost;
    (*ghost)->type = randomGhost();
    (*ghost)->boredom = 0;
    
    // Add ghost to the house
    house->ghost = *ghost;

    l_ghostInit((*ghost)->type, (*ghost)->room->name);
}

/*  Function: enum GhostClass randomGhost()
    Purpose: Returns a random ghost class from the avialable enum GhostClass
*/
enum GhostClass randomGhost() {
    return (enum GhostClass) randInt(0, GHOST_COUNT);
}

/*  Function: void *runGhost(void *ptr)
    Purpose: Function that simulates a ghost interacting with a house type structure, ghost
        found at 'ptr' will randomly move rooms, leave evidence, or do nothing. Function ends
        when ghost gets bored (i.e., boredom increases when ghost isn't in a room with a hunter)  
*/
void *runGhost(void* ptr) {
    GhostType* ghost = (GhostType*) ptr;

    // While the ghost isnt bored
    while (ghost->boredom < BOREDOM_MAX) {

        // If ghost is with hunter, set boredom to 0, otherwise increment
        if (ghostWithHunter(ghost)) {
            ghost->boredom = 0;
        } else {
            ghost->boredom++;
        }
        
        // Randomly select action, call corresponding function
        switch (randomGhostAction()) {
            case MOVE_ROOMS:
                moveGhostRooms(ghost);
                break;
            case LEAVE_EVIDENCE:
                leaveEvidence(ghost);
                break;
            default:
                break;
        }
        
        // Sleep at the end of turn
        usleep(GHOST_WAIT);
    }

    // If ghost bored exit the thread
    sem_wait(&(ghost->room->sem));
    ghost->room->ghost = NULL;
    l_ghostExit(LOG_BORED);
    sem_post(&(ghost->room->sem));
    pthread_exit(NULL);
}

/*  Function: int ghostWithHunter(GhostType* ghost)
    Purpose: Returns one if there are hunters in the same room as the provided ghost 
            and a 0 otherwise
*/
int ghostWithHunter(GhostType* ghost) {
    // If there are more than 0 hunters return true
    if (ghost->room->hunters.size > 0) {
        return C_TRUE;
    }
    // otherwise, return false
    return C_FALSE;
}

/*  Function: enum GhostActions randomGhostAction()
    Purpose: Returns a random ghost action enum, within the upper range specified by
            the stopIndex integer
*/
enum GhostActions randomGhostAction() {
    return (enum GhostActions) randInt(0, GA_COUNT);
}

/*  Function: void moveGhostRooms(GhostType* ghost)
    Purpose: Moves the ghost pointer from their current room to a random room from the 
            avialable connected rooms, logs that the ghost moved
*/
void moveGhostRooms(GhostType* ghost) {
    // Store the old and new room pointers
    RoomType* oldRoom = ghost->room;
    RoomType* newRoom = randomRoom(&(ghost->room->connectedRooms), 0);

    // Wait semaphore until ghost has moved rooms
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_wait(&(oldRoom->sem));
        sem_wait(&(newRoom->sem));
    } else {
        sem_wait(&(newRoom->sem));
        sem_wait(&(oldRoom->sem));
    }

    ghost->room->ghost = NULL;      // Set old room ghost pointer to null
    ghost->room = newRoom;          // assign new room
    ghost->room->ghost = ghost;     // Set new room ghost pointer to ghost
    l_ghostMove(ghost->room->name); // Log that ghost moved

    // Post the semaphore now that the ghost has moved
    if (&(oldRoom->sem) > &(newRoom->sem)) {
        sem_post(&(oldRoom->sem));
        sem_post(&(newRoom->sem));
    } else {
        sem_post(&(newRoom->sem));
        sem_post(&(oldRoom->sem));
    }
};

/*  Function: void leaveEvidence(GhostType* ghost)
    Purpose: Selects a random evidence type, adds the evidence to the room's list of
            evidence and logs that evidence was left
*/
void leaveEvidence(GhostType* ghost) {
    // Pick evidence to leave based on ghost type
    EvidenceType evidence = pickEvidence(ghost->type);

    // Add evidence to the room
    sem_wait(&(ghost->room->evidence.sem));
    addEvidence(&(ghost->room->evidence), evidence); 
    sem_post(&(ghost->room->evidence.sem));

    // Log that evidence was added
    l_ghostEvidence(evidence, ghost->room->name);
}