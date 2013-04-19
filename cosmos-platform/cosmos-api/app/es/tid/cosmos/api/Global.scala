package es.tid.cosmos.api

import play.api.{Play, GlobalSettings}
import play.api.Play.current

/**
 * @author sortega
 */
object Global extends GlobalSettings {
  override def getControllerInstance[A](controllerClass: Class[A]): A =
    (if (Play.isProd) ProductionApplication else TestApplication).asInstanceOf[A]
}
