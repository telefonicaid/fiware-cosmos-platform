name := "service-manager"

mainClass := Some("scalarest.Main")

resolvers ++= Seq(
    DefaultMavenRepository,
    "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases/",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies ++= Seq(
  "com.typesafe" % "config" % "1.0.0",
  "com.wordnik" %% "swagger-play2" % "1.2.3",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "net.liftweb" %% "lift-json" % "2.5-RC4",
  "org.mockito" % "mockito-all" % "1.9.5" % "test",
  "org.scalatest" %% "scalatest" % "1.9.1" % "test"
)
