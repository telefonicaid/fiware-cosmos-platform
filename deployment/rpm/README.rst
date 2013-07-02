====================================
How to create the Cosmos RPM package
====================================

In order to create the Cosmos RPM, the following commands need to be run:

- From the root directory:

  - `mvn clean install`
  - `sbt "set retrieveManaged in ThisBuild := true" clean test package`
  - `sbt "project cosmos-api" dist`

- From the `rpm` directory:

  - `mvn clean install`
