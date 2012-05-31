#
# Command line tools that do not use QT or KDChart
#
ADD_SUBDIRECTORY(samsonLocal)
ADD_SUBDIRECTORY(samsonSetup)
ADD_SUBDIRECTORY(samsonWorker)
ADD_SUBDIRECTORY(delilah)
ADD_SUBDIRECTORY(samsonModuleParser)

ADD_SUBDIRECTORY(samsonCat)
ADD_SUBDIRECTORY(samsonModuleCat)

ADD_SUBDIRECTORY(passiveLocationPush)
ADD_SUBDIRECTORY(samsonPush)
ADD_SUBDIRECTORY(samsonPop)
ADD_SUBDIRECTORY(samsonConnector)
ADD_SUBDIRECTORY(samsonPushLogs)
ADD_SUBDIRECTORY(samsonMemCheck)

ADD_SUBDIRECTORY(logServer)
ADD_SUBDIRECTORY(ngsi)

#ADD_SUBDIRECTORY(samsonTest) #Sandbox for debuggin and testing

if ( IGNORE_UNI_TEST )
   MESSAGE( "Ignoring unit test..." )
ELSE ( IGNORE_UNI_TEST )
   if (HAVE_GTEST)
      ADD_SUBDIRECTORY(unitTest)
   else	(HAVE_GTEST)
   		MESSAGE( "Skipping unitTest since library gtest is not found" )
   endif (HAVE_GTEST)
ENDIF ( IGNORE_UNI_TEST )
#
# Any application that makes use of QT or KDChart should be in the correct section below
#

if (HAVE_QT)

    if (HAVE_KDCHART)

        #ADD_SUBDIRECTORY(delilah_graph)
        #ADD_SUBDIRECTORY(samsonLevelMonitor)

    endif (HAVE_KDCHART) 		    
       
	#ADD_SUBDIRECTORY(samsonTopicMonitor)

endif (HAVE_QT)

# Extra apps
ADD_SUBDIRECTORY(extra)

