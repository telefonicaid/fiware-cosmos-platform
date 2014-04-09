name := "infinity-server"

description := "Infinity Server"

libraryDependencies ++= Dependencies.akka ++ Dependencies.spray ++ Seq(
  Dependencies.anorm,
  Dependencies.commonsCodec,
  Dependencies.h2database % "test",
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided",
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.scalalikejdbc,
  Dependencies.scalaMigrations,
  Dependencies.scalaz
)

seq(Revolver.settings: _*)
