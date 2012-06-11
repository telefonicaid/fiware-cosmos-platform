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
    PARAMETER_TYPES = ['string', 'filepath', 'mongocoll']

    def __init__(self, key, value):
        if key.find('=') >= 0:
            raise InvalidJarFile("Invalid parameter name '%s'" % key)
        self.name = key
        tokens = value.split('|', 1)
        self.type = tokens[0]
        if len(tokens) > 1:
            self.default_value = tokens[1]
        else:
            self.default_value = None
        if self.type not in self.PARAMETER_TYPES:
            raise InvalidJarFile('Invalid parameter description "%s"' % value)


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
        self.__load_manifest()

    def close(self):
        self.jar.close()

    def is_parameterized(self):
        return self.manifest.has_key(PARAMETERS_TEMPLATE_KEY)

    def parameters(self):
        content = self.__parameters_template_content()
        path = self.__parameters_path()
        if path.endswith('.properties'):
            return self.__load_from_properties(content)
        elif path.endswith('.xml'):
            return self.__load_from_xml(content)
        else:
            raise InvalidJar("Unsupported properties template format")

    def __load_manifest(self):
        """Returns the manifest as a dict"""
        try:
            content = self.__unfold_lines(
                    self.jar.read('META-INF/MANIFEST.MF'))
        except KeyError:
            raise InvalidJarFile('MANIFEST.MF not found')

        self.manifest = {}
        for line in content.splitlines():
            try:
                key, value = line.split(':', 1)
                self.manifest.setdefault(key, value.strip())
            except ValueError:
                if len(line.strip()) > 0:
                    raise InvalidJarFile('Invalid line in MANIFEST.MF: "%s"' %
                                         line)

    def __unfold_lines(self, content):
        """Unwrap MANIFEST.MF lines.
        
        MANIFEST.MF specification states that long lines are wrapped and the
        continuation line must start with an extra whitespace. We remove that
        pattern to join long lines back as a single line.
        """
        return re.sub(r'(?:\r|\n)+ ', '', content)

    def __parameters_path(self):
        path = self.manifest.get(PARAMETERS_TEMPLATE_KEY)
        if path.startswith('/'):
            path = path[1:]
        return path

    def __parameters_template_content(self):
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
