/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import sbt._
import sbt.Keys._
import play.{Keys => PlayKeys}
import com.typesafe.sbt.packager.Keys._
import sbtassembly.Plugin._

object Build extends sbt.Build {

  val distProject = TaskKey[sbt.File]("dist", "Build project distribution.")

  val projectVersion = IO.read(file("VERSION")).trim

  object Versions {
    val akka = "2.3.0"
    val hdp2Hadoop = "2.2.0" /** HDP 2 has a patched 2.2.0 Hadoop stabilized by themselves */
    val spray = "1.3.0"
  }

  object JavaVersions {
    val java6 = forVersion("1.6")
    val java7 = forVersion("1.7")

    private def forVersion(version: String) = {
      val forScala = scalacOptions ++= Seq(s"-target:jvm-$version")
      val forJava = javacOptions ++= Seq("-source", version, "-target", version)
      forScala ++ forJava
    }
  }

  object Dependencies {
    lazy val anorm = "play" %% "anorm" % "2.1.5"
    lazy val commonsCodec = "commons-codec" % "commons-codec" % "1.9"
    lazy val commonsLogging = "commons-logging" % "commons-logging" % "1.1.1"
    lazy val dispatch = "net.databinder.dispatch" %% "dispatch-core" % "0.10.0"
    lazy val finatra = "com.twitter" %% "finatra" % "1.5.2" exclude(
      "org.scalatest", "scalatest_2.10")
    lazy val finagleStream = "com.twitter" %% "finagle-stream" % "6.11.1"
    lazy val h2database = "com.h2database" % "h2" % "1.3.175"
    lazy val hadoopCommon = "org.apache.hadoop" % "hadoop-common" % Versions.hdp2Hadoop
    lazy val hadoopHdfs = "org.apache.hadoop" % "hadoop-hdfs" % Versions.hdp2Hadoop
    lazy val liftJson = "net.liftweb" %% "lift-json" % "2.6-M2"
    lazy val log4j = "log4j" % "log4j" % "1.2.17"
    lazy val logbackClassic = "ch.qos.logback" % "logback-classic" % "1.0.13"
    lazy val mockito = "org.mockito" % "mockito-all" % "1.9.5"
    lazy val scalalikejdbc = "com.github.seratch" %% "scalikejdbc" % "[0.5,)"
    lazy val scalaLogging = "com.typesafe" %% "scalalogging-slf4j" % "1.0.1"
    lazy val scalaMigrations = "com.imageworks.scala-migrations" %% "scala-migrations" % "1.1.1"
    lazy val scalatest = "org.scalatest" %% "scalatest" % "1.9.1"
    lazy val scalaStm = "org.scala-stm" %% "scala-stm" % "0.7"
    lazy val scalaz = "org.scalaz" %% "scalaz-core" % "7.0.4"
    lazy val squeryl = "org.squeryl" %% "squeryl" % "0.9.5-6"
    lazy val typesafeConfig = "com.typesafe" % "config" % "1.2.0"
    lazy val unfiltered = Seq(
      "net.databinder" %% "unfiltered-filter" % "0.6.8" % "test, it",
      "net.databinder" %% "unfiltered-jetty" % "0.6.8" % "test, it",
      "org.eclipse.jetty.orbit" % "javax.servlet" % "3.0.0.v201112011016" % "test, it" artifacts (
        Artifact("javax.servlet", "jar", "jar")),
      "org.eclipse.jetty.orbit" % "javax.servlet" % "3.0.0.v201112011016" % "test, it" artifacts (
        Artifact("javax.servlet", "jar", "jar")),
      "org.eclipse.jetty" % "jetty-webapp" % "7.0.0.v20091005" % "it" artifacts (
        Artifact("jetty-webapp", "jar", "jar"))
    )
  }

  object ExternalSources {
    lazy val servicesConfigDirectory = (baseDirectory) {_ / "../deployment/puppet/modules/cosmos/templates/services"}
  }

  override lazy val settings = super.settings ++ Seq(Keys.version in ThisBuild := projectVersion)

