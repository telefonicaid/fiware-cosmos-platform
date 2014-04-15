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

package es.tid.cosmos.api.email

import com.typesafe.plugin._
import play.api.Play.current

/** Emailer using the Play Mailer Plugin.
  *
  * @param sender the sender's email address.
  */
class PlayEmailer(sender: String) extends Emailer {

  override def email(mail: Email): Unit = {
    val mailApi = use[MailerPlugin].email
    mailApi
      .setFrom(sender)
      .setRecipient(mail.to)
      .setSubject(mail.subject)
      .send(mail.message)
  }
}
