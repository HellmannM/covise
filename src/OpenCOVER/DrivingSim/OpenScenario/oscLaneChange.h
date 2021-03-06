/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */

#ifndef OSC_LANE_CHANGE_H
#define OSC_LANE_CHANGE_H

#include "oscExport.h"
#include "oscObjectBase.h"
#include "oscObjectVariable.h"

#include "oscVariables.h"
#include "oscRelativeTypeA.h"
#include "oscAbsoluteTypeA.h"
#include "oscLaneDynamics.h"


namespace OpenScenario {

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscLaneChange: public oscObjectBase
{
public:
    oscLaneChange()
    {
        OSC_OBJECT_ADD_MEMBER_CHOICE(relativeLaneChange, "oscRelativeTypeA");
        OSC_OBJECT_ADD_MEMBER_CHOICE(absoluteLaneChange, "oscAbsoluteTypeA");
        OSC_OBJECT_ADD_MEMBER(dynamics, "oscLaneDynamics");
        OSC_ADD_MEMBER(targetLaneOffset);
    };

    oscRelativeTypeAMember relativeLaneChange;
    oscAbsoluteTypeAMember absoluteLaneChange;
    oscLaneDynamicsMember dynamics;
    oscDouble targetLaneOffset;
};

typedef oscObjectVariable<oscLaneChange *> oscLaneChangeMember;

}

#endif //OSC_LANE_CHANGE_H
