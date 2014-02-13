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

package es.tid.cosmos.api.test.matchers

import java.net.URL

import org.scalatest.matchers.{ClassicMatchers, Matcher, MatchResult, BeMatcher}
import play.api.libs.json.{JsString, JsUndefined, JsArray, JsValue}

trait JsonMatchers extends ClassicMatchers {

  def anArrayOf(elementMatcher: BeMatcher[JsValue]) = new BeMatcher[JsValue] {
    def apply(js: JsValue): MatchResult = {
      val failureMessage = s"$js is not an array of desired elements"
      val negatedFailureMessage = s"$js is an array of desired elements"
      js match {
        case JsArray(elems) =>
          val firstError = elems.map(e => elementMatcher(e)).find(!_.matches)
          if (firstError.isDefined) firstError.get
          else MatchResult(matches = true, failureMessage, negatedFailureMessage)
        case _ => MatchResult(matches = false, failureMessage, negatedFailureMessage)
      }
    }
  }

  def containFieldThatMust[T <: JsValue: Manifest](
      expectedField: String, elemMatcher: Matcher[T]) = new Matcher[JsValue] {
    def apply(js: JsValue) = {
      js \ expectedField match {
        case field: T => elemMatcher(field)
        case _ => MatchResult(
          matches = false,
          failureMessage = s"no such field $expectedField in $js",
          negatedFailureMessage = s"field $expectedField found in $js"
        )
      }
    }
  }

  def containFieldWithValue[T <: JsValue: Manifest](expectedField: String, expectedValue: T) =
    containFieldThatMust(expectedField, equal(expectedValue))

  def containsField(expectedField: String) = containFieldThatMust(
    expectedField,
    not be JsUndefined
  )

  def containFieldWithUrl(expectedField: String) = containFieldThatMust(
    expectedField,
    new Matcher[JsString] {
      def apply(url: JsString) = MatchResult(
        matches = Seq("http", "https").exists(proto => new URL(url.value).getProtocol.equals(proto)),
        failureMessage = s"$url is not a URL",
        negatedFailureMessage = s"$url is a URL"
      )
    }
  )

  def beAnArrayWhoseElementsMust[T <: JsValue: Manifest](
      elemMatcher: Matcher[T]) = new Matcher[JsValue] {
    def apply(js: JsValue) = js match {
      case JsArray(fields: Seq[T]) => allElementsMust(elemMatcher)(fields)
      case _ => MatchResult(
        matches = false,
        failureMessage = s"$js is not an array",
        negatedFailureMessage = s"$js is an array"
      )
    }
  }

  private def allElementsMust[T](matcher: Matcher[T]) = new Matcher[Traversable[T]] {
    def apply(left: Traversable[T]) = {
      left.map(matcher(_)).reduce((a, b) => if (a.matches) b else a)
    }
  }
}
