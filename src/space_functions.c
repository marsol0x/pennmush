/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include "dbdefs.h"
#include "flags.h"
#include "function.h"
#include "parse.h"
#include "strutil.h"
#include "sort.h"

#include "space.h"

static bool SpaceIsSpaceObject(dbref thing)
{
    return has_flag_in_space_by_name("FLAG", thing, "SPACE", (TYPE_THING | TYPE_ROOM));
}

static void SpaceSetObjectFromAttributes(space_object *Object, dbref Id)
{
    memset(Object, 0, sizeof(space_object));
    Object->Id = Id;
    SpaceUpdateObjectFromAttributes(Object);
}

FUNCTION(space_fun_sensors)
{
    dbref SpaceShipId = parse_dbref(args[0]);
    dbref SpaceShipLocationId = Location(SpaceShipId);
    space_room *SpaceShipLocation = 0;
    space_object *SpaceShipObject = 0;

    if (!SpaceIsSpaceObject(SpaceShipId))
    {
        safe_format(buff, bp, "#-1 Object is not a space object.");
        return;
    }

    for (int RoomIndex = 0;
         RoomIndex < SpaceSystem.RoomCount;
         ++RoomIndex)
    {
        space_room *Room = SpaceSystem.Rooms[RoomIndex];
        if (Room && Room->Id == SpaceShipLocationId)
        {
            SpaceShipLocation = Room;
            break;
        }
    }

    // TODO(marshel): Consider creating a new space_object on the stack and
    // filling it in
    SpaceShipObject = SpaceGetObjectFromRoomById(SpaceShipLocation, SpaceShipId);
    for (int ObjectIndex = 0;
         ObjectIndex < SpaceShipLocation->ObjectCount;
         ++ObjectIndex)
    {
        space_object *Object = SpaceShipLocation->Objects[ObjectIndex];

        if (Object)
        {
            if (Object == SpaceShipObject) { continue; }

            float Distance = VectorDistance(SpaceShipObject->Position, Object->Position);
            if (Distance <= SpaceShipObject->PassiveSensorRange)
            {
                safe_format(buff, bp, "#%d ", Object->Id);
            }
        }
    }
}

FUNCTION(space_fun_head_toward_object)
{
    dbref SpaceShipId = parse_dbref(args[0]);
    dbref DestinationId = parse_dbref(args[1]);

    if (!SpaceIsSpaceObject(SpaceShipId))
    {
        safe_format(buff, bp, "#-1 Object is not a space object.");
        return;
    }

    if (!SpaceIsSpaceObject(DestinationId))
    {
        safe_format(buff, bp, "#-1 Destination object is not a space object.");
        return;
    }

    if (Location(SpaceShipId) != Location(DestinationId))
    {
        safe_format(buff, bp, "#-1 Space object and destination object are not in the same system.");
        return;
    }

    space_object SpaceShipObject;
    SpaceSetObjectFromAttributes(&SpaceShipObject, SpaceShipId);

    space_object DestinationObject;
    SpaceSetObjectFromAttributes(&DestinationObject, DestinationId);

    SpaceShipObject.Destination = DestinationObject.Position;
    SpaceSetAttributesFromObject(&SpaceSystem, &SpaceShipObject);
}

FUNCTION(space_fun_set_destination)
{
    dbref ObjectId = parse_dbref(args[0]);
    v3f Destination; 

    if (!SpaceIsSpaceObject(ObjectId))
    {
        safe_format(buff, bp, "#-1 Object is not a space object.");
        return;
    }

    float Value;
    Value = atof(args[1]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid X value, must be valid floating point number."); return; }
    Destination.X = Value;

    Value = atof(args[2]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid Y value, must be valid floating point number."); return; }
    Destination.Y = Value;

    Value = atof(args[3]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid Z value, must be valid floating point number."); return; }
    Destination.Z = Value;

    space_object SpaceObject;
    SpaceSetObjectFromAttributes(&SpaceObject, ObjectId);
    SpaceObject.Destination = Destination;
    SpaceSetAttributesFromObject(&SpaceSystem, &SpaceObject);
}

FUNCTION(space_fun_set_position)
{
    dbref ObjectId = parse_dbref(args[0]);
    v3f Position; 

    if (!SpaceIsSpaceObject(ObjectId))
    {
        safe_format(buff, bp, "#-1 Object is not a space object.");
        return;
    }

    float Value;
    Value = atof(args[1]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid X value, must be valid floating point number."); return; }
    Position.X = Value;

    Value = atof(args[2]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid Y value, must be valid floating point number."); return; }
    Position.Y = Value;

    Value = atof(args[3]);
    if (isnan(Value)) { safe_format(buff, bp, "#-1 Invalid Z value, must be valid floating point number."); return; }
    Position.Z = Value;

    space_object SpaceObject;
    SpaceSetObjectFromAttributes(&SpaceObject, ObjectId);
    SpaceObject.Position = Position;
    SpaceSetAttributesFromObject(&SpaceSystem, &SpaceObject);
}

FUNCTION(space_fun_set_speed)
{
    dbref ObjectId = parse_dbref(args[0]);

    if (!SpaceIsSpaceObject(ObjectId))
    {
        safe_format(buff, bp, "#-1 Object is not a space object.");
        return;
    }

    // TODO(marshel): This could wrap
    int Speed = atoi(args[1]);

    space_object SpaceObject;
    SpaceSetObjectFromAttributes(&SpaceObject, ObjectId);
    SpaceObject.Speed = Speed;
    SpaceSetAttributesFromObject(&SpaceSystem, &SpaceObject);
}

FUNCTION(space_fun_get_distance)
{
    dbref ObjectAId = parse_dbref(args[0]);
    dbref ObjectBId = parse_dbref(args[1]);

    if (!SpaceIsSpaceObject(ObjectAId))
    {
        safe_format(buff, bp, "#-1 First object is not a space object.");
        return;
    }

    if (!SpaceIsSpaceObject(ObjectBId))
    {
        safe_format(buff, bp, "#-1 Second object is not a space object.");
        return;
    }

    space_object ObjectA, ObjectB;
    SpaceSetObjectFromAttributes(&ObjectA, ObjectAId);
    SpaceSetObjectFromAttributes(&ObjectB, ObjectBId);

    float Distance = VectorDistance(ObjectA.Position, ObjectB.Position);
    safe_format(buff, bp, "%f", Distance);
}

void SpaceAddFunctions()
{
    // TODO(marshel): Document these in help files
    // TODO(marshel): These need better names
    function_add("SPACE_OBJECT_SENSORS",            space_fun_sensors,            1, 1, FN_WIZARD);
    function_add("SPACE_OBJECT_HEAD_TOWARD_OBJECT", space_fun_head_toward_object, 2, 2, FN_WIZARD);
    function_add("SPACE_OBJECT_SET_DESTINATION",    space_fun_set_destination,    4, 4, FN_WIZARD);
    function_add("SPACE_OBJECT_SET_POSITION",       space_fun_set_position,       4, 4, FN_WIZARD);
    function_add("SPACE_OBJECT_SET_SPEED",          space_fun_set_speed,          2, 2, FN_WIZARD);
    function_add("SPACE_OBJECT_GET_DISTANCE",       space_fun_get_distance,       2, 2, FN_WIZARD);
}
