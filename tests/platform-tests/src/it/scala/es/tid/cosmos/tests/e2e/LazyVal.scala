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

package es.tid.cosmos.tests.e2e

import java.io.Closeable

/**
  * This class provides a means of deferring value construction similar to
  * lazy val, but by using this class you can pass the value as a function
  * parameter without having to trigger object creation.
  *
  * This class is implicitly convertible to T, so all methods in T can be
  * called directly on the LazyVal[T]. The first time any method of T is called
  * it will trigger the creation of the value.
  *
  * @param constructor A by-name parameter which creates the object
  * @tparam T The type of the value that needs deferred construction
  */
class LazyVal[T <: Closeable](constructor: =>T) extends Closeable {
  private var _value: Option[T] = None
  lazy val value = _value match {
    case None =>
      _value = Some(constructor)
      _value.get
    case Some(t) => t
  }

  override def close() {
    _value match {
      case None => ()
      case Some(t) => t.close()
    }
  }
}

object LazyVal {
  import scala.language.implicitConversions

  implicit def getValue[T <: Closeable](value: LazyVal[T]): T = value.value
}
