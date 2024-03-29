#include "defs.h"

/*
    Returns a pseudo randomly generated number, in the range min to (max - 1), inclusively
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated integer in the range [0, max-1) 
*/
int randInt(int min, int max)
{
    return (int) randFloat(min, max);
}

/*
    Returns a pseudo randomly generated floating point number.
    A few tricks to make this thread safe, just to reduce any chance of issues using random
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated floating point number in the range [min, max)
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
    Returns the string representation of the given enum EvidenceType.
        in: type - the enum EvidenceType to convert
        out: str - the string representation of the given enum EvidenceType, minimum 16 characters
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

/*
    Returns the enum EvidenceType representation of the given string.
        in: str - the string representation of the given enum EvidenceType, minimum 16 characters
        out: type - the enum EvidenceType to convert
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

/* 
    Returns the string representation of the given enum GhostClass.
        in: ghost - the enum GhostClass to convert
        out: buffer - the string representation of the given enum GhostClass, minimum 16 characters
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

/*  Function: void endSimulation(HouseType* house)
    Purpose: Prints the results of the simulation to the console, including
        the winner of the simulation, which hunters left the house and why, 
        the evidence that was collected and hunter guess of the ghost type
        in:   house - pointer to house of our simulation
    return:   N/A 
*/
void endSimulation(HouseType* house) {
    int failedHunters;
    
    // Print the results title
    printf("======================================\n");
    printf("All done! Let's tally the results...\n");
    printf("======================================\n");

    // Print all the hunters and record how many hunters failed
    failedHunters = printHunters(house);

    // If all hunters failed print that the ghost won
    if (failedHunters >= NUM_HUNTERS) {
        printf("The ghost has won the game!\n");
        printGhostType(house->ghost);
    } else {
        // If we have hunters print if they collected enough evidence
        printResult(house);
    }

    // Print all the evidence that was collected
    printEvidence(house);
}

/*  Function: int printHunters(HouseType* house)
    Purpose: Prints each hunter that left the house due to fear and boredom, returns the
        number of hunters who failed in this way.
        in:   house - pointer to the house where our hunter array is found
    return:   N/A 
*/
int printHunters(HouseType* house) {
    int n = 0;

    // Loop though each hunter
    for (int i = 0; i < house->hunters.size; i++) {
        // If hunter left because of fear or boredom print hunter
        if (house->hunters.elements[i]->fear >= FEAR_MAX || house->hunters.elements[i]->boredom >= BOREDOM_MAX) {
            printf("    * %s has fear %d and boredom %d\n", house->hunters.elements[i]->name, house->hunters.elements[i]->fear, house->hunters.elements[i]->boredom);
            n++;
        }
    }
    printf("--------------------------------------\n");

    // Return the number of hunters who left house due to boredom and fear
    return n;
}

/*  Function: void printEvidence(HouseType* house)
    Purpose: Prints all the evidence that was collected by the hunters during the
        simulation
        in:   house - pointer to the house where our evidence list is found
    return:   N/A 
*/
void printEvidence(HouseType* house) {
    // Initialize variables
    EvidenceNode* current = house->evidence.head;
    char evidence[MAX_STR];

    // Loop over each node in the evidence array and print the evidence
    printf("The hunters have collected the following evidence:\n");
    while (current != NULL) {
        evidenceToString(current->data, evidence);
        printf("    * %s\n", evidence); 
        current = current->next;
    }
}

/*  Function: void printResult(HouseType* house)
    Purpose: Prints all the evidence that was collected by the hunters during the
        simulation
        in:   house - pointer to the house where our evidence list is found
    return:   N/A 
*/
void printResult(HouseType* house) {
    // Initialize variables
    EvidenceNode* current = house->evidence.head;
    int correct = C_FALSE;
    int temp[EV_COUNT] = {0};
    char str[MAX_STR];

    // Loop over evidence list and the evidence collected
    while (current != NULL) {
        temp[current->data] = 1;
        current = current->next;
    }

    // Using the evidence determine the ghost type 
    if (temp[0] == 1 && temp[1] == 1 && temp[2] == 1) {
        strcpy(str, "POLTERGEIST");
        correct = C_TRUE;
    } else if (temp[0] == 1 && temp[1] == 1 && temp[3] == 1) {
        strcpy(str, "BANSHEE");
        correct = C_TRUE;
    } else if (temp[0] == 1 && temp[2] == 1 && temp[3] == 1) {
        strcpy(str, "BULLIES");
        correct = C_TRUE;
    } else if (temp[1] == 1 && temp[2] == 1 && temp[3] == 1) {
        strcpy(str, "PHANTOM");
        correct = C_TRUE;
    };

    // Print the result
    if (correct) {
        printf("The hunters have won the game!\n");
        printf("Using the evidence they found, they correctly determined that the ghost is a %s.\n", str); 
    } else {
        printf("The hunters didn't have enough to determine that the ghost was a %s.\n", str); 
    }
}

/*  Function: printGhostType(GhostType* ghost)
    Purpose: Prints the ghost type to the console
        in:  ghost - pointer to ghost to print
    return:   N/A 
*/
void printGhostType(GhostType* ghost) {
    // Get the string representation of the ghost type
    char ghost_str[MAX_STR];
    ghostToString(ghost->type, ghost_str);

    // Print the ghost type
    printf("Ghost is a [%s], hunters didn't correctly guess the ghost.\n", ghost_str);
}
