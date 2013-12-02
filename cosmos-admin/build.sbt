name := "cosmos-admin"

mainClass := Some("es.tid.cosmos.admin.Main")

libraryDependencies ++= Seq(
  "com.sandinh" % "play-jdbc-standalone_2.10" % "2.0.1_2.2",
  "com.typesafe" % "config" % "1.0.0",
  "org.rogach" %% "scallop" % "0.9.4",
   Dependencies.logbackClassic
)
