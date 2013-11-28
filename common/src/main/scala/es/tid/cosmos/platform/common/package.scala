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

package es.tid.cosmos.platform

import scalaz.ValidationNel
import scalaz.syntax.validation._

/** Common type aliases. */
package object common {

  /** Type alias for executable validations with error messages.
    * This is useful for separating validation creation and execution where the execution
    * can be deferred to a different context down the line.
    *
    * It makes use of Scalaz `ValidationNel`
    */
  type ExecutableValidation = () => ValidationNel[String, Any]

  /** ''Null'' object that does a pass-through without any validation */
  val PassThrough: ExecutableValidation = () => ().successNel[String]
}
