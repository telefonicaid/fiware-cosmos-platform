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

package es.tid.cosmos.servicemanager.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

class DependencyMappingTest extends FlatSpec with MustMatchers {

  "Dependency mapping" must "resolve dependencies when there are no additional dependencies" in {
    val mapping = DependencyMapping.requiredDependencies(3 -> Set(4))
    mapping.resolve(Set(1, 2)) must be (Set(1, 2))
  }

  it must "resolve dependencies when dependencies are internal to the input set" in {
    val mapping = DependencyMapping.requiredDependencies(
      1 -> Set(2),
      2 -> Set(3)
    )
    mapping.resolve(Set(1, 2, 3)) must be (Set(1, 2, 3))
  }

  it must "resolve direct dependencies" in {
    val mapping = DependencyMapping.requiredDependencies(
      1 -> Set(3),
      2 -> Set(4)
    )
    mapping.resolve(Set(1, 2)) must be (Set(1, 2, 3, 4))
  }

  it must "resolve transitive dependencies" in {
    val mapping = DependencyMapping.requiredDependencies(
      1 -> Set(2, 3),
      2 -> Set(4)
    )
    mapping.resolve(Set(1)) must be (Set(1, 2, 3, 4))
  }

  it must "put dependencies after dependants when ordering for execution" in {
    val mapping = DependencyMapping.requiredDependencies(
      "a" -> Set("a1", "a2"),
      "b" -> Set("b1", "b2")
    )
    val execution = mapping.executionOrder(Set("a1", "a2", "a", "b1", "b2", "b"))
    execution must containInOrder("a1", "a")
    execution must containInOrder("a2", "a")
    execution must containInOrder("b1", "b")
    execution must containInOrder("b2", "b")
  }

  it must "detect dependency cycles" in {
    val mapping = DependencyMapping.requiredDependencies(
      "a" -> Set("b", "d"),
      "b" -> Set("c"),
      "c" -> Set("a")
    )
    evaluating {
      mapping.executionOrder(Set("a", "b", "c", "d"))
    } must produce [CyclicDependencyException]
  }

  it must "not resolve missing optional dependencies" in {
    val mapping = new DependencyMapping(Map(
      "a" -> Map("b" -> Optional, "c" -> Optional),
      "b" -> Map("d" -> Required)
    ))
    mapping.resolve(Set("a", "b")) must be (Set("a", "b", "d"))
  }

  it must "consider optional dependencies for the execution order" in {
    val mapping = new DependencyMapping(Map(
      "a" -> Map("b" -> Required, "e" -> Optional),
      "b" -> Map("d" -> Required)
    ))
    val execution = mapping.executionOrder(Set("a", "b", "d", "e"))
    execution must containInOrder("b", "a")
    execution must containInOrder("e", "a")
    execution must containInOrder("d", "a")
  }

  def containInOrder[T](elements: T*): Matcher[Seq[T]] = new Matcher[Seq[T]] {
    override def apply(sequence: Seq[T]): MatchResult = MatchResult(
      matches = sequence.filter(elements.toSet) == elements,
      failureMessage = s"$sequence does not contains $elements in the same relative order",
      negatedFailureMessage = s"$sequence contains $elements in the same relative order"
    )
  }
}
