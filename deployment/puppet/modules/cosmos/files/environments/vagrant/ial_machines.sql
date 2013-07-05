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
        'store1',
        4,
        'vzbr0',
        'Store 1',
        1,
        'cosmos-store1.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.21/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        3,
        32768,
        20,
        '192.168.11.21'
    ),
    (
        'store2',
        4,
        'vzbr0',
        'Store 2',
        1,
        'cosmos-store2.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.22/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        2,
        32768,
        20,
        '192.168.11.22'
    ),
    (
        'compute1',
        4,
        'vzbr0',
        'Compute 1',
        1,
        'cosmos-compute1.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.23/system?socket=/var/run/libvirt/libvirt-sock',
        3,
        1,
        32768,
        20,
        '192.168.11.23'
    ),
    (
        'compute2',
        4,
        'vzbr0',
        'Compute 2',
        1,
        'cosmos-compute2.vagrant',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.11.24/system?socket=/var/run/libvirt/libvirt-sock',
        4,
        1,
        32768,
        20,
        '192.168.11.24'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
