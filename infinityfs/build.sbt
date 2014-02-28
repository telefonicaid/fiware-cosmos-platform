name := "infinityfs"

description := "Infinity HFS driver"

libraryDependencies ++= Seq(
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided"
)

/* TODO remove unce merged in develop */
publishTo := {
  val releaseType = if (version.value.trim.endsWith("SNAPSHOT")) "snapshots" else "releases"
  Some(releaseType  at "http://cosmos10.hi.inet/nexus/content/repositories/" + releaseType)
}

/* TODO remove unce merged in develop */
credentials += Credentials(Path.userHome / ".ivy2" / ".credentials")
