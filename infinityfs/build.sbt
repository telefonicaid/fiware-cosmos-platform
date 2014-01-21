import AssemblyKeys._

name := "Infinity HFS driver"

assemblySettings

jarName in assembly := "infinityfs.jar"

mainClass in assembly := Some("es.tid.cosmos.admin.Main")

libraryDependencies ++= Seq(
  Dependencies.hadoopCore % "provided"
)
