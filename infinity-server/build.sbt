name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.spray ++ Dependencies.akka ++ Seq(
  Dependencies.commonsCodec,
  Dependencies.liftJson,
  Dependencies.logbackClassic
)

seq(Revolver.settings: _*)
