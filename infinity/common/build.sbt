name := "infinity-common"

description := "Infinity Common"

libraryDependencies ++= Seq(
  Dependencies.commonsLogging % "provided",
  Dependencies.hadoopCommon % "provided",
  Dependencies.liftJson
)
