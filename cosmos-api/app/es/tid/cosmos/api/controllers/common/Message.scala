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

package es.tid.cosmos.api.controllers.common

import play.api.libs.json.{JsString, Json, JsValue, Writes}

/**
 * Represents a simple text message to the client.
 * @param m Text of the message
 */
case class Message(m: String)

object Message {
  implicit object MessageWrites extends Writes[Message]{
    def writes(instance: Message): JsValue = Json.obj(
      "message" -> instance.m
    )
  }
}
