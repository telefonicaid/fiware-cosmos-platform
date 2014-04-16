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
