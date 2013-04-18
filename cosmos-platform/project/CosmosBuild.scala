import sbt._
import Keys._

object CosmosBuild extends Build {
  lazy val root = Project(id = "cosmos-platform",
                          base = file(".")) aggregate(cosmosApi, serviceManager)
  lazy val serviceManager = Project(id = "service-manager", base = file("service-manager"))
  lazy val cosmosApi = Project(id = "cosmos-api",
                               base = file("cosmos-api")) dependsOn(serviceManager)
}
