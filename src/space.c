/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include "attrib.h"
#include "dbdefs.h"
#include "game.h"
#include "log.h"
#include "mushtype.h"
#include "mymalloc.h"
#include "parse.h"
#include "strutil.h"

#include "space.h"

char *SpaceObjectAttributes[SpaceObjectAttribute_Count] = {
    "SPACE`CONSOLE",
    "SPACE`POS_X",
    "SPACE`POS_Y",
    "SPACE`POS_Z",
    "SPACE`HEAD_X",
    "SPACE`HEAD_Y",
    "SPACE`HEAD_Z",
    "SPACE`SPEED",
};

char *SpaceObjectDimensions[SpaceObjectDimension_Count] = {
    "SPACE`RADIUS",
    "SPACE`SIZE_HEIGHT",
    "SPACE`SIZE_WIDTH",
    "SPACE`SIZE_LENGTH",
};

void SpaceAddFlags(FLAGSPACE *flags)
{
    if (strcmp(flags->name, "FLAG") == 0)
    {
        add_flag("SPACE", 0, (TYPE_THING | TYPE_ROOM), F_WIZARD, F_WIZARD);
    }
}

static dbref SpaceFindSpaceWizard()
{
    dbref Id;

    for (Id = 0; Id < db_top; ++Id)
    {
        if (IS(Id, TYPE_PLAYER, "WIZARD") && (strcmp(Name(Id), "SpaceWizard") == 0))
        {
            return Id;
        }
    }

    return NOTHING;
}

void SpaceStartup()
{
    SpaceSystem.SpaceWizard = SpaceFindSpaceWizard();

    sq_register_loop(1, SpaceUpdate, 0, 0);
}

