===================================
Big Data Platform (Codename Cosmos)
===================================

Executive summary
-----------------

The Big Data Platform is a product that allows you to:

  - create and release clusters of different sizes
  - create and monitor jobs
  - set up external storage from Telefonica's cloud solution
  - connect to get data in and out via SFTP, streaming, and landing area
  - make sure their cluster is not out of storage space, including the
    definition of storage quota
  - allow the user to RDP into the head node for any additional advanced
    functionality, except if this compromises cluster performance
  - provide fast, scalable consumption APIs
  - support for productized analytical models and base ML techniques

What is this document?
----------------------
This document frames the goals and scope for the current Cosmos release. Documentation for each feature should be placed in a README file on that feature's folder.

=================================
Cosmos CTP (internal alpha) goals
=================================
**Due Date**: 30/03/2012

**One line goal**: Create a web front-end to let users launch Hadoop jobs on the cluster and view the results of the job.

*Note*: The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.


Web front-end
-------------

Goals
~~~~~
- Main Page:
   - The front-end MUST support user login.
   - The main page of the front-end (once the user is logged in) MUST provide a list of previous and active Hadoop tasks, and their state: Created, Running, Completed, Error.
   - The rows that describe Completed tasks MUST provide a link to view the results of the job.
   - The rows that describe Error tasks MUST provide a link to view extended error information about the failure.
   - The rows that describe Created tasks MUST provide a link to launch the job.
   - The main page MUST provide a button to create new Hadoop jobs
- Job creation page (unless otherwise specified, users are assumed to be authenticated):
   - Non authenticated users MUST NOT be able to create Hadoop jobs in the cluster.
   - Users MUST be able to upload the input data through an HTTP POST (i.e. a Web form).
   - Users MUST be able to upload a JAR that contains the logic of the job to be run.
   - The job creation page MUST provide a link to a project that builds a sample JAR that works on the cluster.
   - The job creation page MUST list the restrictions that the JAR must follow in order to work on the cluster.
   - Jobs MUST appear in the main page in the "Created" state once thay have been created.
- Job results page:
   - Users MUST be able to see the result of their "Completed" jobs on a paged table.
  
Non-goals
~~~~~~~~~
- User management: User creation through the web front-end. *Workaround*: users must contact cosmos@tid.es to request an account
- Data management: users won't be able to delete their JARs, input data or output. *Workaround*: the team will administer the cluster and remove old data whenever disk space runs low.
- JAR configuration: the uploaded JARs will not be able to receive configuration parameters.
- JAR validation: Uploaded JAR files have full access to the cluster and are assumed non-malicious.

Backend
-------

Goals
~~~~~
- The system MUST support having more than one job in the Running state, even if these jobs are created by the same user, are using the same JAR and even use the same input data.
 - The output results MUST be stored in a MongoDB database where the DB name MUST be the username and the collection MUST match the model name that has been run, together with a jobid to be able to store results from different runs. Example: dmicol.profile_32

JAR requirements
----------------
The JAR file that is uploaded through the web front-end must follow the following rules to be successfully run in the cluster. These rules MAY NOT be explicitly checked in the cluster.

Submitting a JAR file which does not follow these rules will result in unspecified behavior.

Rules:

- JARs MUST contain a manifest that describes the main class.
- The main class MUST implement Hadoop's Tool interface.
- The JAR MUST NOT take any configuration parameters.
- The InputFormat used by the JAR MUST use an HDFS file as an input (e.g. it cannot be a database connection).
- The output data MUST be written to a MongoDB instance (e.g. primitives, BSON objects).

Other non-goals
---------------
- Submitting input data through SFTP or SCP
- Use of existing models through the web front-end (unless they support all the restrictions under the "JAR requirements" section)

==========================================================
Open Issues (this section must be empty prior to signoff!)
==========================================================
- Deployment:
    - VDC or cluster in Barcelona?
    - Machine roles
