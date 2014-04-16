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

import scala.annotation.tailrec

/** Simple transitive dependency mapping.  */
private[services] class DependencyMapping[T](dependencyMapping: Map[T, Map[T, DependencyType]]) {

  private val dependencies = dependencyMapping.withDefaultValue(Map.empty)

  /** Resolves services dependencies transitively. */
  @tailrec
  final def resolve(entities: Set[T]): Set[T] = {
    val withDirectDependencies = entities union directRequiredDependencies(entities)
    if (withDirectDependencies == entities) entities
    else resolve(withDirectDependencies)
  }

  private def directRequiredDependencies(entities: Set[T]): Set[T] =
    entities.flatMap(dependencies).collect {
      case (dependency, Required) => dependency
    }

  /** Arranges services in a valid execution order or throws an exception if it is not possible.
    *
    * @param entities Entities to be executed
    * @return         A valid execution order in which entities appear before their dependencies
    * @throws CyclicDependencyException  If there is such cycle
    */
  def executionOrder(entities: Set[T]): Seq[T] = {

    def executionDependencies(entity: T): Set[T] = dependencies(entity).collect {
      case (requiredDep, Required) => requiredDep
      case (optionalDep, Optional) if entities.contains(optionalDep) => optionalDep
    }.toSet

    val entitiesWithDependencies = entities.map { entity =>
      entity -> executionDependencies(entity)
    }.toMap
    topologicalSort(entitiesWithDependencies)
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

private[services] object DependencyMapping {

  def requiredDependencies[T](dependencyMapping: (T, Set[T])*) = new DependencyMapping[T](
    dependencyMapping.map {
      case (entity, requiredDeps) => (entity, requiredDeps.zip(Stream.continually(Required)).toMap)
    }.toMap
  )
}
