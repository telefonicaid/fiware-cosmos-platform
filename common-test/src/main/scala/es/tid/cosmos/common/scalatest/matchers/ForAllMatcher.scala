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

package es.tid.cosmos.common.scalatest.matchers

import org.scalatest.matchers.{MatchResult, Matcher}

/**
 * A matcher that verifies whether all elements of a sequence matches a given predicate.
 */
class ForAllMatcher[T](matcherDesc: String)(pred: T => Boolean) extends Matcher[Seq[T]]
{
  def apply(left: Seq[T]) = MatchResult(
    left.forall(pred),
    s"expected all $matcherDesc in sequence $left",
    s"expected none $matcherDesc in sequence $left")
}
