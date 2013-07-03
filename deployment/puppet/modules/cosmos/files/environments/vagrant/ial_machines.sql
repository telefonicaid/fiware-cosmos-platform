/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

--
-- Data for table `SERVERS`
--

LOCK TABLES `SERVERS` WRITE;
/*!40000 ALTER TABLE `SERVERS` DISABLE KEYS */;
INSERT INTO `SERVERS` VALUES
    (
        'slave1',
        4,
        'vzbr0',
        'Slave 1',
        1,
        'cosmos-slave1.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.21/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        1,
        32768,
        20,
        '192.168.11.21'
    ),
    (
        'slave2',
        4,
        'vzbr0',
        'Slave 2',
        1,
        'cosmos-slave2.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.22/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        1,
        32768,
        20,
        '192.168.11.22'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
