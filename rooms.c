#include "defs.h"

/*  Function: RoomType* createRoom(char* name)
    Purpose: Initializes a room structure, allocats memory on the heap, initilzes all the fields
            and returns a pointer the the structure in the heap
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

    // Return pointer to room structure on heap
    return room;
}

/*  Function: void initRoomList(RoomList* list)
    Purpose: Initializes a linked list of room structures, sets the head and tail to NULL 
            and size to 0
*/
void initRoomList(RoomList* list) {
    list->head = NULL;   //Set head of linked list to NULL
    list->tail = NULL;   //Set tail of linked list to NULL
    list->size = 0;      //Set size of linked list to 0
};

/*  Function: void connectRooms(RoomType* room1, RoomType* room2)
    Purpose: Adds the room at pointer 'room1' to the linked list of connected rooms in the room
            structure at pointer 'room2', and vise-versa
*/
void connectRooms(RoomType* room1, RoomType* room2) {
    // add room2 to room1 connectedRoom list
    addRoom(&(room1->connectedRooms), room2);

    // add room1 to room2 connectedRoom list
    addRoom(&(room2->connectedRooms), room1);
}

/*  Function: void addRoom(RoomList *list, RoomType* room)
    Purpose: Allocates a new room node on the heap and sets data to be the provided 'room' pointer,
            adds the room node to the back of the provided room linked list found at the pointer 
            'list', increments size of list
*/
void addRoom(RoomList* list, RoomType* room) {
    // Allocate memory on heap for node structure
    RoomNode* new = malloc(sizeof(RoomNode));

    // Set data for node to be provided room and next pointer to null
    new->data = room;
    new->next = NULL;

    // Add new node to back of the list   
    if (list->head == NULL) { // If empty list
        list->head = new;
        list->tail = new;
    } else {                  // All other conditions
        list->tail->next = new;
        list->tail = new;
    }
    list->size++;
}

/* Function: void cleanRoomData(RoomList* list)
   Purpose:  Frees the allocated memory in the heap for the room type data in 
            the nodes of room list type at the memory address 'list'
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
        type at the memory address 'list'
*/
void cleanRoomList(RoomList* list) {
    RoomNode* current = list->head;
    RoomNode* temp;

    // Loop through every node in list and free the node
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}

/*  Function: RoomType* randomRoom(RoomList* rooms, int startIndex)
    Purpose: Returns a pointer to a randomly choosen room structure from the
            the provided room list at the pointer 'rooms'. Randomly selects a 
            room starting at the provided startIndex
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
