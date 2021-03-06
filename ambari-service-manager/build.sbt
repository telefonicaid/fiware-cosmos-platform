name := "ambari-service-manager"

unmanagedClasspath in IntegrationTest <+= (classDirectory in Test) map { t => Attributed.blank(t) }

compile in IntegrationTest <<= compile in IntegrationTest dependsOn (compile in Test)

libraryDependencies ++= Seq(
  "com.jcraft" % "jsch" % "0.1.49",
  "com.imageworks.scala-migrations" %% "scala-migrations" % "1.1.1",
  "mysql" % "mysql-connector-java" % "5.1.10",
  Dependencies.dispatch,
  Dependencies.liftJson,
  Dependencies.logbackClassic,
  Dependencies.scalaLogging,
  Dependencies.squeryl,
  Dependencies.typesafeConfig
)

unmanagedResourceDirectories in IntegrationTest <+= ExternalSources.servicesConfigDirectory
