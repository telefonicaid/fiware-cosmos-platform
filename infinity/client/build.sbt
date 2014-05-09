name := "infinity-client"

description := "Infinity Client"

libraryDependencies ++= Dependencies.unfiltered ++ Seq(
  Dependencies.commonsLogging % "provided, test, it",
  Dependencies.dispatch,
  Dependencies.scalaStm % "test"
)
