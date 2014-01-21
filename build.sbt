name := "cosmos-platform"

organization in ThisBuild := "es.tid.cosmos"

// Note: This is the Scala version used by Play 2.2.0. Ensure Play compatibility before upgrading.
scalaVersion in ThisBuild := "2.10.2"

scalacOptions in ThisBuild ++= Seq("-deprecation", "-feature")

addCommandAlias("run-local-it", ";it:compile ;it:test-only * -- -l \"HasExternalDependencies EndToEndTest\"")

resolvers in ThisBuild ++= Seq(
    DefaultMavenRepository,
    "Cosmos Nexus Repository" at "http://cosmos10/nexus/content/groups/public/",
    "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases/",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies in ThisBuild ++= Seq(
  Dependencies.mockito % "test, it",
  Dependencies.scalatest % "test, it"
)
