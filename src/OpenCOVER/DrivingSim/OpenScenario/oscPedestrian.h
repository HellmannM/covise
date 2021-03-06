/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */

#ifndef OSC_PEDESTRIAN_H
#define OSC_PEDESTRIAN_H

#include "oscExport.h"
#include "oscNameRefId.h"
#include "oscObjectVariable.h"

#include "oscVariables.h"
#include "oscFileHeader.h"
#include "oscBehavior.h"
#include "oscDimensionTypeA.h"
#include "oscFile.h"


namespace OpenScenario {

class OPENSCENARIOEXPORT pedestrianClassType: public oscEnumType
{
public:
    static pedestrianClassType *instance(); 
private:
    pedestrianClassType();
    static pedestrianClassType *inst;
};

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscPedestrian: public oscNameRefId
{
public:
    oscPedestrian()
    {
        OSC_OBJECT_ADD_MEMBER(fileHeader, "oscFileHeader");
        OSC_ADD_MEMBER(model);
        OSC_ADD_MEMBER(mass);
        OSC_OBJECT_ADD_MEMBER(behavior, "oscBehavior");
        OSC_OBJECT_ADD_MEMBER(dimension, "oscDimensionTypeA");
        OSC_OBJECT_ADD_MEMBER(geometry, "oscFile");
        OSC_ADD_MEMBER(category);

        category.enumType = pedestrianClassType::instance();
    };

    oscFileHeaderMember fileHeader;
    oscString model;
    oscDouble mass;
    oscBehaviorMember behavior;
    oscDimensionTypeAMember dimension;
    oscFileMember geometry;
    oscEnum category;

    enum pedestrianClass
    {
        pedestrian,
        wheelchair,
        animal,
    };
};

typedef oscObjectVariable<oscPedestrian *> oscPedestrianMember;

}

#endif //OSC_PEDESTRIAN_H
