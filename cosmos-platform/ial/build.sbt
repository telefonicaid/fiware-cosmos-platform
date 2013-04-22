name := "Cosmos IAL"

scalaVersion := "2.10.1"

libraryDependencies  ++=  Seq(
  "org.squeryl" %% "squeryl" % "0.9.5-6",
  "mysql" % "mysql-connector-java" % "5.1.10",
  "org.scalatest" %% "scalatest" % "1.9.1" % "it"
)
