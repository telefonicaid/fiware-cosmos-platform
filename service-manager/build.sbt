name := "service-manager"

libraryDependencies ++= Seq(
  "ch.qos.logback" % "logback-classic" % "1.0.7",
  "com.jcraft" % "jsch" % "0.1.49",
  "com.typesafe" % "config" % "1.0.0",
  "com.typesafe" %% "scalalogging-slf4j" % "1.0.1",
  "com.wordnik" %% "swagger-play2" % "1.2.3",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "net.liftweb" %% "lift-json" % "2.5-RC6"
)
