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

package es.tid.cosmos.api.controllers

import org.scalatest.matchers.{MatchResult, Matcher}
import play.api.mvc.{AsyncResult, Result}
import play.api.test.Helpers._
import scala.Some
import scala.concurrent.Await
import scala.concurrent.duration.Duration

object ResultMatchers {
  def failWith[E](clazz: Class[E]): Matcher[Result] =
    new Matcher[Result] {
      def apply(r: Result): MatchResult = r match {
        case AsyncResult(f) =>
          MatchResult(
            matches = Await.result(f.failed, Duration.Inf).getClass == clazz,
            failureMessage = s"result doesn't fail with $clazz",
            negatedFailureMessage = s"result fails with $clazz")
        case _ =>
          MatchResult(
            matches = false,
            failureMessage = s"result doesn't fail at all",
            negatedFailureMessage = s"result is failed")
      }
    }

  def redirectTo(path: String): Matcher[Result] =
    new Matcher[Result] {
      def apply(r: Result): MatchResult =
        if (status(r) != SEE_OTHER) {
          MatchResult(
            matches = false,
            failureMessage = s"${status(r)} status found when $SEE_OTHER were expected",
            negatedFailureMessage = s"${status(r)} status not found when $SEE_OTHER were expected"
          )
        } else {
          MatchResult(
            matches = header("Location", r) == Some(path),
            failureMessage = s"result does not redirect to $path",
            negatedFailureMessage = s"result redirects to $path"
          )
        }
    }
}
