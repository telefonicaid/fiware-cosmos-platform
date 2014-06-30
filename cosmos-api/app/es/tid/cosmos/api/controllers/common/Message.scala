/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.common

import play.api.libs.json.{Json, JsValue, Writes}

/**
 * Represents a simple text message to the client.
 * @param message Text of the message
 */
case class Message(message: String)

object Message {
  implicit object MessageWrites extends Writes[Message]{
    def writes(instance: Message): JsValue = Json.obj(
      "message" -> instance.message
    )
  }
}
