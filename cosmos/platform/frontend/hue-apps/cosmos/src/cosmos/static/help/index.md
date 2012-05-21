Cosmos Shell App
================

Cosmos Shell App enables you to upload and run Hadoop jobs.

Custom mapreduce JAR requirements
---------------------------------

Upon writing your own customized JAR file, there are a few things you need to
take into consideration:

- Your code must have a `main` method that will control the execution flow.
- Your program may implement the `Tool` interface.
- The main method of your program will be provided with three arguments: input
  and output files, and MongoDB URL where to export the results.
- You should use an API that is compatible with Hadoop’s 0.20.2 version. We
  currently support both mapred and mapreduce interfaces.
- In order to export to MongoDB, the mongo-hadoop plug-in version 1.0 shall be
  used.
