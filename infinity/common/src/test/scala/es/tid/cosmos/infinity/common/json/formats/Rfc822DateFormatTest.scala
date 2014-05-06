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

package es.tid.cosmos.infinity.common.json.formats

import java.util.{Calendar, TimeZone}

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class Rfc822DateFormatTest extends FlatSpec with MustMatchers {

  "Date format" must "parse RFC 822 dates" in {
    val date = Rfc822DateFormat.parse("2014-04-08T12:31:45+0100")
    val calendar = Calendar.getInstance(TimeZone.getTimeZone("GMT+01:00"))
    calendar.setTime(date)
    calendar.get(Calendar.YEAR) must be (2014)
    calendar.get(Calendar.MONTH) must be (3)
    calendar.get(Calendar.DAY_OF_MONTH) must be (8)
    calendar.get(Calendar.HOUR_OF_DAY) must be (12)
  }
}
