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

package es.tid.cosmos.platform.common.scalatest.matchers

import java.util.concurrent.TimeoutException
import scala.util.Failure
import scala.reflect.Manifest
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.{FiniteDuration, Duration}

import org.scalatest.matchers.{Matcher, MatchResult}

/** Matchers for asserting on futures. */
trait FutureMatchers {

  /** Create a matcher that fails if the future takes more than a given duration to complete.
    * Note that this will match independently of the future succeeding or not.
    *
    * @param timeout Finite timeout
    * @return        A matcher
    */
  def runUnder[T](timeout: FiniteDuration): Matcher[Future[T]] = new Matcher[Future[T]] {
    def apply(left: Future[T]) = MatchResult(
      matches = try {
          Await.ready(left, timeout)
          true
        } catch {
          case ex: TimeoutException => false
        },
      failureMessage = s"future didn't complete in $timeout",
      negatedFailureMessage= s"future completed in $timeout"
    )
  }

  /** Create a matcher that matches on futures that produce a given value.
    * Note that non-terminating futures will block this matcher without failing. It is advisable
    * to combine this matcher with {{{runUnder}}}.
    *
    * @param matcher  Matcher of a plain value of type T
    * @tparam T       Type of the result the future produces
    * @return         A matcher
    */
  def eventually[T](matcher: Matcher[T]): Matcher[Future[T]] = new Matcher[Future[T]] {
    def apply(left: Future[T]): MatchResult = matcher.apply(Await.result(left, Duration.Inf))
  }

  /** Matcher of futures that succeed.
    * This will match independently of the produced value or the time taken to produce it.
    */
  val eventuallySucceed: Matcher[Future[_]] = new Matcher[Future[_]] {
    def apply(left: Future[_]): MatchResult = MatchResult(
      matches = {
        Await.ready(left, Duration.Inf)
        left.value.get.isSuccess
      },
      failureMessage = s"future didn't succeeded: ${left.value.get}",
      negatedFailureMessage = "future succeeded"
    )
  }

  /** Matcher of futures that fail with a given exception type.
    *
    * @param manifest  Compiler hint for finding the runtime type of the exception
    * @tparam E        Expected exception type
    * @return          A matcher
    */
  def eventuallyFailWith[E](implicit manifest: Manifest[E]): Matcher[Future[Any]] =
    new Matcher[Future[Any]] {
      private val clazz = manifest.runtimeClass.asInstanceOf[Class[E]]

      def apply(left: Future[Any]) = {
        val value = Await.ready(left, Duration.Inf).value
        MatchResult(
          matches = value match {
            case Some(Failure(ex)) => ex.getClass.equals(clazz)
            case _ => false
          },
          failureMessage = s"future didn't fail with exception of $clazz, final value: $value",
          negatedFailureMessage = s"future failed with exception of $clazz"
        )
      }
    }

  /** Create a matcher for futures that fail.
    *
    * @param exceptionMatcher  Matcher for the expected exception.
    * @return                  A matcher
    */
  def eventuallyFailWith(exceptionMatcher: Matcher[Throwable]): Matcher[Future[Any]] =
    new Matcher[Future[Any]] {
      def apply(left: Future[Any]) = {
        val value = Await.ready(left, Duration.Inf).value
        value match {
          case Some(Failure(ex)) => exceptionMatcher.apply(ex)
          case _ => MatchResult(
            matches = false,
            failureMessage = "future didn't fail",
            negatedFailureMessage = "future failed"
          )
        }
      }
    }

  /** Create a matcher for futures that will fail with a given exception message.
    *
    * @param expectedSubstring  String that should be present in the failure exception
    * @return                   A matcher
    */
  def eventuallyFailWith(expectedSubstring: String): Matcher[Future[Any]] =
    eventuallyFailWith(new Matcher[Throwable]{
      def apply(ex: Throwable): MatchResult = MatchResult(
        ex.getMessage.contains(expectedSubstring),
        s"exception message '${ex.getMessage}' did not include '$expectedSubstring'",
        s"exception message '${ex.getMessage}' include '$expectedSubstring'"
      )
    })
}
