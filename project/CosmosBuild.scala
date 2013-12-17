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
    lazy val dispatch = "net.databinder.dispatch" %% "dispatch-core" % "0.10.0"
    lazy val liftJson = "net.liftweb" %% "lift-json" % "2.5.1"
    lazy val logbackClassic = "ch.qos.logback" % "logback-classic" % "1.0.13"
    lazy val typesafeConfig = "com.typesafe" % "config" % "1.0.0"
    lazy val scalaLogging = "com.typesafe" %% "scalalogging-slf4j" % "1.0.1"
    lazy val scalaz = "org.scalaz" %% "scalaz-core" % "7.0.4"
    lazy val squeryl = "org.squeryl" %% "squeryl" % "0.9.5-6"
  }

  override lazy val settings = super.settings ++ Seq(Keys.version in ThisBuild := POM.version)

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings(ScctPlugin.mergeReportSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    aggregate(cosmosApi, serviceManager, ial, cosmosAdmin, common, common_test, platformTests)
  )

  lazy val common = (Project(id = "common", base = file("common"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common_test % "compile->compile;test->test")
  )

  lazy val common_test = (Project(id = "common-test", base = file("common-test"))
    settings(ScctPlugin.instrumentSettings: _*)
    settings(Defaults.itSettings : _*)
    configs(IntegrationTest)
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common, common_test % "compile->compile;test->test")
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(ial, common_test % "compile->compile;test->test")
  )

  lazy val cosmosApi = (play.Project("cosmos-api", POM.version, path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm, PlayKeys.jdbc))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(serviceManager)
    dependsOn(common_test) % "test->compile"
  )

  lazy val cosmosAdmin = (Project(id = "cosmos-admin", base = file("cosmos-admin"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(
      serviceManager,
      cosmosApi % "compile->compile;test->test",
      common    % "compile->compile:test->test"
    )
  )

  lazy val platformTests = (Project(id = "platform-tests", base = file("tests/platform-tests"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs(IntegrationTest)
    settings(Defaults.itSettings : _*)
    dependsOn(common_test % "compile->compile;test->test")
  )
}
