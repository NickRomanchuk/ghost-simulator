#include "defs.h"

/* 
    Logs the hunter being created.
*/
void l_hunterInit(char* hunter, enum EvidenceType equipment) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(equipment, ev_str);
    printf("[HUNTER INIT] [%s] is a [%s] hunter\n", hunter, ev_str);    
}

/*
    Logs the hunter moving into a new room.
*/
void l_hunterMove(char* hunter, char* room) {
    if (!LOGGING) return;
    printf("[HUNTER MOVE] [%s] has moved into [%s]\n", hunter, room);
}

/*
    Logs the hunter exiting the house.
*/
void l_hunterExit(char* hunter, enum LoggerDetails reason) {
    if (!LOGGING) return;
    printf("[HUNTER EXIT] [%s] exited because ", hunter);
    switch (reason) {
        case LOG_FEAR:
            printf("[FEAR]\n");
            break;
        case LOG_BORED:
            printf("[BORED]\n");
            break;
        case LOG_EVIDENCE:
            printf("[EVIDENCE]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
    }
}

/*
    Logs the hunter reviewing evidence.
*/
void l_hunterReview(char* hunter, enum LoggerDetails result) {
    if (!LOGGING) return;
    printf("[HUNTER REVIEW] [%s] reviewed evidence and found ", hunter);
    switch (result) {
        case LOG_SUFFICIENT:
            printf("[SUFFICIENT]\n");
            break;
        case LOG_INSUFFICIENT:
            printf("[INSUFFICIENT]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
    }
}

/*
    Logs the hunter collecting evidence.
*/
void l_hunterCollect(char* hunter, enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(evidence, ev_str);
    printf("[HUNTER EVIDENCE] [%s] found [%s] in [%s] and [COLLECTED]\n", hunter, ev_str, room);
}

/*
    Logs the ghost moving into a new room.
*/
void l_ghostMove(char* room) {
    if (!LOGGING) return;
    printf("[GHOST MOVE] Ghost has moved into [%s]\n", room);
}

/*
    Logs the ghost exiting the house.
*/
void l_ghostExit(enum LoggerDetails reason) {
    if (!LOGGING) return;
    printf("[GHOST EXIT] Exited because ");
    switch (reason) {
        case LOG_FEAR:
            printf("[FEAR]\n");
            break;
        case LOG_BORED:
            printf("[BORED]\n");
            break;
        case LOG_EVIDENCE:
            printf("[EVIDENCE]\n");
            break;
        default:
            printf("[UNKNOWN]\n");
    }
}

/*
    Logs the ghost leaving evidence in a room.
*/
void l_ghostEvidence(enum EvidenceType evidence, char* room) {
    if (!LOGGING) return;
    char ev_str[MAX_STR];
    evidenceToString(evidence, ev_str);
    printf("[GHOST EVIDENCE] Ghost left [%s] in [%s]\n", ev_str, room);
}

/*
    Logs the ghost being created.
*/
void l_ghostInit(enum GhostClass ghost, char* room) {
    if (!LOGGING) return;
    char ghost_str[MAX_STR];
    ghostToString(ghost, ghost_str);
    printf("[GHOST INIT] Ghost is a [%s] in room [%s]\n", ghost_str, room);
}