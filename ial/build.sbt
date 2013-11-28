name := "Cosmos IAL"

resolvers ++= Seq(
    "libvirt-org" at "http://libvirt.org/maven2",
    Resolver.url("Play", url("http://download.playframework.org/ivy-releases/"))(Resolver.ivyStylePatterns)
)

libraryDependencies ++=  Seq(
  "net.java.dev.jna" % "jna" % "3.4.0",
  "net.java.dev.jna" % "platform" % "3.4.0",
  "org.libvirt" % "libvirt" % "0.4.9",
  "mysql" % "mysql-connector-java" % "5.1.10",
  Dependencies.logbackClassic,
  Dependencies.scalaLogging,
  Dependencies.squeryl,
  Dependencies.typesafeConfig
)
