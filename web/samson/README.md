Samson WEB monitoring Interface
===============================

This README.md explains the basic setup process for getting 

Requirements
------------

* Ubuntu 11.04 or Redhat/CentOS Linux 6.0
* Apache 2.x (1.3 would probably work)
* PHP 5.3 (earlier versions would probably work)
* SAMSON :)

Setup
-----

Make sure you have a working Apache/PHP environment. If you don't have either
packages installed use the following commands:

* Ubuntu
    apt-get install apache2 php5
* Redhat/CentOS
    yum install xxxxxx

To verify this place the following PHP code in a file under a directory that
the Apache web server has access. Typically this is /var/www (Ubuntu) or
/var/www/htdocs (RedHat/CentOS).

    <?php
        phpinfo();
    ?>

Save the above code to a file, say phpinfo.php, and then open up a browser to
the URL http://server/phpinfo.php (change server to the name of the machine you
are working on). If you see a page that tells you about the PHP environment and
the extensions that are installed you have a working installation. If not check
google for some trouble shooting guides. Once you have determined that Apache
and PHP are working ok, delete the phpinfo.php script as it is considered a
security risk to have that information available.

Instalation
-----------

Copy the files under web/samson to a directory that can be accessed by the web server. e.g. on Ubuntu:
    cp -r web/samson /var/www/samson


