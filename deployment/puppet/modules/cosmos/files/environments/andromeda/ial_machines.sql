--
-- Telefónica Digital - Product Development and Innovation
--
-- THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
-- EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
--
-- Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
-- All rights reserved.
--

--
-- Data for table `SERVERS`
--

LOCK TABLES `SERVERS` WRITE;
/*!40000 ALTER TABLE `SERVERS` DISABLE KEYS */;
INSERT INTO `SERVERS` VALUES
    (
        'andromeda02',
        4,
        'vzbr0',
        'Andromeda 02',
        1,
        'andromeda-compute02.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        1,
        32768,
        20,
        '192.168.63.76'
    ),
    (
        'andromeda03',
        4,
        'vzbr0',
        'Andromeda 03',
        1,
        'andromeda-compute03.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        1,
        32768,
        20,
        '192.168.63.77'
    ),
    (
        'andromeda04',
        4,
        'vzbr0',
        'Andromeda 04',
        1,
        'andromeda-compute04.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock',
        3,
        1,
        32768,
        20,
        '192.168.63.78'
    ),
    (
        'andromeda05',
        4,
        'vzbr0',
        'Andromeda 05',
        1,
        'andromeda-compute05.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock',
        4,
        1,
        32768,
        20,
        '192.168.63.79'
    ),
    (
        'andromeda06',
        4,
        'vzbr0',
        'Andromeda 06',
        1,
        'andromeda-compute06.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock',
        5,
        1,
        32768,
        20,
        '192.168.63.80'
    ),
    (
        'andromeda07',
        4,
        'vzbr0',
        'Andromeda 07',
        1,
        'andromeda-compute07.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.17/system?socket=/var/run/libvirt/libvirt-sock',
        6,
        1,
        32768,
        20,
        '192.168.63.81'
    ),
    (
        'andromeda08',
        4,
        'vzbr0',
        'Andromeda 08',
        1,
        'andromeda-infinity.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.18/system?socket=/var/run/libvirt/libvirt-sock',
        7,
        3,
        32768,
        20,
        '192.168.63.82'
    ),
    (
        'andromeda09',
        4,
        'vzbr0',
        'Andromeda 09',
        1,
        'andromeda-data99.hi.inet',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://root@192.168.63.33/system?socket=/var/run/libvirt/libvirt-sock',
        8,
        2,
        32768,
        20,
        '192.168.63.97'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
