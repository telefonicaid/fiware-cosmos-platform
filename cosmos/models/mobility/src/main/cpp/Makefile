CXXFLAGS=-shared -fpic
LDDFLAGS=-I/usr/java/default/include/linux \
	 -I/usr/lib/jvm/java-6-sun/include \
	 -I/usr/lib/jvm/java-6-sun/include/linux \
	 -I/System/Library/Frameworks/JavaVM.framework/Headers

all: libVoronoi.so

es/tid/bdp/mobility/voronoi/VoronoiJNI.class: ../java/es/tid/bdp/mobility/voronoi/VoronoiJNI.java
	javac -d . ../java/es/tid/bdp/mobility/voronoi/VoronoiJNI.java

es_tid_bdp_mobility_voronoi_VoronoiJNI.h: es/tid/bdp/mobility/voronoi/VoronoiJNI.class
	javah -jni es.tid.cosmos.mobility.voronoi.VoronoiJNI

VoronoiDiagramGenerator.so: VoronoiDiagramGenerator.h VoronoiDiagramGenerator.cpp
	g++ $(CXXFLAGS) -o VoronoiDiagramGenerator.so VoronoiDiagramGenerator.cpp

libVoronoi.so: es_tid_bdp_mobility_voronoi_VoronoiJNI.h es_tid_bdp_mobility_voronoi_VoronoiJNI.cpp VoronoiDiagramGenerator.so
	g++ $(CXXFLAGS) $(LDDFLAGS) -o libVoronoi.so es_tid_bdp_mobility_voronoi_VoronoiJNI.cpp VoronoiDiagramGenerator.so

clean:
	rm -rf es es_tid_bdp_mobility_voronoi_VoronoiJNI.h VoronoiDiagramGenerator.so libVoronoi.so

.PHONY: clean all
