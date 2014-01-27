// Comment to get more information during initialization
logLevel := Level.Warn

resolvers += "Cosmos Nexus Repository" at "http://cosmos10.hi.inet/nexus/content/groups/public/"

addSbtPlugin("com.typesafe.play" % "sbt-plugin" % "2.2.1")

addSbtPlugin("com.eed3si9n" % "sbt-assembly" % "0.10.2")

/*
 * Taken from SNAPSHOT as it has fix of including test resources.
 * TODO: Replace with released version once available
 */
addSbtPlugin("com.github.scct" % "sbt-scct" % "0.3-Cosmos")

/*
 * Taken from SNAPSHOT fixed and recompiled to work with Scala 2.10.2.
 * TODO: Replace with released version once available
 */
addSbtPlugin("com.github.shivawu" % "sbt-maven-plugin" % "0.1.3-Cosmos")
