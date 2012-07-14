----------------------------------------------------
Quick tutorial on samson
----------------------------------------------------

Preparing your local machine for running samson
Install cmake, qt4-qmake, libqt4-core libqt4-dev, libreadline-dev, protobuf-c-compiler, libprotobuf-dev

* Kill everything
  --> make sure no samsonWorker, samsonController is working

* samsonProcessesSupervise
  --> Supervise samson-related application running

* samsonSpawner -reset -fg
  --> Start the platform in all nodes
  --> -reset: Clear previous setup of the cluster ( /opt/samson/config/samsonPlatformProcesses ) 
  --> -fg: Foreground 

* samsonSetup -controller samson05 -workers 3 -ips "samson05 samson06 samson07"
  --> Setup the platform

* delilah -controller samson05
  --> Client of the platform

* delilah -controller samson05 -monitorization
  --> Basic monitorization of the platform


Working directory:

/home/samson/sb/andreu/trunk --> make to recompile and distribute
