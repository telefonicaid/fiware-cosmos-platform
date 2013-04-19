package es.tid.cosmos.api

import play.api.GlobalSettings

/**
 * @author sortega
 */
object Global extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A =
    ProductionApplication.asInstanceOf[A]
}
