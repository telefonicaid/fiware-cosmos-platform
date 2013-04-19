name := "service-manager"

scalaVersion := "2.10.1"

mainClass := Some("scalarest.Main")

libraryDependencies ++= Seq(
  "org.scalatest" %% "scalatest" % "1.9.1" % "test",
  "net.databinder.dispatch" %% "dispatch-core" % "0.10.0",
  "net.liftweb" %% "lift-json" % "2.5-RC4"
)
