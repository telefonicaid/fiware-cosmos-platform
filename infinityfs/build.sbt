name := "infinityfs"

description := "Infinity HFS driver"

libraryDependencies ++= Seq(
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided"
)

publishTo := {
  val releaseType = if (version.value.trim.endsWith("SNAPSHOT")) "snapshots" else "releases"
  Some(releaseType  at "http://cosmos10.hi.inet/nexus/content/repositories/" + releaseType)
}

credentials += Credentials(Path.userHome / ".ivy2" / ".credentials")
