name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.spray ++ Dependencies.akka ++ Seq(
  Dependencies.commonsCodec,
  Dependencies.logbackClassic
)

seq(Revolver.settings: _*)
