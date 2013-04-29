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

package es.tid.cosmos.platform.manager.ial

/**
 * Infrastructure provider component trait. This trait is aimed to implement a cake pattern to declare an artifact
 * that requires a infrastructure provider as one of its components.
 *
 * @author sortega
 */
trait InfrastructureProviderComponent {

  /**
   * Obtain the infrastructure provider instance for this component.
   */
  def infrastructureProvider: InfrastructureProvider
}
