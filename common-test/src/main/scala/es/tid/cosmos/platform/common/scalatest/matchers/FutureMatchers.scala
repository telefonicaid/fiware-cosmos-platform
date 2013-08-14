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
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.Duration
import scala.util.Failure

import org.scalatest.matchers.{Matcher, MatchResult}
import scala.reflect.Manifest

trait FutureMatchers {

  def runUnder[T](timeout: Duration): Matcher[Future[T]] = new Matcher[Future[T]] {
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

  def eventually[T](matcher: Matcher[T]): Matcher[Future[T]] = new Matcher[Future[T]] {
    def apply(left: Future[T]): MatchResult = matcher.apply(Await.result(left, Duration.Inf))
  }

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
}
