Cosmos Shell App
================

Cosmos Shell App enables you to upload and run Hadoop jobs.

Custom mapreduce JAR requirements
---------------------------------

Upon writing your own customized JAR file, there are a few things you need to
take into consideration for simple jobs:

- Your code must have a `main` method that will control the execution flow.
- Your program may implement the `Tool` interface.
- The main method of your program will be provided with three arguments: input
  and output files, and MongoDB URL where to export the results.
- You should use an API that is compatible with Hadoop's 0.20.2 version. We
  currently support both mapred and mapreduce interfaces.
- In order to export to MongoDB, the mongo-hadoop plug-in version 1.0 shall be
  used.

Job parametrization
-------------------

When the three parameter schema falls short, custom jobs can use the custom
parametrization mechanism.  To do so, some metadata must be included in the
job JAR in a Java properties file similar to the next example:

    mobility.max_surface=string|1.2
    mobility.foobar=string
    mobility.debug_path=filepath
    mongo.output.uri=mongocoll

Every property refers to a parametrized configuration setting. The key is
the name of the setting that will be available through the
`org.apache.hadoop.conf.Configuration` class.  The value specifies the type
and, optionally, the default setting value.  Supported types are:

 * `string`: any text string.
 * `filepath`: an absolute path on the platform HDFS.
 * `mongocoll`: a valid collection URL (only numbers, letters, dashes and
   underlines, not starting by number) is requested but the application will
   get the full mongo collection URL (e.g. `mongodb://host/db.col`).
   In the case of lacking a default value `db_${job.id}` is taken (see section
   on variable expansion below).

To enable this feature, the MANIFEST.MF should have a property
`Cosmos-Parameters-Template` pointing to the properties file within the JAR.
Note that this will disable the three command line arguments available in the
basic execution.

The new job wizard will prompt for values for the specified parameters by
means of a form.  The fields will behave depending on the type, `filepath`
fields would have a file chooser button, for instance.


Variable expansion for parameters
---------------------------------

Some basic form of variable expansion is supported for the value of job
parameters.  Variables are marked by a dollar and braces (`${var}`) and it is
not possible to nest them.

Supported variables are:

 * Job-related variables:
    * `job.id`: unique positive integer that identifies the job.  Useful for
      being safe on output directory naming.
    * `job.name`: non-unique name.
 * User-related variables:
    * `user.name`: login of the user launching the job.
    * `user.home`: home path in the HDFS tree. Useful for setting input/output
      paths.


Uploading data with SFTP
========================

From a graphic user interface
-----------------------------

We recommend using [FileZilla](http://filezilla-project.org/), a cross-platform
FTP and SFTP client.

Configure the IP of the platform front-end, your HUE username and password and
optionally port 22 in the Quickconnect bar of FileZilla. Press 'Quickconnect'.

Use the left window pane to browse your local files and the right window pane to
browse the remote files. You can drag and drop files between both.

If you choose to use FireFTP for Firefox, remember to set 'Security' equal to
SFTP in the 'Connection' tab of your new connection.

From command line
-----------------

### on a Windows system

on a Windows system your best bet is to install PuTTY and then use its
psftp command-line tool. Follow then the instructions for POSIX systems.

### on a POSIX-compliant system 

on a POSIX-compliant system such as Linux or Mac OS X, you can use the
built-in SFTP client.

    $ sftp YOUR_USER@PLATFORM_FRONTEND

You will have to accept the RSA key if this is the first time that you are
connecting.

Enter the password that you use on the HUE front-end.

You can list your home directory with the following command:

    sftp> ls
    imaginary_file.txt

Create a new directory with the following command:

    sftp> mkdir upload
    sftp> ls
    upload/

Review your local directory with the following command:

    sftp> lls
    sample.txt

Upload data to your newly created directory

    sftp> put sample.txt upload/

Download data from your remote home directory to your local directory:

    sftp> get imaginary_file.txt .
