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

import scala.Some
import scala.concurrent.{Future, Await}
import scala.concurrent.duration.Duration

import org.scalatest.matchers.{MatchResult, Matcher}
import play.api.mvc.SimpleResult
import play.api.test.Helpers._

object ResultMatchers {
  def failWith[E](clazz: Class[E]): Matcher[Future[SimpleResult]] =
    new Matcher[Future[SimpleResult]] {
      def apply(r_> : Future[SimpleResult]): MatchResult = MatchResult(
        matches = Await.result(r_>.failed, Duration.Inf).getClass == clazz,
        failureMessage = s"result doesn't fail with $clazz",
        negatedFailureMessage = s"result fails with $clazz")
    }

  def redirectTo(path: String): Matcher[Future[SimpleResult]] =
    new Matcher[Future[SimpleResult]] {
      def apply(r_> : Future[SimpleResult]): MatchResult =
        if (status(r_>) != SEE_OTHER) {
          MatchResult(
            matches = false,
            failureMessage = s"${status(r_>)} status found when $SEE_OTHER were expected",
            negatedFailureMessage = s"${status(r_>)} status not found when $SEE_OTHER were expected"
          )
        } else {
          MatchResult(
            matches = header("Location", r_>) == Some(path),
            failureMessage = s"result does not redirect to $path",
            negatedFailureMessage = s"result redirects to $path"
          )
        }
    }
}
