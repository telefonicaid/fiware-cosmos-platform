package es.tid.cosmos.scalarest

import es.tid.cosmos.platform.manager.ial.{InfrastructureProviderComponent, InfrastructureProvider}

/**
 * @author sortega
 */
trait FakeInfrastructureProviderComponent extends InfrastructureProviderComponent {
  def infrastructureProvider: InfrastructureProvider = new FakeInfrastructureProvider
}
