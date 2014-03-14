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

package es.tid.cosmos.servicemanager.services

import scala.annotation.tailrec

import es.tid.cosmos.servicemanager.ambari.services.dependencies.CyclicDependencyException

/** Simple transitive dependency mapping.  */
private[services] class DependencyMapping[T](dependencyMapping: Map[T, Set[T]]) {

  def this(dependencyMapping: (T, Set[T])*) = this(dependencyMapping.toMap)

  private val dependencies = dependencyMapping.withDefaultValue(Set.empty)

  /** Resolves services dependencies transitively. */
  @tailrec
  final def resolve(entities: Set[T]): Set[T] = {
    val withDirectDependencies = entities union directDependencies(entities)
    if (withDirectDependencies == entities) entities
    else resolve(withDirectDependencies)
  }

  private def directDependencies(entities: Set[T]) = entities.flatMap(dependencies)

  /** Arranges services in a valid execution order or throws an exception if it is not possible.
    *
    * @param entities Entities to be executed
    * @return         A valid execution order in which entities appear before their dependencies
    * @throws CyclicDependencyException  If there is such cycle
    */
  def executionOrder(entities: Set[T]): Seq[T] = {
    val entitiesWithDeps = entities.map(e => (e, dependencies(e))).toMap
    topologicalSort(entitiesWithDeps)
  }

  /** Sort topologically a dependency graph.
    *
    * @param entities  Dependency modelled as a dictionary from entity to entity dependencies
    * @param sorted    Already sorted entities, used as accumulator on tail recursion
    * @return          Topologically sorted sequence of entities
    * @throws CyclicDependencyException  If there is such cycle
    */
  @tailrec
  private def topologicalSort(entities: Map[T, Set[T]], sorted: Seq[T] = Seq.empty): Seq[T] =
    if (entities.isEmpty) sorted
    else {
      val executableEntities = entities.collect {
        case (entity, entityDeps) if entityDeps.isEmpty => entity
      }.toSet
      if (executableEntities.isEmpty)
        throw new CyclicDependencyException(entities.keys.toSeq.map(_.toString))
      val nextEntities = entities.collect {
        case (entity, entityDeps) if !executableEntities.contains(entity) =>
          entity -> (entityDeps diff executableEntities)
      }
      topologicalSort(nextEntities, sorted ++ executableEntities)
    }
}
