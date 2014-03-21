name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.akka ++ Dependencies.spray ++ Seq(
  Dependencies.anorm,
  Dependencies.commonsCodec,
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.scalalikejdbc,
  Dependencies.scalaMigrations,
  Dependencies.scalaz,
  Dependencies.h2database % "test"
)

seq(Revolver.settings: _*)
