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

package es.tid.cosmos.infinity.server.util

import java.io.Closeable

/** Utility for dealing with data resources such as releasing them after being used. */
object IoUtil {

  /** Perform a `block` of code ensuring that the given closeables will always be released
    * at the end. Useful for performing data operations such as reading input streams
    * without worrying about releasing it afterwards.
    *
    * @param closeables the closeables to be released after the block has been executed
    * @param block      the block to be executed that can safely use any of the closeables
    * @tparam T         the type of the block's result
    * @return           the block's result ensuring that all closeables have been closed.
    */
  def withAutoClose[T](closeables: Closeable*)(block: => T): T =
    try { block } finally { closeables foreach (_.close()) }

  /** Perform a `block` of code ensuring that the given closeables will be released <b>only</b>
    * if there is an exception. The exception will then be thrown again
    * after releasing the resources.
    *
    * @param closeables the closeables to be released after the block has been executed
    * @param block      the block to be executed that can safely use any of the closeables
    * @tparam T         the type of the block's result
    * @return           the block's result ensuring that all closeables have been closed.
    */
  def withAutoCloseOnFail[T](closeables: Closeable*)(block: => T): T =
    try { block } catch {
      case e: Throwable =>
        closeables foreach (_.close())
        throw e
    }
}
