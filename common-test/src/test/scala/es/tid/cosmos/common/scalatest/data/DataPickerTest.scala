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

package es.tid.cosmos.common.scalatest.data

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class DataPickerTest extends FlatSpec with MustMatchers{

  "A picker" must "pick elements randomly" in new DataPicker {
    override val seed = 1
    val elements = 1 to 10000
    val left, right = pickAnyOne(elements)
    left must not be right
  }
}
