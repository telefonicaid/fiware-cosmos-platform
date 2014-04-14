// Comment to get more information during initialization
logLevel := Level.Warn

resolvers += "Cosmos Nexus Repository" at "http://cosmos10.hi.inet/nexus/content/groups/public/"

addSbtPlugin("com.typesafe.play" % "sbt-plugin" % "2.2.2" exclude(
    "com.typesafe.sbt", "sbt-native-packager"
  ))

addSbtPlugin("com.eed3si9n" % "sbt-assembly" % "0.10.2")

/**
 * RPM building on non-rpm based systems (ie. OSX).
 */
libraryDependencies += ("com.typesafe.sbt" % "sbt-native-packager" % "0.6.4-cosmos1"
  force() from "http://cosmos10.hi.inet/nexus/content/groups/public/com/typesafe/sbt/sbt-native-packager/0.6.4-cosmos1/sbt-native-packager-0.6.4-cosmos1.jar")

libraryDependencies += "org.apache.commons" % "commons-compress" % "1.4.1"

addSbtPlugin("com.sqality.scct" % "sbt-scct" % "0.3")

addSbtPlugin("io.spray" % "sbt-revolver" % "0.7.1")

