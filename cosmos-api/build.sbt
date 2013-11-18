sourceDirectory in IntegrationTest <<= baseDirectory / "it"

// orbit workaround https://jira.codehaus.org/browse/JETTY-1493
classpathTypes ~= (_ + "orbit")

libraryDependencies ++= Seq(
  Dependencies.scalaz,
  // TODO: Taken from SNAPSHOT to work with Play 2.2.0. Replace with released version once available
  "com.wordnik" %% "swagger-play2" % "1.2.6-Cosmos",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "mysql" % "mysql-connector-java" % "5.1.10",
  "org.scala-stm" %% "scala-stm" % "0.7" % "it",
  "net.databinder" %% "unfiltered-filter" % "0.6.8" % "test, it",
  "net.databinder" %% "unfiltered-jetty" % "0.6.8" % "test, it",
  "org.eclipse.jetty.orbit" % "javax.servlet" % "3.0.0.v201112011016" % "test, it" artifacts (
    Artifact("javax.servlet", "jar", "jar")),
  "org.eclipse.jetty.orbit" % "javax.servlet" % "3.0.0.v201112011016" % "test, it" artifacts (
    Artifact("javax.servlet", "jar", "jar")),
  "org.eclipse.jetty" % "jetty-webapp" % "7.0.0.v20091005" % "it" artifacts (
    Artifact("jetty-webapp", "jar", "jar")),
  "com.typesafe.play" %% "play-test" % play.core.PlayVersion.current % "it"
)
