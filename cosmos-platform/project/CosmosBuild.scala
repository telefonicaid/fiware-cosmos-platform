import sbt._

object CosmosBuild extends Build {

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings (ScctPlugin.mergeReportSettings: _*)
    aggregate(cosmosApi, serviceManager, ial)
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings (ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings (Defaults.itSettings : _*)
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings (ScctPlugin.instrumentSettings: _*)
    dependsOn(ial)
  )

  lazy val cosmosApi = (play.Project("cosmos-api", "1.0-SNAPSHOT", path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm))
    settings (ScctPlugin.instrumentSettings: _*)
    dependsOn(serviceManager)
  )
}
