// Comment to get more information during initialization
logLevel := Level.Warn

resolvers += "Nexus" at "http://cosmos10.hi.inet/nexus/content/groups/public/"

resolvers += Classpaths.typesafeResolver

resolvers += "scct-github-repository" at "http://mtkopone.github.com/scct/maven-repo"

addSbtPlugin("reaktor" % "sbt-scct" % "0.2-SNAPSHOT")

addSbtPlugin("play" % "sbt-plugin" % "2.1.0")

addSbtPlugin("com.github.shivawu" % "sbt-maven-plugin" % "0.1.2")
