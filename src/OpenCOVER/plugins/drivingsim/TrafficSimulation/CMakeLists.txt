IF(NOT COVISE_BUILD_DRIVINGSIM)
  RETURN()
ENDIF()

COVISE_FIND_PACKAGE(OSGCAL)
COVISE_FIND_PACKAGE(CAL3D)

USING(XERCESC OSGCAL CAL3D)
using(VRML)

IF(NOT OSGCAL_FOUND)
  RETURN()
ENDIF()

covise_find_package(OpenSceneGraph 3.2.0 COMPONENTS osgTerrain)
COVISE_FIND_PACKAGE(osgTerrain)
COVISE_FIND_PACKAGE(GDAL)
COVISE_FIND_PACKAGE(V8)

if(NOT GDAL_FOUND)
    return()
endif()
if(NOT V8_FOUND)
    return()
endif()

SET(HEADERS
  TrafficSimulationPlugin.h
  Vehicle.h
  DonkeyCar.h
  VehicleGeometry.h
  CarGeometry.h
  VehicleManager.h
  VehicleFactory.h
  HumanVehicle.h
  HumanVehicleGeometry.h
  AgentVehicle.h
  VehicleUtils.h
  mtrand.h
  projects/radarcones.hpp
  PorscheFFZ.h
  UDPBroadcast.h
  PedestrianManager.h
  Pedestrian.h
  PedestrianGeometry.h
  PedestrianFactory.h
)
SET(SOURCES
  TrafficSimulationPlugin.cpp
  Vehicle.cpp
  DonkeyCar.cpp
  CarGeometry.cpp
  VehicleManager.cpp
  VehicleFactory.cpp
  HumanVehicle.cpp
  HumanVehicleGeometry.cpp
  AgentVehicle.cpp
  VehicleUtils.cpp
  mtrand.cpp
  projects/radarcones.cpp
  PorscheFFZ.cpp
  UDPBroadcast.cpp
  PedestrianManager.cpp
  Pedestrian.cpp
  PedestrianGeometry.cpp
  PedestrianFactory.cpp
)


INCLUDE_DIRECTORIES(
   ../../../DrivingSim/VehicleUtil
   ${OSGTERRAIN_INCLUDE_DIR}
   ${OSGCAL3D_INCLUDE_DIR}
   ${CAL3D_INCLUDE_DIR}
   ${GDAL_INCLUDE_DIR}
   ${V8_INCLUDE_DIR}
)

cover_add_plugin(TrafficSimulation  ${HEADERS} ${SOURCES})
qt_use_modules(TrafficSimulation Script ScriptTools)
target_link_libraries(TrafficSimulation
 ${OSGTERRAIN_LIBRARIES}
 ${OSGCAL_LIBRARIES}
 ${EXTRA_LIBS}
 RoadTerrain
 SteeringWheel #required for vrmlNodeVehicle (windows)
 coOpenVehicleUtil)

IF(MSVC80 OR MSVC90)
  ADD_COVISE_COMPILE_FLAGS(TrafficSimulation "/Zc:wchar_t-")
ENDIF()

COVISE_WNOERROR(TrafficSimulation)

#COVER_INSTALL_PLUGIN(TrafficSimulation)
