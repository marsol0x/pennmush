/* ==================================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Marshel Helsper $
   $Notice: $
   ================================================================================== */

#include <math.h>

#include "attrib.h"
#include "dbdefs.h"
#include "game.h"
#include "log.h"
#include "mushtype.h"
#include "mymalloc.h"
#include "parse.h"
#include "strutil.h"

#include "space.h"
#include "space_vector.c"
#include "space_functions.c"

char *SpaceObjectAttributes[SpaceObjectAttribute_Count] = {
    "SPACE`TYPE",
    "SPACE`CONSOLE",
    "SPACE`POS_X",
    "SPACE`POS_Y",
    "SPACE`POS_Z",
    "SPACE`HEAD_X",
    "SPACE`HEAD_Y",
    "SPACE`HEAD_Z",
    "SPACE`DEST_X",
    "SPACE`DEST_Y",
    "SPACE`DEST_Z",
    "SPACE`SPEED",
    "SPACE`PASSIVE_SENSOR_RANGE",
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
    SpaceSystem.SpaceWizard = NOTHING;
    sq_register_loop(1, SpaceUpdate, 0, 0);
}

static void SpaceInitAttributes(space_system *SpaceSystem, dbref Id, dbref Console)
{
    // NOTE(marshel): Set initial SPACE` attribute tree to have the proper
    // flags
    atr_add(Id, "SPACE", "Space Attributes", SpaceSystem->SpaceWizard, AF_WIZARD | AF_MDARK | AF_NOPROG);
    for (int Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
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

void SpaceUpdateObjectFromAttributes(space_object *SpaceObject)
{
    if (!SpaceObject)
    {
        return;
    }

    dbref Id = SpaceObject->Id;
    for (int Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
    {
        ATTR *Attribute = atr_get_noparent(Id, SpaceObjectAttributes[Index]);

        if (Attribute)
        {
            char *AttributeString = atr_value(Attribute);
            switch (Index)
            {
                case SpaceObjectAttribute_Type:
                {
                    SpaceObject->Type = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_Console:
                {
                    SpaceObject->Console = parse_dbref(AttributeString);
                } break;

                case SpaceObjectAttribute_PosX:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Position.X = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_PosY:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Position.Y = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_PosZ:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Position.Z = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_HeadX:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Heading.X = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_HeadY:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Heading.Y = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_HeadZ:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Heading.Z = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_DestX:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Destination.X = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_DestY:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Destination.Y = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_DestZ:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->Destination.Z = isnan(Value) ? 0.0f : Value;
                } break;

                case SpaceObjectAttribute_Speed:
                {
                    SpaceObject->Speed = atoi(AttributeString);
                } break;

                case SpaceObjectAttribute_PassiveSensorRange:
                {
                    float Value = atof(AttributeString);
                    SpaceObject->PassiveSensorRange = isnan(Value) ? 0.0f : Value;
                } break;
            }
        }
    }

    bool HasRadius = false;
    for (int Index = 0; Index < SpaceObjectDimension_Count; ++Index)
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

void SpaceSetAttributesFromObject(space_system *SpaceSystem, space_object *SpaceObject)
{
    dbref Id = SpaceObject->Id;
    char Buffer[BUFFER_LEN] = {0};

    for (int Index = 0; Index < SpaceObjectAttribute_Count; ++Index)
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
                sprintf(Buffer, "%f", SpaceObject->Position.X);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PosY:
            {
                sprintf(Buffer, "%f", SpaceObject->Position.Y);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PosZ:
            {
                sprintf(Buffer, "%f", SpaceObject->Position.Z);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadX:
            {
                sprintf(Buffer, "%f", SpaceObject->Heading.X);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadY:
            {
                sprintf(Buffer, "%f", SpaceObject->Heading.Y);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_HeadZ:
            {
                sprintf(Buffer, "%f", SpaceObject->Heading.Z);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_DestX:
            {
                sprintf(Buffer, "%f", SpaceObject->Destination.X);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_DestY:
            {
                sprintf(Buffer, "%f", SpaceObject->Destination.Y);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_DestZ:
            {
                sprintf(Buffer, "%f", SpaceObject->Destination.Z);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_Speed:
            {
                sprintf(Buffer, "%d", SpaceObject->Speed);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;

            case SpaceObjectAttribute_PassiveSensorRange:
            {
                sprintf(Buffer, "%f", SpaceObject->PassiveSensorRange);
                atr_add(Id, SpaceObjectAttributes[Index], Buffer, SpaceSystem->SpaceWizard, 0);
            } break;
        }
    }
}

static void SpaceTickSpaceShip(space_system *SpaceSystem, space_object *Object, int TimeStep)
{
    // NOTE(marshel): Update position based on speed and heading
    // TODO(marshel): The dumb way is to instantly update heading, which we are
    // doing, but we should have some sort of turning speed and update the
    // heading based on that.
    // TODO(marshel): Acceleration?

    // NOTE(marshel): Stop is we're close to our destination
    // TODO(marshel): Players might want to simple point at an object in space
    // to set the heading so we will want to have a flag that determines
    // whether or not to stop when the destination is reached
    // TODO(marshel): Allow for non-autopilot travel
    float DistanceToDestination = VectorDistance(Object->Position, Object->Destination);
    if (DistanceToDestination < Object->Speed)
    {
        Object->Speed /= 2;
    }

    if (DistanceToDestination < 2.0f)
    {
        Object->Speed = 0;
    }

    // NOTE(marshel): Movement
    v3f Heading = VectorSubtract(Object->Destination, Object->Position);
    v3f NormalizedHeading = VectorNormal(Heading);
    v3f Velocity = VectorMultiplyScalar(NormalizedHeading, Object->Speed);
    Velocity = VectorMultiplyScalar(Velocity, TimeStep);
    v3f NewPosition = VectorAdd(Object->Position, Velocity);

    // NOTE(marshel): Save changes
    Object->Heading = NormalizedHeading;
    Object->Position = NewPosition;
    SpaceSetAttributesFromObject(SpaceSystem, Object);
}

bool SpaceUpdate(void *data)
{
    if (SpaceSystem.SpaceWizard == NOTHING)
    {
        SpaceSystem.SpaceWizard = SpaceFindSpaceWizard();
        return false;
    }

    SpaceSystem.RoomCount = 0;

    for (dbref Id = 0; Id < db_top; ++Id)
    {
        if (RealGoodObject(Id))
        {
            if (IS(Id, TYPE_THING, "SPACE"))
            {
                ATTR *SpaceAttributes = atr_get_noparent(Id, "SPACE");
                if (!SpaceAttributes)
                {
                    SpaceInitAttributes(&SpaceSystem, Id, NOTHING);
                }
            }

            if (IS(Id, TYPE_ROOM, "SPACE"))
            {
                SpaceSystem.Rooms[SpaceSystem.RoomCount] = Id;
                ++SpaceSystem.RoomCount;
            }
        }
    }

    for (int RoomIndex = 0; RoomIndex < SpaceSystem.RoomCount; ++RoomIndex)
    {
        dbref Room = SpaceSystem.Rooms[RoomIndex];

        for (dbref ObjectId = Contents(Room);
             ObjectId != -1;
             ObjectId = Next(ObjectId))
        {
            if (SpaceIsSpaceObject(ObjectId))
            {
                space_object Object;
                memset(&Object, 0, sizeof(space_object));
                Object.Id = ObjectId;
                SpaceUpdateObjectFromAttributes(&Object);

                switch (Object.Type)
                {
                    case SpaceObjectType_Ship:
                    {
                        SpaceTickSpaceShip(&SpaceSystem, &Object, 1);
                    } break;

                    case SpaceObjectType_Planet:
                    {
                        // TODO(marshel): Planet orbit stuff goes here
                    } break;

                    default:
                    {
                        // NOTE(marshel): Do nothing if the object type isn't handled.
                        // TODO(marshel): Maybe log this in a way that informs connected Wizards?
                    } break;
                }
            }
        }
    }

    return true;
}
