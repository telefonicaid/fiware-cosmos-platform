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

import java.net.URI
import java.text.SimpleDateFormat
import java.util.Date
import scala.concurrent.Await
import scala.concurrent.duration.Duration

import sbt._
import sbt.Keys._
import sbt.URI
import sbtassembly.Plugin._
import sbtassembly.Plugin.AssemblyKeys._
import com.typesafe.sbt.packager.Keys._
import com.typesafe.sbt.packager.linux.LinuxSymlink
import com.typesafe.sbt.SbtNativePackager._

object InfinityDeployment {

  val settings: Seq[Setting[_]] = assemblySettings ++ packagerSettings ++ rpmSettings ++
    publishRpmSettings ++ Seq(
    jarName in assembly <<= name.map(_ + ".jar"),
    dist <<= (packageBin in Rpm)
  )

  val JarPath = "/opt/pdi-cosmos/infinityfs.jar"
  val JarLinks = for (service <- Seq("hdfs", "mapreduce", "yarm"))
    yield s"/usr/lib/hadoop-$service/lib/infinityfs.jar"

  private def publishRpmSettings: Seq[Setting[_]] = inConfig(Rpm)(Seq(
    publish := {
      val repo = publishTo.value
      repo match {
        case Some(MavenRepository(_, root)) => publishContent(
          repo = root,
          group = organization.value,
          name = (name in Rpm).value,
          version = (version in ThisBuild).value,
          creds = credentials.value,
          rpm = (packageBin in Rpm).value,
          log = streams.value.log
        )
        case Some(_) => sys.error(s"Cannot publish RPMs to $repo")
        case None => sys.error("Please, define where to publish the RPM ('publishTo')")
      }
    }
  ))

  private def publishContent(
      repo: String,
      group: String,
      name: String,
      version: String,
      creds: Seq[sbt.Credentials],
      rpm: File,
      log: Logger) {
    val uri = (Seq(repo) ++ group.split("\\.") ++ Seq(name, version, rpm.getName)).mkString("/")
    val (u,p) = findCreds(URI.create(repo), creds).headOption
      .getOrElse(sys.error(s"Unable to find credentials for $repo"))

    import dispatch._, Defaults._
    val upload = Http(url(uri).as_!(u, p) <<< rpm).transform(
      s = _ => log.info(s"Uploaded RPM to $uri"),
      f = ex => sys.error("Cannot upload RPM: " + ex)
    )
    Await.ready(upload, Duration.Inf)
  }

  private def rpmSettings: Seq[Setting[_]] = Seq(
    name in Rpm := "infinityfs",
    maintainer := "Cosmos Team <cosmos@tid.es>",
    packageSummary := "Infinity HFS driver",
    description in Rpm := "Library that enables the infinity:// scheme.",
    rpmVendor := "Telefonica Digital",
    rpmGroup := Some("System Environment/Libraries"),
    rpmLicense := Some("All rights reserved"),
    version in Rpm <<= version apply { v =>
      v.replace("-SNAPSHOT", currentTimestamp())
    },
    linuxPackageMappings in Rpm := Seq(
      packageMapping(assembly.value -> JarPath) withUser "root" withGroup "root" withPerms "0755"
    ),
    linuxPackageSymlinks in Rpm := JarLinks.map(destination => LinuxSymlink(JarPath, destination))
  )

  private def findCreds(uri: URI, creds: Seq[sbt.Credentials]): Seq[(String, String)] = for {
    c <- creds
    cred = Credentials.toDirect(c)
    if cred.host == uri.getHost
  } yield cred.userName -> cred.passwd

  private def currentTimestamp() = new SimpleDateFormat(".yyyyMMdd.HHmmss").format(new Date)
}
