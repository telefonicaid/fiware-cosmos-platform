name := "service-manager"

libraryDependencies ++= Seq(
  "ch.qos.logback" % "logback-classic" % "1.0.7",
  "com.jcraft" % "jsch" % "0.1.49",
  "com.typesafe" %% "scalalogging-slf4j" % "1.0.1",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "mysql" % "mysql-connector-java" % "5.1.10",
  Dependencies.liftJson,
  Dependencies.squeryl,
  Dependencies.typesafeConfig
)
