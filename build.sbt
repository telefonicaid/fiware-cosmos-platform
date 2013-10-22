name := "cosmos-platform"

organization in ThisBuild := "es.tid.cosmos"

scalaVersion in ThisBuild := "2.10.2"

scalacOptions in ThisBuild ++= Seq("-deprecation", "-feature")

resolvers in ThisBuild ++= Seq(
    DefaultMavenRepository,
    "Cosmos Nexus Repository" at "http://cosmos10/nexus/content/groups/public/",
    "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases/",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies in ThisBuild ++= Seq(
    "org.scalatest" %% "scalatest" % "1.9.1" % "test",
    "org.scalatest" %% "scalatest" % "1.9.1" % "it",
    "org.mockito" % "mockito-all" % "1.9.5" % "test"
)
