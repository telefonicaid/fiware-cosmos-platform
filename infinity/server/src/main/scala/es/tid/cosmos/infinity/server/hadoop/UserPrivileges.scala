/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.hadoop

import java.security.PrivilegedAction

import org.apache.hadoop.security.UserGroupInformation

import es.tid.cosmos.infinity.common.permissions.{PermissionClass, PermissionsMask, UserProfile}
import es.tid.cosmos.infinity.server.groups.ArtificialUsersGroupMapping

/** Mix-in trait to allow executing code using a user's privileges. */
trait UserPrivileges {

  /** Runs the body with the permissions of user.
    *
    * This method affects the current thread's AccessControlContext, so the user permissions
    * are only applied to the current thread. If the body is spawning new threads or running
    * stuff in a different computation context, the user permissions will not apply and the default
    * (superuser) permissions will be used. For more information, check the javadoc for
    * javax.security.auth.Subject
    *
    * @param user The user to impersonate
    * @param body The actions to run
    * @return     Whatever body returns
    */
  def as[A](user: UserProfile)(body: => A): A = {
    val ugi = UserGroupInformation.createRemoteUser(
      if (!user.sharedCluster) user.username
      else user.groups.head)
    ugi.doAs(new PrivilegedAction[A] {
      override def run(): A = body
    })
  }
}
