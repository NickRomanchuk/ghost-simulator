#include "defs.h"

int main() {
    //Initalize variables
    HouseType house;
    HunterType* hunter;
    GhostType* ghost;
    char name[MAX_STR];
    char equipment[MAX_STR];
    pthread_t threadIDS[NUM_HUNTERS + NUM_GHOSTS];

    // Initialize the random number generator
    srand(time(NULL));

    // Create the house
    initHouse(&house);
    populateRooms(&house);
    
    // Loop to create all the hunters
    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Enter the name of hunter %d: \n", i + 1);
        scanf("%s", name);
        while ((getchar()) != '\n');

        printf("What type of data should %s collect (EMF, TEMPERATURE, FINGERPRINTS, SOUND, UNKNOWN): \n", name);
        scanf("%s", equipment);
        while ((getchar()) != '\n');

        initHunter(house.rooms.head->data, stringToEvidence(equipment), &(house.evidence), name, &hunter);
        addHunter(&(house.hunters), hunter);
    }
 
    // Create the ghost and put them in the house structure        
    initGhost(&house, &ghost);
    house.ghost = ghost;
    
    // Create threads for the hunters and the ghost
    for (int i = 0; i < 5; i++) {
        if (i == 0){
            pthread_create(threadIDS + i, NULL, runGhost, ghost);
        } else {
            pthread_create(threadIDS + i, NULL, runHunter, house.hunters.elements[i - 1]);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++) { 
        pthread_join(threadIDS[i], NULL);
    }
    
    // End simulation and print results
    endSimulation(&house);
    
    // Clean up all memory used in the heap
    cleanUp(&house);

    return 0;
}