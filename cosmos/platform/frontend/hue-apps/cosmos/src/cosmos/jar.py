# -*- coding: utf-8 -*-
"""
JAR manipulation module.

"""
import re
from zipfile import BadZipfile, ZipFile


class InvalidJarFile(Exception):
    pass

class JarFile(object):
    """
    Represents a Java JAR file
    """

    def __init__(self, path):
        self.path = path
        try:
            self.jar = ZipFile(path, 'r')
        except BadZipfile:
            raise InvalidJarFile('Not a valid ZIP')

    def manifest(self):
        """
        Returns the manifest as a dict
        """
        try:
            content = self.jar.read('META-INF/MANIFEST.MF')
        except KeyError:
            raise InvalidJarFile('MANIFEST.MF not found')

        # Long lines are folded and the next line starts by an space
        # so we remove that pattern
        unfolded_content = re.sub(r'(?:\r|\n)+ ', '', content)

        manifest_dict = {}
        for line in re.split(r'(?:\r|\n)+', unfolded_content):
            match = re.match(r'^([^:]+):\s+(.*)$', line)
            if match:
                key, value = match.groups()
                manifest_dict[key] = value
            elif len(line.strip()) > 0:
                raise InvalidJarFile('Invalid line in MANIFEST.MF: "%s"' % line)

        return manifest_dict

    def close(self):
        self.jar.close()
