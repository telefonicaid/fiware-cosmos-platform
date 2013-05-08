name := "service-manager"

mainClass := Some("scalarest.Main")

libraryDependencies ++= Seq(
  "org.mockito" % "mockito-all" % "1.9.5" % "test",
  "com.typesafe" % "config" % "1.0.0",
  "org.scalatest" %% "scalatest" % "1.9.1" % "test",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "net.liftweb" %% "lift-json" % "2.5-RC4"
)
