name := "service-manager"

libraryDependencies ++= Seq(
  "ch.qos.logback" % "logback-classic" % "1.0.7",
  "com.jcraft" % "jsch" % "0.1.49",
  "com.typesafe" %% "scalalogging-slf4j" % "1.0.1",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  Dependencies.liftJson,
  Dependencies.typesafeConfig
)
