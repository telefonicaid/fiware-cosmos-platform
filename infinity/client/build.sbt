name := "infinity-client"

description := "Infinity Client"

libraryDependencies ++= Dependencies.unfiltered.map(_ % "test, it") ++ Seq(
  Dependencies.commonsLogging % "provided, test, it",
  Dependencies.commonsIO % "test, it",
  Dependencies.dispatch,
  Dependencies.log4j % "test",
  Dependencies.scalaStm % "test",
  Dependencies.jettyWebapp % "it"
)
