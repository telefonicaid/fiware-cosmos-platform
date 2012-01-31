ADD_SUBDIRECTORY(samsonLocal)

ADD_SUBDIRECTORY(samsonSetup)
ADD_SUBDIRECTORY(samsonWorker)
ADD_SUBDIRECTORY(delilah)
ADD_SUBDIRECTORY(samsonModuleParser)

ADD_SUBDIRECTORY(samsonCat)
#ADD_SUBDIRECTORY(samsonData)

#ADD_SUBDIRECTORY(samsonTest)
#ADD_SUBDIRECTORY(samsonDiskTest)

ADD_SUBDIRECTORY(passiveLocationPush)
ADD_SUBDIRECTORY(samsonPush)
ADD_SUBDIRECTORY(samsonPop)

if (HAVE_GTEST)
   ADD_SUBDIRECTORY(unitTest)
endif (HAVE_GTEST)

if (HAVE_QT)
    if (HAVE_KDCHART)
        #ADD_SUBDIRECTORY(delilahQt)
        ADD_SUBDIRECTORY(samsonLevelMonitor)
    endif (HAVE_KDCHART)
    ADD_SUBDIRECTORY(samsonTopicMonitor)
endif (HAVE_QT)

# Extra apps
ADD_SUBDIRECTORY(extra)

#On hold
#ADD_SUBDIRECTORY(samsonSupervisor)

