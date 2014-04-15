sourceDirectory in IntegrationTest <<= baseDirectory / "it"

unmanagedClasspath in IntegrationTest <+= (classDirectory in Test) map { t => Attributed.blank(t) }

compile in IntegrationTest <<= compile in IntegrationTest dependsOn (compile in Test)

libraryDependencies ++= Dependencies.unfiltered ++ Seq(
  Dependencies.scalaz,
  "com.typesafe" %% "play-plugins-mailer" % "2.2.0",
  // TODO: Taken from SNAPSHOT to work with Play 2.2.0. Replace with released version once available
  "com.wordnik" %% "swagger-play2" % "1.2.6-Cosmos",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "mysql" % "mysql-connector-java" % "5.1.10",
  "org.scala-stm" %% "scala-stm" % "0.7" % "it",
  "com.typesafe.play" %% "play-test" % play.core.PlayVersion.current % "test, it"
)
