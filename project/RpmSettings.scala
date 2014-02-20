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

import java.text.SimpleDateFormat
import java.util.Date

import com.typesafe.sbt.SbtNativePackager
import com.typesafe.sbt.SbtNativePackager._
import com.typesafe.sbt.packager.Keys._
import com.typesafe.sbt.packager.linux.LinuxSymlink
import sbt._
import sbt.Keys._
import sbtassembly.Plugin.AssemblyKeys._

object RpmSettings {

  val currentTimestamp = new SimpleDateFormat(".yyyyMMdd.HHmmss").format(new Date)

  val commonRpmSettings: Seq[Setting[_]] = Seq(
    dist <<= (packageBin in Rpm) dependsOn dist,
    maintainer := "Cosmos Team <cosmos@tid.es>",
    rpmVendor := "tefdigital",
    rpmGroup := Some("System Environment/Libraries"),
    rpmLicense := Some("All rights reserved"),
    version in Rpm <<= version apply { v =>
      v.replace("-SNAPSHOT", currentTimestamp)
    },
    packageArchitecture in Rpm := "noarch",
    rpmOs in Rpm:= "linux",
    rpmRequirements := Seq("java"),
    rpmAutoreq := "no",
    rpmAutoprov := "no"
  )

  lazy val CosmosCli = config("cosmosCli")

  val cosmosApiSettings: Seq[Setting[_]] = commonRpmSettings ++ Seq(
    name in Rpm := "cosmos-api",
    packageSummary := "Cosmos API",
    packageDescription in Rpm := "Cosmos API server",
    packageBin in CosmosCli := {
      CosmosCliBuildHelper.make(baseDirectory.value.getParentFile / "cosmos-cli", streams.value.log)
    },
    linuxPackageMappings in Rpm <<= (mappings in Universal) map { (f: Seq[(File,String)]) =>
      f map { case (file: File, name: String) =>
        (packageMapping(file -> s"/opt/pdi-cosmos/cosmos-api/$name")
          withUser "root" withGroup "root" withPerms "0755")
      }
    },
    linuxPackageMappings in Rpm <++= baseDirectory map { base =>
      Seq(
        packageMapping(base / "scripts/cosmos-api" -> "/etc/init.d/cosmos-api"),
        packageMapping(IO.temporaryDirectory / "." -> "/opt/pdi-cosmos/var/run/")
          withUser "root" withGroup "root" withPerms "0440"
      )
    },
    linuxPackageMappings in Rpm <+= (baseDirectory, packageBin in CosmosCli) map { (base, egg) =>
      packageMapping(egg -> s"/opt/repos/eggs/${egg.name}")
    },
    linuxPackageSymlinks := {
      Seq(LinuxSymlink("/usr/bin/cosmos-api", "/opt/pdi-cosmos/cosmos-api/bin/cosmos-api"))
    },
    rpmPost := Some("""chmod +x /etc/init.d/cosmos-api
                      |chkconfig --add /etc/init.d/cosmos-api
                      |chkconfig cosmos-api on
                    """.stripMargin)
  )

  val cosmosAdminSettings: Seq[Setting[_]] =
    SbtNativePackager.packageArchetype.java_application ++ commonRpmSettings ++
    Seq(
      name in Rpm := "cosmos-admin",
      packageSummary := "Cosmos Admin",
      packageDescription in Rpm := "Cosmos Admin server",
      linuxPackageMappings in Rpm <<= (mappings in Universal) map { (files: Seq[(File,String)]) =>
        for {
          (file, name) <- files if name.endsWith(".jar")
        } yield {
          (packageMapping(file -> s"/opt/pdi-cosmos/cosmos-admin/lib/${file.name}")
            withUser "root" withGroup "root" withPerms "0755")
        }
      },
      linuxPackageMappings in Rpm <++= baseDirectory map { base =>
        Seq(
          packageMapping ( base / "scripts/cosmos-admin" -> "/opt/pdi-cosmos/cosmos-admin/cosmos-admin")
            withUser "root" withGroup "root" withPerms "0755",
          packageMapping ( IO.temporaryDirectory / "." -> "/opt/pdi-cosmos/cosmos-admin/logs")
            withUser "root" withGroup "root" withPerms "0755"
        )
      },
      linuxPackageSymlinks := {
        Seq(LinuxSymlink("/usr/bin/cosmos-admin", "/opt/pdi-cosmos/cosmos-admin/cosmos-admin"))
      }
    )

  val JarPath = "/opt/pdi-cosmos/infinityfs.jar"
  val JarLinks = for (service <- Seq("hdfs", "mapreduce", "yarm"))
    yield s"/usr/lib/hadoop-$service/lib/infinityfs.jar"

  val infinitySettings: Seq[Setting[_]] = commonRpmSettings ++ Seq(
    name in Rpm := "infinityfs",
    packageSummary := "Infinity HFS driver",
    packageDescription in Rpm := "Library that enables the infinity:// scheme.",
    linuxPackageMappings in Rpm := Seq(
      packageMapping(assembly.value -> JarPath) withUser "root" withGroup "root" withPerms "0755"
    ),
    linuxPackageSymlinks in Rpm := JarLinks.map(destination => LinuxSymlink(JarPath, destination))
  )

}