  lazy val root = (Project(id = "cosmos-platform", base = file("."))
    settings(ScctPlugin.mergeReportSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(rootPackageSettings: _*)
    settings(projectArtifact: _*)
    aggregate(
      cosmosApi, ambariServiceManager, serviceManager, ial, cosmosAdmin, common, common_test,
      platformTests, infinityDriver, infinityServer, infinityCommon, infinityClient
    )
  )

  lazy val common = (Project(id = "common", base = file("common"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(JavaVersions.java6: _*)
    dependsOn(common_test % "test")
  )

  lazy val common_test = (Project(id = "common-test", base = file("common-test"))
    settings(ScctPlugin.instrumentSettings: _*)
    settings(Defaults.itSettings: _*)
    configs IntegrationTest
    settings(JavaVersions.java6: _*)
  )

  lazy val ial = (Project(id = "ial", base = file("ial"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(JavaVersions.java7: _*)
    dependsOn(common, common_test % "test, it")
  )

  lazy val serviceManager = (Project(id = "service-manager", base = file("service-manager"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(JavaVersions.java7: _*)
    dependsOn(common, ial, common_test % "test, it")
  )

  lazy val ambariServiceManager =
    (Project(id = "ambari-service-manager", base = file("ambari-service-manager"))
      settings(ScctPlugin.instrumentSettings: _*)
      configs IntegrationTest
      settings(Defaults.itSettings: _*)
      settings(JavaVersions.java7: _*)
      dependsOn(serviceManager % "compile->compile;test->test", common, ial, common_test % "test, it")
    )

  lazy val cosmosApi = (play.Project("cosmos-api", projectVersion, path = file("cosmos-api"),
                        dependencies = Seq(PlayKeys.anorm, PlayKeys.jdbc))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(RpmSettings.cosmosApiSettings: _*)
    settings(JavaVersions.java7: _*)
    dependsOn(ambariServiceManager, serviceManager, common, ial, common_test % "test, it")
  )

  lazy val cosmosAdmin = (Project(id = "cosmos-admin", base = file("cosmos-admin"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(RpmSettings.cosmosAdminSettings: _*)
    settings(JavaVersions.java7: _*)
    dependsOn(
      ambariServiceManager,
      serviceManager,
      cosmosApi % "compile->compile;test->test,it",
      common % "compile->compile;test->test"
    )
  )

  lazy val platformTests = (Project(id = "platform-tests", base = file("tests/platform-tests"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(parallelExecution in ThisBuild := false)
    settings(JavaVersions.java7: _*)
    dependsOn(
      common_test % "test, it",
      serviceManager % "compile->compile;test->test",
      cosmosApi % "compile->compile;test->test")
  )

  lazy val infinityCommon = (Project(id = "infinity-common", base = file("infinity/common"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(JavaVersions.java6: _*)
  )

  lazy val infinityClient = (Project(id = "infinity-client", base = file("infinity/client"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(JavaVersions.java6: _*)
    dependsOn(infinityCommon, common, common_test % "test, it")
  )

  lazy val infinityDriver = (Project(id = "infinity-driver", base = file("infinity/driver"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(RpmSettings.infinityDriverSettings: _*)
    settings(JavaVersions.java6: _*)
    dependsOn(infinityClient, infinityCommon)
  )

  lazy val infinityServer = (Project(id = "infinity-server", base = file("infinity/server"))
    settings(ScctPlugin.instrumentSettings: _*)
    configs IntegrationTest
    settings(Defaults.itSettings: _*)
    settings(buildSettings: _*)
    settings(RpmSettings.infinityServerSettings: _*)
    settings(JavaVersions.java6: _*)
    dependsOn(infinityCommon, common, common_test % "test")
  )

  def rootPackageSettings: Seq[Setting[_]] = Seq(
    aggregate in dist := false,
    aggregate in publish := false,
    publish <<= publish.dependsOn(distProject),
    publishArtifact in (Compile, packageBin) := false,
    publishArtifact in (Compile, packageSrc) := false,
    publishArtifact in (Compile, packageDoc) := false,
    crossPaths := false,
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
      val infinityRPM = (dist in infinityDriver).value
      IO.copyFile(infinityRPM, filesDir / infinityRPM.name)

      s.log.info("Copying infinity-server RPM to project dist directory...")
      val infinityServerRPM = (dist in infinityServer).value
      IO.copyFile(infinityServerRPM, filesDir / infinityServerRPM.name)

      val distDir = target.value / "dist"
      distDir.mkdirs()

      s.log.info("Copying deployment files to project dist directory...")
      val depBase = baseDirectory.value / "deployment"
      val depFiles = (depBase.*** --- depBase)
      val relativeModulePaths = for {
        (file, name) <- depFiles pair relativeTo(depBase)
      } yield (file, distDir / name)
      IO.copy(relativeModulePaths, overwrite = true)

      val distFile = target.value / s"cosmos-platform-$projectVersion.zip"

      s.log.info("Generating cosmos-platform zip package...")
      val distFiles = (distDir.*** --- distDir) pair relativeTo(distDir)
      IO.delete(distFile)
      IO.zip(distFiles, distFile)

      s.log.success(s"Cosmos platform distribution ready in $distFile")
      distFile
    }
  )

  lazy val projectArtifact = addArtifact(Artifact("cosmos-platform", "zip", "zip"), distProject)
}
