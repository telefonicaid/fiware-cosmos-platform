import AssemblyKeys._

name := "infinity-server"

description := "Infinity Server"

assemblySettings

excludedJars in assembly <<= (fullClasspath in assembly) map { cp =>
  cp filter { path => path.data.getName.contains("cglib-nodep-2.2.jar") }
}

mergeStrategy in assembly <<= (mergeStrategy in assembly) { oldStrategy => {
  case "com/twitter/common/args/apt/cmdline.arg.info.txt.1" => MergeStrategy.first
  case other => oldStrategy(other)
}}

jarName in assembly <<= (name, version) map { (name, version) => name + "-" + version + ".jar" }

libraryDependencies ++= Dependencies.unfiltered ++ Seq(
  Dependencies.anorm,
  Dependencies.commonsCodec,
  Dependencies.dispatch,
  Dependencies.jettyWebapp % "it",
  Dependencies.h2database % "test",
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided",
  Dependencies.liftJson,
  Dependencies.scalalikejdbc,
  Dependencies.scalaMigrations,
  Dependencies.scalaz,
  Dependencies.typesafeConfig
)
