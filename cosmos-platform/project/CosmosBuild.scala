import sbt._
import Keys._

object CosmosBuild extends Build {
  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings (ScctPlugin.mergeReportSettings: _*)
    aggregate(cosmosApi, serviceManager))
  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings (ScctPlugin.instrumentSettings: _*))
  lazy val cosmosApi = (Project(id = "cosmos-api", base = file("cosmos-api"))
    settings (ScctPlugin.instrumentSettings: _*)
    dependsOn(serviceManager))
}
