//
// author: dmicol
//

#include "es_tid_bdp_mobility_voronoi_VoronoiJNI.h"

#include <iostream>

#include <jni.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "VoronoiDiagramGenerator.h"

JNIEXPORT jboolean JNICALL Java_es_tid_bdp_mobility_voronoi_VoronoiJNI_generateVoronoi(
    JNIEnv* env, jobject obj, jdoubleArray xValues, jdoubleArray yValues,
    jint numPoints, jdouble minX, jdouble maxX, jdouble minY, jdouble maxY,
    jdouble minDist) {
  jdouble* xValuesPtr = env->GetDoubleArrayElements(xValues, 0);
  jdouble* yValuesPtr = env->GetDoubleArrayElements(yValues, 0);

  VoronoiDiagramGenerator generator;
  bool retVal = generator.generateVoronoi(xValuesPtr, yValuesPtr, numPoints,
                                          minX, maxX, minY, maxY, minDist);

  env->ReleaseDoubleArrayElements(xValues, xValuesPtr, 0);
  env->ReleaseDoubleArrayElements(yValues, yValuesPtr, 0);

  return retVal;
}
