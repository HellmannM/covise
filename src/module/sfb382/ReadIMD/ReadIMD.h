/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/**************************************************************************\ 
 **                                                           (C)2002 RUS  **
 **                                                                        **
 ** Description: Read IMD checkpoint files from ITAP.                      **
 **                                                                        **
 **                                                                        **
 ** Author:                                                                **
 **                                                                        **
 **                     Juergen Schulze-Doebold                            **
 **     High Performance Computing Center University of Stuttgart          **
 **                         Allmandring 30                                 **
 **                         70550 Stuttgart                                **
 **                                                                        **
 ** Cration Date: 03.09.2002                                               **
\**************************************************************************/

#ifndef _READ_IMD_H_
#define _READ_IMD_H_

#include <api/coSimpleModule.h>
using namespace covise;

/** Routines for checkpoint file handling.
 */
class coCheckpointFile
{
public:
    vvArray<char *> paramNames; ///< names of atom parameters (x, y, z, vx, vy, vz, type, mass, ...)
    float boxSize[3][3]; ///< size of simulation box
    FILE *fp; ///< current file pointer
    coModule *module; ///< module in which this class is used

    coCheckpointFile(coModule *, FILE *);
    virtual ~coCheckpointFile();
    void clearParamNames();
    int getParamIndex(char *);
    void getSpeedIndices(int *, int *, int *);
    void getLocationIndices(int *, int *, int *);
    bool parseHeader();
};

/** Reader module for IMD checkpoint files. The file format was developed
    at the ITAP at the University of Stuttgart.
    This is an example file with three atoms:<PRE>
    #F A 1 1 1 3 3 1
    #C number type mass x y z vx vy vz Epot
    #X      3.0766609395000000e+02 0.0000000000000000e+00 0.0000000000000000e+00
    #Y      0.0000000000000000e+00 1.0442535916900000e+02 0.0000000000000000e+00
    #Z      0.0000000000000000e+00 0.0000000000000000e+00 1.4357751050999990e+01
    ## Generated by /hwwt3e/rus/ita/pof30/bin/cray-t3e/imd_mpi_nve_stress_ordpar_efilter on Thu Jun 27 23:56:46 2002
    #E
    18368 0     1.000000    12.311201    48.337746     1.031030    -0.032552     0.047432    -0.014428   -17.623187
18800 0     1.000000    12.310159    48.341527     3.080848    -0.024594     0.040695    -0.009033   -17.630691
15772 1     1.000000    10.766565    47.946747     2.054420    -0.009312    -0.063240    -0.027128   -21.059210
</PRE>
@author Juergen Schulze
*/
class coReadIMD : public coSimpleModule
{
private:
    // Ports:
    coOutputPort *poPoints;
    coOutputPort *poSpeed;
    coOutputPort *poScalar1;
    coOutputPort *poScalar2;

    // Parameters:
    coFileBrowserParam *pbrCheckpointFile; ///< name of first checkpoint file of a sequence
    coBooleanParam *pboPeriodic; ///< true = use periodic boundaries with specified center, false = no periodic boundaries
    coFloatVectorParam *pfvPeriodic; ///< bottom left front corner of bounding box for periodic boundary conditions
    coBooleanParam *pboConstrainSpeed; ///< true = atom display is constrained to a certain speed range, false = no constraints
    coFloatParam *pfsSpeedMin; ///< minimum absolute speed value to display
    coFloatParam *pfsSpeedMax; ///< maximum absolute speed value to display
    coChoiceParam *pchScalar1; ///< choice box for selection of output parameter 1
    coChoiceParam *pchScalar2; ///< choice box for selection of output parameter 2
    coFloatParam *pfsFactor1; ///< factor to multiply scalar parameter 1 with
    coFloatParam *pfsFactor2; ///< factor to multiply scalar parameter 2 with
    coFloatParam *pfsOffset1; ///< offset value to add to scalar parameter 1
    coFloatParam *pfsOffset2; ///< offset value to add to scalar parameter 2
    coBooleanParam *pboWarnings; ///< true = display warnings when reading a file

    // Attributes:
    vvArray<char *> paramNames; ///< names of atom parameters in header (atom type, Epot,...)

    // Methods:
    virtual int compute(const char *port);
    virtual void param(const char *, bool inMapLoading);
    void readParameters();
    void updateParameters(char **);
    float absVector(float, float, float);

public:
    coReadIMD(int argc, char **argv);
    bool displayWarnings();
};
#endif