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

void SpaceAddFunctions()
{
    function_add("SPACE_SHIP_SENSORS", space_fun_sensors, 1, 1, FN_WIZARD);
}
