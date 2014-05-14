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
import sbt.Keys._
import play.{Keys => PlayKeys}
import com.github.shivawu.sbt.maven.MavenBuild
import com.typesafe.sbt.packager.Keys._

object Build extends sbt.Build {

  val distProject = TaskKey[sbt.File]("dist", "Build project distribution.")

  object POM extends MavenBuild {
    val version = pom.ver
  }

  object Versions {
    /** HDP 2 has a patched 2.2.0 Hadoop stabilized by themselves */
    val hdp2Hadoop = "2.2.0"
  }

  object Dependencies {
    lazy val dispatch = "net.databinder.dispatch" %% "dispatch-core" % "0.10.0"
    lazy val hadoopCommon = "org.apache.hadoop" % "hadoop-common" % Versions.hdp2Hadoop
    lazy val hadoopHdfs = "org.apache.hadoop" % "hadoop-hdfs" % Versions.hdp2Hadoop
    lazy val liftJson = "net.liftweb" %% "lift-json" % "2.6-M2"
    lazy val logbackClassic = "ch.qos.logback" % "logback-classic" % "1.0.13"
    lazy val mockito = "org.mockito" % "mockito-all" % "1.9.5"
    lazy val scalaLogging = "com.typesafe" %% "scalalogging-slf4j" % "1.0.1"
    lazy val scalaz = "org.scalaz" %% "scalaz-core" % "7.0.4"
    lazy val scalatest = "org.scalatest" %% "scalatest" % "1.9.1"
    lazy val squeryl = "org.squeryl" %% "squeryl" % "0.9.5-6"
    lazy val typesafeConfig = "com.typesafe" % "config" % "1.0.0"
  }

  object ExternalSources {
    lazy val servicesConfigDirectory = (baseDirectory) {_ / "../deployment/puppet/modules/cosmos/files/services"}
  }

  override lazy val settings = super.settings ++ Seq(Keys.version in ThisBuild := POM.version)

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings(ScctPlugin.mergeReportSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(rootPackageSettings: _*)
    settings(projectArtifact: _*)
    aggregate(
      cosmosApi, serviceManager, ial, cosmosAdmin, common, common_test, platformTests, infinityfs)
  )

  lazy val common = (Project(id = "common", base = file("common"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    dependsOn(common_test % "compile->compile;test->test")
  )

  lazy val common_test = (Project(id = "common-test", base = file("common-test"))
    settings(ScctPlugin.instrumentSettings: _*)
    settings(Defaults.itSettings: _*)
    configs IntegrationTest
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    dependsOn(common, common_test % "compile->compile;test->test")
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    dependsOn(common, ial, common_test % "compile->compile;test->test")
  )

  lazy val cosmosApi = (play.Project("cosmos-api", POM.version, path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm, PlayKeys.jdbc))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(RpmSettings.cosmosApiSettings: _*)
    dependsOn(serviceManager, common, ial, common_test % "test->compile")
  )

  lazy val cosmosAdmin = (Project(id = "cosmos-admin", base = file("cosmos-admin"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(RpmSettings.cosmosAdminSettings: _*)
    dependsOn(
      serviceManager,
      cosmosApi % "compile->compile;test->test",
      common    % "compile->compile:test->test"
    )
  )

  lazy val platformTests = (Project(id = "platform-tests", base = file("tests/platform-tests"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(parallelExecution in ThisBuild := false)
    dependsOn(
      common_test % "compile->compile;test->test",
      serviceManager % "compile->compile;test->test",
      cosmosApi % "compile->compile;test->test")
  )

  lazy val infinityfs = (Project(id = "infinityfs", base = file("infinityfs"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(InfinityDeployment.settings: _*)
    settings(RpmSettings.infinitySettings: _*)
  )

  def rootPackageSettings: Seq[Setting[_]] = Seq(
    aggregate in dist := false,
    distProject := {
      val s = streams.value

      val filesDir = target.value / "dist/rpms/cosmosplatform/files"
      IO.delete(filesDir)
      filesDir.mkdirs

      s.log.info("Copying cosmos-api RPM to project dist directory...")
      val cosmosApiRPM = (dist in cosmosApi).value
      IO.copyFile(cosmosApiRPM, filesDir / cosmosApiRPM.name)

      s.log.info("Copying cosmos-admin RPM to project dist directory...")
      val cosmosAdminRPM = (dist in cosmosAdmin).value
      IO.copyFile(cosmosAdminRPM, filesDir / cosmosAdminRPM.name)

      s.log.info("Copying infinity RPM to project dist directory...")
      val infinityRPM = (dist in infinityfs).value
      IO.copyFile(infinityRPM, filesDir / infinityRPM.name)

      val distDir = target.value / "dist"
      distDir.mkdirs()

      s.log.info("Copying deployment files to project dist directory...")
      val depBase = baseDirectory.value / "deployment"
      val depFiles = (depBase.*** --- depBase)
      val relativeModulePaths = for {
        (file, name) <- depFiles pair relativeTo(depBase)
      } yield (file, distDir / name)
      IO.copy(relativeModulePaths, overwrite = true)

      val distFile = target.value / s"cosmos-platform-${POM.version}.zip"

      s.log.info("Generating cosmos-platform zip package...")
      val distFiles = (distDir.*** --- distDir) pair relativeTo(distDir)
      IO.delete(distFile)
      IO.zip(distFiles, distFile)

      s.log.success(s"Cosmos platform distribution ready in ${distFile}")
      distFile
    }
  )

  lazy val projectArtifact = addArtifact(Artifact("cosmos-platform", "zip", "zip"), distProject)

}
