name := "cosmos-platform"

organization in ThisBuild := "es.tid.cosmos"

// Note: This is the Scala version used by Play 2.2.2. Ensure Play compatibility before upgrading.
scalaVersion in ThisBuild := "2.10.3"

scalacOptions in ThisBuild ++= Seq("-deprecation", "-feature")

addCommandAlias("run-local-it", ";it:compile ;it:test-only * -- -l \"HasExternalDependencies EndToEndTest\"")

resolvers in ThisBuild ++= Seq(
    DefaultMavenRepository,
    "Cosmos Nexus Repository" at "http://cosmos10/nexus/content/groups/public/",
    "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases/",
    "Twitter" at "http://maven.twttr.com",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies in ThisBuild ++= Seq(
  Dependencies.mockito % "test, it",
  Dependencies.scalatest % "test, it"
)

publishTo := {
  val releaseType = if (version.value.trim.endsWith("SNAPSHOT")) "snapshots" else "releases"
  Some(releaseType  at "http://cosmos10.hi.inet/nexus/content/repositories/" + releaseType)
}

credentials += Credentials(Path.userHome / ".ivy2" / ".credentials")

cleanKeepFiles := (target.value * "centos-6-cosmos.HDP.*").get
