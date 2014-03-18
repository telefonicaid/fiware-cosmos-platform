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

package es.tid.cosmos.servicemanager.ambari.rest.as

import com.ning.http.client
import net.liftweb.json
import json.JsonAST.JValue

/**
 * Helper object to translate HTTP responses to JSON.
 */
object Json extends (client.Response => JValue) {
  def apply(r: client.Response) = json.parse(r.getResponseBody match {
    case "" => "{}"
    case body => body
  })
}
