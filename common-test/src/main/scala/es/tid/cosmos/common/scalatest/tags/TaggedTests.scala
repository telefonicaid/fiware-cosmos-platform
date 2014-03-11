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

package es.tid.cosmos.common.scalatest.tags

import org.scalatest.{Tag, Suite}

/** Mix-in for tagging all tests with a given tag. */
trait TaggedTests extends Suite {

  /** Tag to be applied to all tests. */
  def testsTag: Tag

  override def tags: Map[String, Set[String]] = {
    val originalTags = super.tags
    (for {
      test <- testNames
    } yield (test, originalTags.getOrElse(test, Set()) + testsTag.name)).toMap
  }
}
