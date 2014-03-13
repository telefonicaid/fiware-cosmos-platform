/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.api.controllers.pages

import es.tid.cosmos.api.task.Finished

trait WithSampleTasks extends WithSampleSessions {
  val taskDao = testApp.taskDao
  val regUserAuthorizedTask = taskDao.registerTask()
  regUserAuthorizedTask.usersWithAccess = Seq(regUserInGroup.handle, "a", "b")
  val inaccesibleTask = taskDao.registerTask()
}
