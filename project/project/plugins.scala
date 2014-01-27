import sbt._
import Keys._

object PluginBuild extends Build {
  override def projects = Seq(root)

  val root = Project("root", file(".")) settings(
    resolvers += Resolver.url("scalasbt", new URL("http://scalasbt.artifactoryonline.com/scalasbt/sbt-plugin-releases"))(Resolver.ivyStylePatterns),
    libraryDependencies += "net.databinder.dispatch" %% "dispatch-core" % "0.11.0"
  )
}
