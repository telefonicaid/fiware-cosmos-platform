name := "cosmos-platform"

version := "0.1-SNAPSHOT"

organization in ThisBuild := "es.tid.cosmos"

scalaVersion in ThisBuild := "2.10.1"

libraryDependencies in ThisBuild ++= Seq(
    "org.scalatest" %% "scalatest" % "1.9.1" % "test",
    "org.scalatest" %% "scalatest" % "1.9.1" % "it"
)
