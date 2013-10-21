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

import sbt._
import play.{Keys => PlayKeys}
import com.github.shivawu.sbt.maven.MavenBuild

object CosmosBuild extends Build {

  object POM extends MavenBuild {
    val version = pom.ver
  }

  object Dependencies {
    lazy val liftJson = "net.liftweb" %% "lift-json" % "2.5.1"
    lazy val typesafeConfig = "com.typesafe" % "config" % "1.0.0"
  }

  override lazy val settings = super.settings ++ Seq(Keys.version in ThisBuild := POM.version)

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings(sbt.scct.ScctPlugin.mergeReportSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    aggregate(cosmosApi, serviceManager, ial, cosmosAdmin, common, platformTests)
  )

  lazy val common = (Project(id = "common", base = file("common"))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common_test % "compile->compile;test->test")
  )

  lazy val common_test = (Project(id = "common-test", base = file("common-test"))
    settings(Defaults.itSettings : _*)
    configs(IntegrationTest)
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common, common_test % "compile->compile;test->test")
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(ial, common_test % "compile->compile;test->test")
  )

  lazy val cosmosApi = (play.Project("cosmos-api", POM.version, path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm, PlayKeys.jdbc))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(serviceManager)
  )

  lazy val cosmosAdmin = (Project(id = "cosmos-admin", base = file("cosmos-admin"))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(cosmosApi, serviceManager, common)
  )

  lazy val platformTests = (Project(id = "platform-tests", base = file("tests/platform-tests"))
    settings(sbt.scct.ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common_test % "compile->compile;test->test")
  )
}
