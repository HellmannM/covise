/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#ifndef _READ_CSV_H
#define _READ_CSV_H
// +++++++++++++++++++++++++++++++++++++++++
// MODULE ReadCSV
//
// This module Reads CSV formated ASCII files
//
#include <util/coviseCompat.h>
#include <api/coModule.h>
#include "reader/coReader.h"
#include "reader/ReaderControl.h"
using namespace covise;

enum
{
    TXT_BROWSER,
    MESHPORT3D,
    X_COORD,
    DPORT1_3D,
    DPORT2_3D,
    DPORT3_3D,
    DPORT4_3D,
    DPORT5_3D,
};
class ReadCSV : public coReader
{
public:
    typedef struct
    {
        float x, y, z;
    } Vect3;

    typedef struct
    {
        int assoc;
        int col;
        std::string name;
        coDistributedObject **dataObjs;
        std::string objectName;
        float *x_d;

    } VarInfo;

private:
    //  member functions
    virtual int compute(const char *port);
    virtual void param(const char *paraName, bool inMapLoading);

    // ports

    // ------------------------------------------------------------
    // -- parameters
    coChoiceParam *x_col, *y_col, *z_col;

    // utility functions
    int readHeader();
    int readASCIIData();

    // already opened file, always rewound after use
    FILE *d_dataFile;
    std::string fileName;

    void nextLine(FILE *fp);

    char buf[5000];
    int numLines;
    int numRows;

    std::vector<VarInfo> varInfos;

    int *v_l_tri;
    int *v_l_quad;
    float *xCoords;
    float *yCoords;
    float *zCoords;

public:
    ReadCSV(int argc, char *argv[]);
    virtual ~ReadCSV();
};
#endif
