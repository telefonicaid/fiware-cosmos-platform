ADD_SUBDIRECTORY(samsonSpawner)
ADD_SUBDIRECTORY(samsonSetup)
ADD_SUBDIRECTORY(samsonWorker)
ADD_SUBDIRECTORY(samsonController)
ADD_SUBDIRECTORY(delilah)
ADD_SUBDIRECTORY(samsonLocal)
ADD_SUBDIRECTORY(samsonModuleParser)
ADD_SUBDIRECTORY(samsonCat)
#ADD_SUBDIRECTORY(samsonTest)
ADD_SUBDIRECTORY(samsonStarter)
ADD_SUBDIRECTORY(samsonData)
ADD_SUBDIRECTORY(samsonKiller)

ADD_SUBDIRECTORY(samsonPush)
ADD_SUBDIRECTORY(samsonPop)
if (HAVE_KDCHART)
  ADD_SUBDIRECTORY(delilahQt)
  ADD_SUBDIRECTORY(samsonPopQT)
endif (HAVE_KDCHART)

if (HAVE_QT)
  ADD_SUBDIRECTORY(samsonTopicMonitor)
endif (HAVE_QT)

# Sandbox...
ADD_SUBDIRECTORY(samsonTest)

# Auxiliar apps
ADD_SUBDIRECTORY(aux)

#On hold
#ADD_SUBDIRECTORY(samsonSupervisor)

