name := "service-manager"

libraryDependencies ++= Seq(
  "com.jcraft" % "jsch" % "0.1.49",
  "com.imageworks.scala-migrations" %% "scala-migrations" % "1.1.1",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "mysql" % "mysql-connector-java" % "5.1.10",
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.scalaLogging,
  Dependencies.squeryl,
  Dependencies.typesafeConfig
)
