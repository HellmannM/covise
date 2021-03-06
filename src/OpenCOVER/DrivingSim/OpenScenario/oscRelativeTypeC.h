/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */

#ifndef OSC_RELATIVE_TYPE_C_H
#define OSC_RELATIVE_TYPE_C_H

#include "oscExport.h"
#include "oscObjectBase.h"
#include "oscObjectVariable.h"

#include "oscVariables.h"


namespace OpenScenario {

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscRelativeTypeC: public oscObjectBase
{
public:
    oscRelativeTypeC()
    {
        OSC_ADD_MEMBER(refObject);
        OSC_ADD_MEMBER(delta);
        OSC_ADD_MEMBER(factor);
    };

    oscString refObject;
    oscDouble delta;
    oscDouble factor;
};

typedef oscObjectVariable<oscRelativeTypeC *> oscRelativeTypeCMember;

}

#endif /* OSC_RELATIVE_TYPE_C_H */
