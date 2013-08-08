==========
Cosmos API
==========

This module exposes Cosmos functionality as a REST API and offers a minimal web
application for managing some account details.

---------------
Management page
---------------

The web application is integrated with the TU|Id service as authentication
provider so if you plan to use Cosmos, you first need to get a TU account.  The
first time you access Cosmos you will be asked to authorize the service and you
should register you SSH public key.  Follow the on-screen instructions.

Once registered you will get your API key and API secret to configure the
client-side tools.

--------
REST API
--------

Despite Cosmos functionality is exposed as a RESTful API you don't need to
write your own client to use it, you can leverage the ``cosmos-cli`` module and
operate from your command line.  However, API details are roughly described
in `API.rst <API.rst>`_ just in case a tightly integrated solution is needed.

----------
Deployment
----------

To create a distributable zip ``cosmos-api/dist/cosmos-api-<version>.zip``::

    $ sbt
    $ project cosmos-api
    $ dist

Unzip the archive in ``/opt/pdi-cosmos`` and make the contained ``start`` script
executable.  Then you need to copy and edit ``application.conf.sample`` to
``/opt/pdi-cosmos/etc/cosmos-api.conf`` as follows:

- General settings::

    application.baseurl="http://<host address>"
    application.secret="<long random string different in each deployment>"
    db.default.host="<database host>"
    db.default.name="<database name>"
    db.default.user="<database user name>"
    db.default.pass="<cosmos user password>"

- To have a link to cosmos CLI: ``cli.url="http://host/cosmos.egg"``

- To get more debugging information in the browser: ``application.mode=dev``

- To create cosmos-api tables on service start: ``evolutionplugin=enabled``

- IAL settings: configure the keys starting with ``ial.`` as IAL component documentation specifies.

- Service manager settings: configure the keys starting with ``ambari.`` and
``hdfs.`` as Service Manager component documentation specifies.

Apart from this, logging is configured by creating a file named
``/opt/pdi-cosmos/etc/logback.conf`` with a configuration similar to the
following::

   <configuration>
             <appender name="STDOUT" class="ch.qos.logback.core.ConsoleAppender">
                   <encoder>
                           <pattern>%d{HH:mm:ss.SSS} [%thread] %-5level %logger{36} - %msg%n</pattern>
                   </encoder>
             </appender>
             <appender name="FILE" class="ch.qos.logback.core.FileAppender">
                     <file>/opt/pdi-cosmos/var/log/cosmos-api.log</file>
                     <encoder>
                             <pattern>%date %level [%thread] %logger{10} [%file:%line] %msg%n</pattern>
                     </encoder>
             </appender>
             <logger name="es.tid.cosmos" level="DEBUG">
                   <appender-ref ref="STDOUT" />
             </logger>
             <root level="INFO">
                   <appender-ref ref="FILE" />
             </root>
   </configuration>

The Cosmos API comes with an init.d script which can be found at
``scripts/cosmos-api`` and can be installed by copying the file to ``/etc/init.d``
and::

    chmod +x /etc/init.d/cosmos-api
    chkconfig --add /etc/init.d/cosmos-api
    chkconfig cosmos-api on

After that, the classical ``/etc/init.d/cosmos-api start|stop|status`` command
will be available.
