sourceDirectory in IntegrationTest <<= baseDirectory / "it"

libraryDependencies += "play" %% "play-test" % play.core.PlayVersion.current % "it"