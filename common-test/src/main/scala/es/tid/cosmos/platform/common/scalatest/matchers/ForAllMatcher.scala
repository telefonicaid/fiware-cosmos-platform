package es.tid.cosmos.platform.common.scalatest.matchers

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