static void SpaceInitAttributes(space_system *SpaceSystem, dbref Id, dbref Console)
{
    int Index;

    for (Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
    {
        atr_add(Id, SpaceObjectAttributes[Index], "0", SpaceSystem->SpaceWizard, 0);
    }

    char Buffer[BUFFER_LEN] = {0};
    char *BufferPointer = Buffer;
    int Result = safe_dbref(Console, Buffer, &BufferPointer);
    if (Result == 0)
    {
        atr_add(Id, SpaceObjectAttributes[SpaceObjectAttribute_Console], Buffer, SpaceSystem->SpaceWizard, 0);
    }
}

static void SpaceUpdateObjectFromAttributes(space_object *SpaceObject)
{
    if (!SpaceObject)
    {
        return;
    }

    int Index;
    dbref Id = SpaceObject->Id;

    for (Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
    {
        ATTR *Attribute = atr_get_noparent(Id, SpaceObjectAttributes[Index]);

        if (Attribute)
        {
            char *AttributeString = atr_value(Attribute);
            switch (Index)
            {
                case SpaceObjectAttribute_Console:
                {
                    SpaceObject->Console = parse_dbref(AttributeString);
                } break;

                case SpaceObjectAttribute_PosX:
                {
                    SpaceObject->Position.X = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_PosY:
                {
                    SpaceObject->Position.Y = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_PosZ:
                {
                    SpaceObject->Position.Z = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_HeadX:
                {
                    SpaceObject->Heading.X = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_HeadY:
                {
                    SpaceObject->Heading.Y = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_HeadZ:
                {
                    SpaceObject->Heading.Z = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_Speed:
                {
                    SpaceObject->Speed = atoi(AttributeString);
                } break;
            }
        }
    }

    bool HasRadius = false;
    for (Index = 0; Index < SpaceObjectDimension_Count; ++Index)
    {
        ATTR *Attribute = atr_get_noparent(Id, SpaceObjectAttributes[Index]);

        if (HasRadius)
        {
            break;
        }

        if (Attribute)
        {
            char *AttributeString = atr_value(Attribute);
            switch (Index)
            {
                case SpaceObjectDimension_Radius:
                {
                    SpaceObject->Size.Radius = atoi(AttributeString);
                    HasRadius = true;
                } break;

                case SpaceObjectDimension_SizeWidth:
                {
                    SpaceObject->Size.Dim.X = atoi(AttributeString);
                } break;

                case SpaceObjectDimension_SizeHeight:
                {
                    SpaceObject->Size.Dim.Y = atoi(AttributeString);
                } break;

                case SpaceObjectDimension_SizeLength:
                {
                    SpaceObject->Size.Dim.Z = atoi(AttributeString);
                } break;
            }
        }
    }

}

static void SpaceSetAttributesFromObject(space_system *SpaceSystem, space_object *SpaceObject)
{
    int Index;
    dbref Id = SpaceObject->Id;
    char Buffer[BUFFER_LEN] = {0};

    for (Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
    {
        switch (Index)
        {
            case SpaceObjectAttribute_Console:
            {
                char *Value = unparse_dbref(SpaceObject->Console);
                atr_add(Id, SpaceObjectAttributes[Index], Value, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PosX:
            {
                sprintf(Buffer, "%d", SpaceObject->Position.X);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PosY:
            {
                sprintf(Buffer, "%d", SpaceObject->Position.Y);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PosZ:
            {
                sprintf(Buffer, "%d", SpaceObject->Position.Z);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadX:
            {
                sprintf(Buffer, "%d", SpaceObject->Heading.X);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadY:
            {
                sprintf(Buffer, "%d", SpaceObject->Heading.Y);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadZ:
            {
                sprintf(Buffer, "%d", SpaceObject->Heading.Z);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;
        }
    }
}

static space_object * SpaceNewObject(dbref Id)
{
    space_object *Result = mush_malloc(sizeof(*Result), "Space Object");
    memset(Result, 0, sizeof(*Result));
    Result->Id = Id;
    Result->Console = NOTHING;

    return Result;
}

static space_object * SpaceGetObjectFromRoomById(space_room *Room, dbref Id)
{
    space_object *Result = 0;

    for (int Index = 0; Index < Room->ObjectCount; ++Index)
    {
        if (Room->Objects[Index]->Id == Id)
        {
            Result = Room->Objects[Index];
            break;
        }
    }

    return Result;
}

static bool SpaceAddRoom(space_system *SpaceSystem, space_room *Room)
{
    if (SpaceSystem->RoomCount + 1 > SPACE_MAX_ROOMS)
    {
        return false;
    }

    SpaceSystem->Rooms[SpaceSystem->RoomCount] = Room;
    ++SpaceSystem->RoomCount;

    return true;
}

static space_room * SpaceFindRoomById(space_system *SpaceSystem, dbref Id)
{
    space_room *Result = 0;

    for (int Index = 0; Index < SpaceSystem->RoomCount; ++Index)
    {
        if (SpaceSystem->Rooms[Index]->Id == Id)
        {
            Result = SpaceSystem->Rooms[Index];
            break;
        }
    }

    return Result;
}

static bool SpaceAddObjectToRoom(space_room *Room, space_object *SpaceObject)
{
    if (Room->ObjectCount + 1 > SPACE_MAX_OBJECTS)
    {
        return false;
    }

    for (int Index = 0; Index < Room->ObjectCount; ++Index)
    {
        if (Room->Objects[Index]->Id == SpaceObject->Id)
        {
            return false;
        }
    }

    Room->Objects[Room->ObjectCount] = SpaceObject;
    ++Room->ObjectCount;

    return true;
}

bool SpaceUpdate(void *data)
{
    dbref Id;

    if (SpaceSystem.SpaceWizard == NOTHING)
    {
        SpaceSystem.SpaceWizard = SpaceFindSpaceWizard();
        return false;
    }

    // TODO(marshel):
    //
    // This can be much better. We really should only be capturing the dbref of
    // our SPACE objects. We should init the SPACE attributes on all SPACE
    // objects that do not have them, but that's all we should be doing in
    // terms of attributes. For everything else we capture the rooms
    // themselves. Then on each pass we simple iterate over the SPACE objects
    // in the room, get the attributes, do the thing, save the changes.
    //
    // Update all objects before checking collision
    for (Id = 0; Id < db_top; ++Id)
    {
        if (RealGoodObject(Id) && IS(Id, TYPE_THING, "SPACE"))
        {
            ATTR *SpaceAttributes = atr_get_noparent(Id, "SPACE");
            if (!SpaceAttributes)
            {
                SpaceInitAttributes(&SpaceSystem, Id, NOTHING);
            }

            dbref RoomId = Location(Id);
            if (IS(RoomId, TYPE_ROOM, "SPACE"))
            {
                space_room *Room = SpaceFindRoomById(&SpaceSystem, RoomId);
                if (!Room)
                {
                    if (SpaceSystem.RoomCount + 1 < SPACE_MAX_ROOMS)
                    {
                        Room = mush_malloc(sizeof(*Room), "Space Room");
                        memset(Room, 0, sizeof(*Room));
                        Room->Id = RoomId;

                        if (!SpaceAddRoom(&SpaceSystem, Room))
                        {
                            // NOTE(marshel): If we fail to add the room we've hit
                            // our max, free the memory
                            mush_free(Room, "Space Room");
                        }
                    }
                }

                if (Room)
                {
                    space_object *SpaceObject = SpaceGetObjectFromRoomById(Room, Id);
                    if (!SpaceObject)
                    {
                        SpaceObject = SpaceNewObject(Id);
                        if (!SpaceAddObjectToRoom(Room, SpaceObject))
                        {
                            // NOTE(marshel): If we fail we've hit the max
                            mush_free(SpaceObject, "Space Object");
                        }
                    }
                }
            }
        }
    }

    for (int RoomIndex = 0; RoomIndex < SpaceSystem.RoomCount; ++RoomIndex)
    {
        space_room *Room = SpaceSystem.Rooms[RoomIndex];

        if (!RealGoodObject(Room->Id))
        {
            memcpy(SpaceSystem.Rooms + RoomIndex, SpaceSystem.Rooms + RoomIndex + 1, (SpaceSystem.RoomCount - RoomIndex - 1) * sizeof(*Room));
            mush_free(Room, "Space Room");
            --RoomIndex;
            --SpaceSystem.RoomCount;
            continue;
        }

        for (int ObjectIndex = 0; ObjectIndex < Room->ObjectCount; ++ObjectIndex)
        {
            space_object *Object = Room->Objects[ObjectIndex];
            if (!RealGoodObject(Object->Id))
            {
                // TODO(marshel): Handle removing deleted/garbage objects
                memcpy(Room->Objects + ObjectIndex, Room->Objects + ObjectIndex + 1, (Room->ObjectCount - ObjectIndex - 1) * sizeof(*Object));
                mush_free(Object, "Space Object");
                --ObjectIndex;
                --Room->ObjectCount;
                continue;
            }

            SpaceUpdateObjectFromAttributes(Object);
            Object->Position.X += Object->Speed;
            SpaceSetAttributesFromObject(&SpaceSystem, Object);
        }
    }

    return true;
}
