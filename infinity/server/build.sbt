import AssemblyKeys._

name := "infinity-server"

description := "Infinity Server"

assemblySettings

excludedJars in assembly <<= (fullClasspath in assembly) map { cp =>
  cp filter { path => path.data.getName.contains("cglib-nodep-2.2.jar") }
}

jarName in assembly <<= (name, version) map { (name, version) => name + "-" + version + ".jar" }

libraryDependencies ++= Dependencies.unfiltered ++ Seq(
  Dependencies.anorm,
  Dependencies.commonsCodec,
  Dependencies.dispatch,
  Dependencies.finagleStream,
  Dependencies.finatra,
  Dependencies.h2database % "test",
  Dependencies.hadoopCommon % "provided",
  Dependencies.hadoopHdfs % "provided",
  Dependencies.liftJson,
  Dependencies.scalalikejdbc,
  Dependencies.scalaMigrations,
  Dependencies.scalaz,
  Dependencies.typesafeConfig
)
