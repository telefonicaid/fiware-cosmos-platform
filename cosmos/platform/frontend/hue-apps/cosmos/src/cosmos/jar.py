# -*- coding: utf-8 -*-
"""
JAR manipulation module.
"""
import re
from zipfile import BadZipfile, ZipFile

from lxml import etree


PARAMETERS_TEMPLATE_KEY = 'Cosmos-Parameters-Template'


class InvalidJarFile(Exception):
    pass


class ParameterTemplate(object):

    def __init__(self, key, value):
        if key.find('=') >= 0:
            raise InvalidJarFile("Invalid parameter name '%s'" % key)
        self.name = key
        match = re.match(r'^(string|filepath)(?:\|(.*))?$', value)
        if not match:
            raise InvalidJarFile('Invalid parameter description "%s"' % value)
        (self.type, self.default_value) = match.groups()


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

    def close(self):
        self.jar.close()

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

    def is_parameterized(self):
        return self.manifest().has_key(PARAMETERS_TEMPLATE_KEY)

    def parameters(self):
        content = self.__parameters_template_content()
        path = self.__parameters_path()
        if path.endswith('.properties'):
            return self.__load_from_properties(content)
        elif path.endswith('.xml'):
            return self.__load_from_xml(content)
        else:
            raise InvalidJar("Unsupported properties template format")

    def __parameters_path(self):
        path = self.manifest().get(PARAMETERS_TEMPLATE_KEY)
        if path.startswith('/'):
            path = path[1:]
        return path

    def __parameters_template_content(self):
        if not self.is_parameterized():
            raise ValueError('Not parameterized JAR')
        path = self.__parameters_path()
        try:
            return self.jar.read(path)
        except KeyError:
            raise InvalidJarFile('Parameters template "%s" not found"' % path)

    def __load_from_properties(self, content):
        """
        Parses parameters from a Java properties file
        """
        return [ParameterTemplate(k, v) for k, v in
                re.findall('([^=\n\r]+)=(.*)', content)]

    def __load_from_xml(self, content):
        """
        Parses parameters from a Java properties file in XML format
        """
        return [ParameterTemplate(entry.get('key'), entry.text)
                for entry in etree.fromstring(content)]
