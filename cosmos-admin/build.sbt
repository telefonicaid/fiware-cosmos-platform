name := "cosmos-admin"

mainClass := Some("es.tid.cosmos.admin.Main")

libraryDependencies ++= Seq(
  "org.rogach" %% "scallop" % "0.9.4",
  Dependencies.logbackClassic,
  Dependencies.typesafeConfig
)
