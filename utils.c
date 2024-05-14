#include "defs.h"

/*
    Returns a pseudo randomly generated number, in the range min to (max - 1), inclusively
*/
int randInt(int min, int max) {
    return (int) randFloat(min, max);
}

/*
    Returns a pseudo randomly generated floating point number.
    A few tricks to make this thread safe, just to reduce any chance of issues using random
*/
float randFloat(float min, float max) {
    static __thread unsigned int seed = 0;
    if (seed == 0) {
        seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();
    }

    float random = ((float) rand_r(&seed)) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

/*
    Returns the enum EvidenceType representation of the given string.
*/
enum EvidenceType stringToEvidence(char* str) {
    if (!strcmp(str, "EMF")) {
        return EMF;
    } else if (!strcmp(str, "TEMPERATURE")) {
        return TEMPERATURE;
    } else if (!strcmp(str, "FINGERPRINTS")) {
        return FINGERPRINTS;
    } else if (!strcmp(str, "SOUND")) {
        return SOUND;
    } else {
        return EV_UNKNOWN;
    }
}

/*  Function: void printResults(HouseType* house)
    Purpose: Prints the results of the simulation to the console, including
        the winner of the simulation, the ghost, the hunters, and the evidence 
        that was collected 
*/
void printResults(HouseType* house) {
    enum EvidenceType uniqueEvidence[EV_COUNT] = {0};

    // Print the results title
    printf("\n========================================\n");
    printf("  All done! Let's tally the results...  \n");
    printf("========================================\n");

    // Print relevant data
    printGhost(house);
    printHunters(house);
    printEvidence(house, uniqueEvidence);

    // Determine Winner
    if (huntersWin(house, uniqueEvidence)) {
        printf("            Hunter's Win!!\n");
    } else {
        printf("             Ghost Wins!!\n");
    }
}

/*  Function: printGhost(GhostType* ghost)
    Purpose: Prints the ghost type to the console
*/
void printGhost(HouseType* house) {
    // Get string representation of ghost type
    char ghost_str[MAX_STR];
    ghostToString(house->ghost->type, ghost_str);

    // Print the ghost type
    printf("Ghost:\n");
    printf("    * %s has boredom %d\n", ghost_str, house->ghost->boredom);
    printf("----------------------------------------\n");
}

/* 
    Returns the string representation of the given enum GhostClass.
*/
void ghostToString(enum GhostClass ghost, char* buffer) {
    switch(ghost) {
        case BANSHEE:
            strcpy(buffer, "Banshee");
            break;
        case BULLIES:
            strcpy(buffer, "Bullies");
            break;
        case PHANTOM:
            strcpy(buffer, "Phantom");
            break;
        case POLTERGEIST:
            strcpy(buffer, "Poltergeist");
            break;
        default:
            strcpy(buffer, "Unknown");
            break;
    }
}

/*  Function: int printHunters(HouseType* house)
    Purpose: Prints each hunter each hunter and their associated fear and boredom
*/
void printHunters(HouseType* house) {
    // Loop though each hunter and print
    printf("Hunters:\n");
    for (int i = 0; i < house->hunters.size; i++) {
        printf("    * %s has fear %d and boredom %d\n", house->hunters.elements[i]->name, house->hunters.elements[i]->fear, house->hunters.elements[i]->boredom);
    }
    printf("----------------------------------------\n");
}

/*  Function: void printEvidence(HouseType* house)
    Purpose: Prints all the unique evidence that was collected by the hunters during the
        simulation
*/
void printEvidence(HouseType* house, enum EvidenceType* uniqueEvidence) {
    // Initialize variables
    char evidenceName[MAX_STR];
    
    // Loop over evidence list and the evidence collected
    EvidenceNode* current = house->evidence.head;
    while (current != NULL) {
        uniqueEvidence[current->data] = C_TRUE;  // record if evidence type was collected
        current = current->next;
    }
    // Loop over each node in the evidence array and print the evidence
    printf("Evidence Collected:\n");
    for (int i = 0; i < EV_COUNT; i++) {
        if (uniqueEvidence[i]) {
            evidenceToString(i, evidenceName);
            printf("    * %s\n", evidenceName); 
        }
    }
    printf("----------------------------------------\n");
}

/*
    Returns the string representation of the given enum EvidenceType.
*/
void evidenceToString(enum EvidenceType type, char* str) {
    switch (type) {
        case EMF:
            strcpy(str, "EMF");
            break;
        case TEMPERATURE:
            strcpy(str, "TEMPERATURE");
            break;
        case FINGERPRINTS:
            strcpy(str, "FINGERPRINTS");
            break;
        case SOUND:
            strcpy(str, "SOUND");
            break;
        default:
            strcpy(str, "UNKNOWN");
            break;
    }
}

/*  Function: int huntersWin(HouseType* house, enum EvidenceType* uniqueEvidence)
    Purpose: Prints all the evidence that was collected by the hunters during the
        simulation
*/
int huntersWin(HouseType* house, enum EvidenceType* evidenceArray) {
    enum GhostClass guess;

    // Using the evidence determine the ghost type 
    if (evidenceArray[0] == 1 && evidenceArray[1] == 1 && evidenceArray[2] == 1) {
        guess = POLTERGEIST;
    } else if (evidenceArray[0] == 1 && evidenceArray[1] == 1 && evidenceArray[3] == 1) {
        guess = BANSHEE;
    } else if (evidenceArray[0] == 1 && evidenceArray[2] == 1 && evidenceArray[3] == 1) {
        guess = BULLIES;
    } else if (evidenceArray[1] == 1 && evidenceArray[2] == 1 && evidenceArray[3] == 1) {
        guess = PHANTOM;
    } else {
        guess = GH_UNKNOWN;
    };

    // If guess correct, return true or return false
    if (guess == house->ghost->type) {
        return C_TRUE;
    }
    return C_FALSE;
}
