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
    dbref Id;
    dbref Console;
    space_object_type Type;

    v3f Destination;
    v3f Position;
    v3f Heading; // TODO(marshel): Replace this with something relative to the local star in a system
    int Speed; // TODO(marshel): Should this be a float too?
    int MaxSpeed;
    float PassiveSensorRange;

    union
    {
        int Radius;
        v3i Dim;
    } Size;
} space_object;

#define SPACE_MAX_ROOMS 64
typedef struct
{
    dbref SpaceWizard;

    int RoomCount;
    dbref Rooms[SPACE_MAX_ROOMS]; // TODO(marshel): Replace this with a dynamic arary of dbrefs
} space_system;
space_system SpaceSystem;

void SpaceAddFlags(FLAGSPACE *flags);
void SpaceAddFunctions();
void SpaceStartup();
bool SpaceUpdate(void *data);

void SpaceUpdateObjectFromAttributes(space_object *SpaceObject);
void SpaceSetAttributesFromObject(space_system *SpaceSystem, space_object *SpaceObject);

#define SPACE_H
#endif
