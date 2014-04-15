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

package es.tid.cosmos

import scala.concurrent.Future
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

  type NowFuture[N, F] = (N, Future[F])
}
