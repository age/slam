cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(NewOfNewSlam CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(sources coreslam.cpp

    FeaturesDetector/featuredetectorsurf.h
    FeaturesDetector/ifeaturedetector.h
    FeaturesDetector/private/featuredetectorsurf.cpp
    FeaturesDetector/private/ifeaturedetector.cpp

    FeaturesMatcher/featurematcherflann.h
    FeaturesMatcher/ifeaturematching.h
    FeaturesMatcher/private/featurematcherflann.cpp
    FeaturesMatcher/private/ifeaturematching.cpp

    MatchesFilter/imatchesfilter.h
    MatchesFilter/matchesfiltercvteam.h
    MatchesFilter/private/imatchesfilter.cpp
    MatchesFilter/private/matchesfiltercvteam.cpp

    TransformationCalculator/irtfinder.h
    TransformationCalculator/rtfindertrilateration.h
    TransformationCalculator/private/irtfinder.cpp
    TransformationCalculator/private/rtfindertrilateration.cpp

    BundleAdjustment/iboundleadjustment.h
    BundleAdjustment/opencvboundleadjustment.h
    BundleAdjustment/private/iboundleadjustment.cpp
    BundleAdjustment/private/opencvboundleadjustment.cpp

    AdditionFunctions/private/staticfunctions.cpp
    AdditionFunctions/staticfunctions.h

    triples.h
    matchpoints.h

    debugger_interface.cpp
)

add_library(slam SHARED ${sources})
