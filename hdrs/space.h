#ifndef SPACE_H
/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include <math.h>

#include "flags.h"

#include "space_vector.h"

#define SPACE_DLIST_INIT(n) \
    (n)->Next = (n); \
    (n)->Prev = (n);

#define SPACE_DLIST_ADD(l, n) \
    (n)->Prev = (l)->Prev; \
    (n)->Next = (l); \
    (l)->Prev->Next = (n); \
    (l)->Prev = (n);

#define SPACE_DLIST_REMOVE(n) \
    (n)->Prev->Next = (n)->Next; \
    (n)->Next->Prev = (n)->Prev;

typedef enum
{
    SpaceObjectType_Ship,
    SpaceObjectType_Planet,
    SpaceObjectType_Star,

    SpaceObjectType_Count,
} space_object_type;

enum
{
    SpaceObjectAttribute_Type,
    SpaceObjectAttribute_Console,
    SpaceObjectAttribute_PosX,
    SpaceObjectAttribute_PosY,
    SpaceObjectAttribute_PosZ,
    SpaceObjectAttribute_HeadX,
    SpaceObjectAttribute_HeadY,
    SpaceObjectAttribute_HeadZ,
    SpaceObjectAttribute_DestX,
    SpaceObjectAttribute_DestY,
    SpaceObjectAttribute_DestZ,
    SpaceObjectAttribute_Speed,
    SpaceObjectAttribute_PassiveSensorRange,

    SpaceObjectAttribute_Count,
};

enum
{
    SpaceObjectDimension_Radius,
    SpaceObjectDimension_SizeHeight,
    SpaceObjectDimension_SizeWidth,
    SpaceObjectDimension_SizeLength,

    SpaceObjectDimension_Count,
};

typedef struct space_object_t
{
    struct space_object_t *Next, *Prev;

    dbref Id;
    dbref Console;
    space_object_type Type;

    v3f Destination;
    v3f Position;
    v3f Heading;
    int Speed; // TODO(marshel): Should this be a float too?
    float PassiveSensorRange;

    union
    {
        int Radius;
        v3i Dim;
    } Size;
} space_object;

#define SPACE_MAX_OBJECTS 64
typedef struct space_room_t
{
    dbref Id;

    int ObjectCount;
    space_object *Objects[SPACE_MAX_OBJECTS];
} space_room;

#define SPACE_MAX_ROOMS 64
typedef struct
{
    dbref SpaceWizard;

    int RoomCount;
    space_room *Rooms[SPACE_MAX_ROOMS];
} space_system;
space_system SpaceSystem;

void SpaceAddFlags(FLAGSPACE *flags);
void SpaceAddFunctions();
void SpaceStartup();
bool SpaceUpdate(void *data);

space_object * SpaceGetObjectFromRoomById(space_room *Room, dbref Id);

#define SPACE_H
#endif
