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
