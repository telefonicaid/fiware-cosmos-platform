name := "infinityfs"

description := "Infinity HFS driver"

libraryDependencies ++= Seq(
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided"
)
