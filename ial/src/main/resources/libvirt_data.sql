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
        'andromeda02',
        'Andromeda 02',
        1,
        'andromeda52.hi.inet',
        'openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        1,
        '192.168.63.62'
    ),
    (
        'andromeda03',
        'Andromeda 03',
        1,
        'andromeda53.hi.inet',
        'openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        1,
        '192.168.63.63'
    ),
    (
        'andromeda04',
        'Andromeda 04',
        1,
        'andromeda54.hi.inet',
        'openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock',
        3,
        1,
        '192.168.63.64'
    ),
    (
        'andromeda05',
        'Andromeda 05',
        1,
        'andromeda55.hi.inet',
        'openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock',
        4,
        1,
        '192.168.63.65'
    ),
    (
        'andromeda06',
        'Andromeda 06',
        1,
        'andromeda56.hi.inet',
        'openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock',
        5,
        1,
        '192.168.63.66'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
