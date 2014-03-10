name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.spray ++ Dependencies.akka

seq(Revolver.settings: _*)

