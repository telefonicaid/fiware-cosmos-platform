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

Uploading data with SFTP
========================

From a graphic user interface
-----------------------------

We recommend using [FileZilla](http://filezilla-project.org/), a cross-platform 
FTP and SFTP client.

Configure the IP of the platform frontend, your HUE username and password and
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
built-in sftp client.

    $ sftp YOUR_USER@PLATFORM_FRONTEND

You will have to accept the RSA key if this is the first time that you are
connecting.

Enter the password that you use on the HUE frontend.

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
