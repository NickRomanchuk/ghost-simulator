#include "defs.h"

/*  Function: void initHouse(HouseType* house)
    Purpose: Initializes a house structure found at the pointer house, initializes
        hunter, rooms, and evidence lists
        in/out:   house - pointer to house structure to initialize
    return:   N/A
*/
void initHouse(HouseType* house) {
    initHunterArray(&(house->hunters));     // Initialize hunter array
    initRoomList(&(house->rooms));          // Initialize room list
    initEvidenceList(&(house->evidence));   // Initialize evidence list
}

/*
    Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
        out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}

/*  Function: void cleanup(HouseType* house)
    Purpose: Deallocated all memory in the heap related to the provided house types
            at the pointer 'house'. This includes freeing the list evidence and connected rooms
            in each room of the house. Then freeing the hunter list, shared evidence list and all
            the room data
        in/out:   house - pointer to house structure to deallocate memory from
    return:   N/A
*/
void cleanUp(HouseType* house) {
    // Set current node to be the start of the room list
    RoomNode* current = house->rooms.head;

    // Loop through each room and free the associated memory for the room and evidence linked lists
    while (current != NULL) {
        cleanRoomList(&(current->data->connectedRooms));
        cleanEvidenceList(&(current->data->evidence));
        current = current->next;
    }

    // Free the rooms in the house
    cleanRoomData(&(house->rooms));
    cleanRoomList(&(house->rooms));

    // Free hunters in the house
    cleanHunters(house->hunters);

    // Free evidence in the house
    cleanEvidenceList(&(house->evidence));

    // Free the ghost in the house
    free(house->ghost);
}

