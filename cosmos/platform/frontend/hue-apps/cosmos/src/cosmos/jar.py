# -*- coding: utf-8 -*-
"""
JAR manipulation module.
"""
import re
from zipfile import BadZipfile, ZipFile

from django import forms
from lxml import etree

from cosmos.jar_parameters import make_parameter


PARAMETERS_TEMPLATE_KEY = 'Cosmos-Parameters-Template'


class InvalidJarFile(Exception):
    pass


def field_name(index):
    """HTML-safe name for a form field."""
    return 'param%d' % index


class ParameterTemplate(object):
    """
    Represents the parametrization template of a job.
    """

    def __init__(self):
        self.parameters = []

    def add(self, new_param):
        """
        Adds a parameter to the template unless its name collides with a
        previous one.
        """
        for param in self.parameters:
            if param.name == new_param.name:
                raise InvalidJarFile("Duplicated key '%s'" % param.name)
        self.parameters.append(new_param)

    def as_form(self, expansion):
        """
        Create a dynamic whose fields are dynamically generated from the
        template.
        """
        form = forms.Form(self.__form_data())
        for (index, param) in enumerate(self.parameters):
            form.fields[field_name(index)] = param.form_field(expansion)
        return form

    def update_from_form(self, form):
        """
        Updates parameter values from a form.

        Field names are assumed to be of the form "param<i>" where <i> is the
        zero-based parameter index.
        """
        for index, param in enumerate(self.parameters):
            param.set_value(form.get(field_name(index), None))

    def as_hadoop_args(self, job, expansion):
        """
        Generates the command line arguments to pass template values to the
        Hadoop job.
        """
        args = []
        for param in self.parameters:
            args.extend(param.as_job_argument(job, expansion))
        return args

    def __form_data(self):
        if not any(param.has_value() for param in self.parameters):
            return
        return dict([(field_name(index), param.get_value())
                     for (index, param) in enumerate(self.parameters)])


class JarFile(object):
    """
    Represents a Java JAR file
    """

    def __init__(self, path):
        self.path = path
        self.manifest = None
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
        """
        Builds a ParameterTemplate from the JAR metadata.
        """
        content = self.__parameters_template_content()
        path = self.__parameters_path()
        if path.endswith('.properties'):
            pairs = self.__load_from_properties(content)
        elif path.endswith('.xml'):
            pairs = self.__load_from_xml(content)
        else:
            raise InvalidJarFile("Unsupported properties template format")
        try:
            template = ParameterTemplate()
            for key, value in pairs:
                template.add(make_parameter(key, value))
            return template
        except ValueError, ex:
            raise InvalidJarFile("Invalid parameter description: %s" % ex)

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
        return [(k, v) for k, v in re.findall('([^=\n\r]+)=(.*)', content)]

    def __load_from_xml(self, content):
        """
        Parses parameters from a Java properties file in XML format
        """
        return [(entry.get('key'), entry.text) for entry in
                etree.fromstring(content)]
