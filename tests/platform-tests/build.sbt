name := "platform-tests"

parallelExecution in Test := false

libraryDependencies ++= Seq(
  Dependencies.dispatch,
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.typesafeConfig
)
