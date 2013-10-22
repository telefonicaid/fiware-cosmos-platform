// Comment to get more information during initialization
logLevel := Level.Warn

resolvers ++= Seq(
  "Cosmos Nexus Repository" at "http://cosmos10.hi.inet/nexus/content/groups/public/",
  "Sonatype OSS Snapshots" at "https://oss.sonatype.org/content/repositories/snapshots"
)

// SNAPSHOT version has fix of including test resources
addSbtPlugin("com.github.scct" % "sbt-scct" % "0.3-SNAPSHOT")

addSbtPlugin("com.typesafe.play" % "sbt-plugin" % "2.2.0")

addSbtPlugin("com.github.shivawu" % "sbt-maven-plugin" % "0.1.3-SNAPSHOT")
