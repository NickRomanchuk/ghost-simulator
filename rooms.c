#include "defs.h"

/*  Function: RoomType* createRoom(char* name)
    Purpose: Initializes a room structures, allocats memory on the heap, initilzes all the fields
            and returns a pointer the the structure in the heap
        in/out:   name   - name of room to initialize
    return:   pointer to a room structure in the heap
*/
RoomType* createRoom(char* name) {
    // Allocate space in heap for new room structure
    RoomType* room = malloc(sizeof(RoomType));
    
    // Initialize all fields on room structure
    strcpy(room->name, name);
    initRoomList(&(room->connectedRooms));
    initEvidenceList(&(room->evidence));           
    initHunterArray(&(room->hunters));         
    room->ghost = NULL;
    sem_init(&(room->sem), 0, 1);

    //Return pointer to room structure on heap
    return room;
}

/*  Function: void initRoomList(RoomList *list)
    Purpose: Initializes a linked list of room structures, sets the head and tail to NULL and size to 0
    in/out:   list      - pointer to linked list of room structures to initialize
    return:   N/A
*/
void initRoomList(RoomList *list) {
    list->head = NULL;   //Set head of linked list to NULL
    list->tail = NULL;   //Set tail of linked list to NULL
    list->size = 0;      //Set size of linked list to 0
};

/*  Function: RoomType* randomRoom(RoomList* rooms, int startIndex)
    Purpose: Returns a pointer to a randomly choosen room structure from the
            the provided room list at the pointer 'rooms'. Randomly selects a 
            room starting at the provided startIndex
    in/out:   rooms      - pointer to linked list of room structures to randomly choose from
        in:   startIndex - the starting index for which rooms to randomly select
    return:   Returns a pointer to a randomly choosen room structure
*/
RoomType* randomRoom(RoomList* rooms, int startIndex) {
    // Set current node to start of room list
    RoomNode* current = rooms->head;

    // Randomly choose an index from avialable rooms
    int roomNum = randInt(startIndex, rooms->size);
    
    // Traverse linked list until index reached
    int i = 0;
    current = rooms->head;
    while (i < roomNum) {
        current = current->next;
        i++;
    } 
    
    // return the pointer to the room at randomly choose index
    return current->data;
}

/*  Function: void connectRooms(RoomType* room1, RoomType* room2)
    Purpose: Adds the room at pointer 'room1' to the linked list of connected rooms in the room
            structure at pointer 'room2', and vise-versa
    in/out:   room1 - pointer to room structure to add to 'room2'
    in/out:   room2 - pointer to room structure to add to 'room1'
    return:   N/A 
*/
void connectRooms(RoomType* room1, RoomType* room2) {
    // add room2 to room1 connectedRoom list
    addRoom(&(room1->connectedRooms), room2);

    // add room1 to room2 connectedRoom list
    addRoom(&(room2->connectedRooms), room1);
}

/*  Function: void addRoom(RoomList *list, RoomType *room)
    Purpose: Allocates a new room node on the heap and sets data to be the provided 'room' pointer,
            adds the room node to the back of the provided room linked list found at the pointer 'list',
            increments size of list
        in:   room - pointer to room structure to add to the node data
    in/out:   list - pointer to to the linked list of rooms to add to
    return:   N/A 
*/
void addRoom(RoomList* list, RoomType *room) {
    // Allocate memory on heap for node structure
    RoomNode* new = malloc(sizeof(RoomNode));

    // Set data for node to be provided room and next pointer to null
    new->data = room;
    new->next = NULL;

    // Add new node to back of the list
    if (list->head == NULL) {
        list->head = new;
    } else if (list->tail == NULL) {
        list->tail = new;
        list->head->next = new;
    } else {
        list->tail->next = new;
        list->tail = new;
    }
    list->size++;
}

/*  Function: void addRoomEvidence(EvidenceList* list, EvidenceType evidence)
    Purpose: Determines if 'evidence' already in evidence list 'list', if not adds the evidence to 
        the back of the list 
      in/out:   list - pointer to the evidence list to add to
          in:   evidence - evidence to add to back of list
    return: N/A
*/
void addRoomEvidence(EvidenceList* list, EvidenceType evidence) {
    // Initialize values
    EvidenceNode* current = list->head;
    int hasEvidence = C_FALSE;
    
    // Loop through list to see if evidence present
    while (current != NULL) {
        if (current->data == evidence) {
            hasEvidence = C_TRUE;
            break;
        }
        current = current->next;
    }

    // If not present add evidence to the back of the list
    if (!hasEvidence) {
        addEvidence(list, evidence);
    }
};

/* Function: void cleanRoomData(RoomList* list)
   Purpose:  Frees the allocated memory in the heap for the room type data in 
            the nodes of room list type at the memory address 'list'.
    in/out     list:   - Memory address of the room list type to free node memory
  return:   N/A
*/
void cleanRoomData(RoomList* list) {
    RoomNode* current = list->head;

    // Loop through every node in list and free room data
    while (current != NULL) {
        free(current->data);
        current = current->next;
    }
}

/*Function: cleanRoomList(RoomList* list)
  Purpose:  Frees all the allocated memory in the heap for the nodes of the room list 
        type at the memory address 'list'.
    in/out     list:    Memory address of the room list type to free nodes
  return:   N/A
*/
void cleanRoomList(RoomList* list) {
    RoomNode* current = list->head; // Set current node to be start of list (i.e., head)
    RoomNode* temp;                 // Declare new temporary node

    // Loop through every node in list and free the node
    while (current != NULL) {
        temp = current;             // Store current in temp
        current = current->next;    // increment current node
        free(temp);                 // free the temporary node
    }
}
