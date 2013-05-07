package es.tid.cosmos.api.controllers

import play.api.test.FakeApplication
import play.api.test.Helpers.running

import es.tid.cosmos.api.servicemock.TestGlobal

trait MockedServices {
  def runWithMockedServices[T] = running[T](FakeApplication(withGlobal = Some(TestGlobal)))_
}
