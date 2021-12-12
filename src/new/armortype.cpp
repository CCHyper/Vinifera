/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          ARMORTYPE.CPP
 *
 *  @authors       CCHyper
 *
 *  @brief         Map armor type class.
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#include "armortype.h"
#include "vinifera_globals.h"
#include "ccini.h"
#include "debughandler.h"
#include "asserthandler.h"


/**
 *  Default class constructor.
 * 
 *  #NOTE: This constructor will not add the object to the vector.
 * 
 *  @author: CCHyper
 */
ArmorTypeClass::ArmorTypeClass() :
    Name("<none>")
{
}


/**
 *  Basic constructor for armor objects.
 * 
 *  @author: CCHyper
 */
ArmorTypeClass::ArmorTypeClass(const char *name) :
    Name("<none>")
{
    if (name[0] != '\0' && std::strlen(name)) {
        std::strncpy(Name, name, sizeof(Name));
        Name[sizeof(Name)-1] = '\0';
    }

    ArmorTypes.Add(this);
}


/**
 *  Class deconstructor.
 * 
 *  @author: CCHyper
 */
ArmorTypeClass::~ArmorTypeClass()
{
    ArmorTypes.Delete(this);
}


/**
 *  Reads armor object data from an INI file.
 * 
 *  @author: CCHyper
 */
bool ArmorTypeClass::Read_INI(CCINIClass &ini)
{
    if (!ini.Is_Present(Name)) {
        return false;
    }

    return true;
}


/**
 *  Performs one time initialization of the armor type class.
 * 
 *  @warning: Do not change this function, otherwise it will break support
 *            with the original game!
 * 
 *  @author: CCHyper
 */
bool ArmorTypeClass::One_Time()
{
    ArmorTypeClass *armor = nullptr;

    /**
     *  Create the default armor types.
     */
    armor = new ArmorTypeClass("none");
    ASSERT(armor != nullptr);

    armor = new ArmorTypeClass("wood");
    ASSERT(armor != nullptr);

    armor = new ArmorTypeClass("light");
    ASSERT(armor != nullptr);

    armor = new ArmorTypeClass("heavy");
    ASSERT(armor != nullptr);

    armor = new ArmorTypeClass("concrete");
    ASSERT(armor != nullptr);

    return true;
}


/**
 *  Fetches a reference to the armor specified.
 * 
 *  @author: CCHyper
 */
const ArmorTypeClass &ArmorTypeClass::As_Reference(ArmorType type)
{
    static const ArmorTypeClass _x;

    //ASSERT(type != ARMOR_NONE && type < Armors.Count());

    if (type == ARMOR_NONE || type >= ArmorTypes.Count()) {
        return _x;
    }

    return *ArmorTypes[type];
}


/**
 *  Converts a armor number into a armor object pointer.
 * 
 *  @author: CCHyper
 */
const ArmorTypeClass *ArmorTypeClass::As_Pointer(ArmorType type)
{
    //ASSERT(type != ARMOR_NONE && type < ArmorTypes.Count());
    return type != ARMOR_NONE && type < ArmorTypes.Count() ? ArmorTypes[type] : nullptr;
}


/**
 *  Fetches a reference to the armor specified.
 * 
 *  @author: CCHyper
 */
const ArmorTypeClass &ArmorTypeClass::As_Reference(const char *name)
{
    return As_Reference(From_Name(name));
}


/**
 *  Converts a armor name into a armor object pointer.
 * 
 *  @author: CCHyper
 */
const ArmorTypeClass *ArmorTypeClass::As_Pointer(const char *name)
{
    return As_Pointer(From_Name(name));
}


/**
 *  Retrieves the ArmorType for given name.
 * 
 *  @author: CCHyper
 */
ArmorType ArmorTypeClass::From_Name(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return ARMOR_NONE;
    }

    if (name != nullptr) {
        for (ArmorType index = ARMOR_FIRST; index < ArmorTypes.Count(); ++index) {
            if (!strcasecmp(As_Reference(index).Name, name)) {
                return index;
            }
        }
    }

    return ARMOR_NONE;
}


/**
 *  Returns name for given armor type.
 * 
 *  @author: CCHyper
 */
const char *ArmorTypeClass::Name_From(ArmorType type)
{
    return (type != ARMOR_NONE && type < ArmorTypes.Count() ? As_Reference(type).Name : "<none>");
}


/**
 *  Find or create a armor of the type specified.
 * 
 *  @author: CCHyper
 */
const ArmorTypeClass *ArmorTypeClass::Find_Or_Make(const char *name)
{
    ASSERT(name != nullptr);

    if (!strcasecmp(name, "<none>") || !strcasecmp(name, "none")) {
        return nullptr;
    }

    for (ArmorType index = ARMOR_FIRST; index < ArmorTypes.Count(); ++index) {
        if (!strcasecmp(ArmorTypes[index]->Name, name)) {
            return ArmorTypes[index];
        }
    }

    ArmorTypeClass *ptr = new ArmorTypeClass(name);
    ASSERT(ptr != nullptr);
    return ptr;
}
