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
