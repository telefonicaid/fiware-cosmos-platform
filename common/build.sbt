name := "Cosmos Platform Common"

libraryDependencies ++=  Seq(
  "org.squeryl" %% "squeryl" % "0.9.5-6",
  "mysql" % "mysql-connector-java" % "5.1.10",
  Dependencies.scalaz,
  Dependencies.typesafeConfig
)
