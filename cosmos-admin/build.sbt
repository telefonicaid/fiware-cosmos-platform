name := "cosmos-admin"

mainClass := Some("es.tid.cosmos.admin.Main")

libraryDependencies ++= Seq(
  "com.typesafe" % "config" % "1.0.0",
  "org.rogach" %% "scallop" % "0.9.4",
   Dependencies.logbackClassic
)
