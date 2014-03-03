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

package es.tid.cosmos.servicemanager.ambari.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.{MatchResult, Matcher, MustMatchers}

class DependencyMappingTest extends FlatSpec with MustMatchers {

  "Dependency mapping" must "resolve dependencies when there are no additional dependencies" in {
    val mapping = DependencyMapping(3 -> Set(4))
    mapping.resolve(Set(1, 2)) must be (Set(1, 2))
  }

  it must "resolve dependencies when dependencies are internal to the input set" in {
    val mapping = DependencyMapping(
      1 -> Set(2),
      2 -> Set(3)
    )
    mapping.resolve(Set(1, 2, 3)) must be (Set(1, 2, 3))
  }

  it must "resolve direct dependencies" in {
    val mapping = DependencyMapping(
      1 -> Set(3),
      2 -> Set(4)
    )
    mapping.resolve(Set(1, 2)) must be (Set(1, 2, 3, 4))
  }

  it must "resolve transitive dependencies" in {
    val mapping = DependencyMapping(
      1 -> Set(2, 3),
      2 -> Set(4)
    )
    mapping.resolve(Set(1)) must be (Set(1, 2, 3, 4))
  }

  it must "put dependencies after dependants when ordering for execution" in {
    val mapping = DependencyMapping(
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
    val mapping = DependencyMapping(
      "a" -> Set("b", "d"),
      "b" -> Set("c"),
      "c" -> Set("a")
    )
    evaluating {
      mapping.executionOrder(Set("a", "b", "c", "d"))
    } must produce [CyclicDependencyException]
  }

  def containInOrder[T](elements: T*): Matcher[Seq[T]] = new Matcher[Seq[T]] {
    override def apply(sequence: Seq[T]): MatchResult = MatchResult(
      matches = sequence.filter(elements.toSet) == elements,
      failureMessage = s"$sequence does not contains $elements in the same relative order",
      negatedFailureMessage = s"$sequence contains $elements in the same relative order"
    )
  }
}
