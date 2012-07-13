# -*- coding: utf-8 -*-
"""
Java properties file parser test.
"""
import unittest as test

from cosmos import properties

class ReadPropertiesTestCase(test.TestCase):

    def test_read_simple_properties(self):
        ps = properties.loads("""
                              key1=val1
                              key2=val with spaces
                              """)
        self.assertEquals(ps, [(u'key1', u'val1'),
                               (u'key2', u'val with spaces')])

    def test_ignore_comments_and_blank_lines(self):
        ps = properties.loads("""# This is a comment followed by an empty line

key1=val1
""")
        self.assertEquals(ps, [(u'key1', u'val1')])

    def test_escape_sequences(self):
        ps = properties.loads(r"""
                               key1=\t\"
                               key2=\=\'
                               key3=multi\nline
                               key4=\u00f1
                               """)
        self.assertEquals(ps, [(u'key1', u'\t"'),
                               (u'key2', u"='"),
                               (u'key3', u'multi\nline'),
                               (u'key4', u'\u00f1')])

    def test_wrong_escapes(self):
        self.assertRaises(properties.InvalidPropertiesError, properties.loads,
                          r"key = wrong unicode \uAAAZ")
        self.assertRaises(properties.InvalidPropertiesError, properties.loads,
                          r"key = wrong escape \g")

    def test_key_val_separation(self):
        ps = properties.loads("""
                              key1=equal sign
                              key2:colon
                              key3 = extra spaces
                              # Empty values:
                              key4 =
                              key5""")
        self.assertEquals(ps, [(u'key1', u'equal sign'),
                               (u'key2', u'colon'),
                               (u'key3', u'extra spaces'),
                               (u'key4', u''),
                               (u'key5', u'')])

    def test_key_val_separator_escaping(self):
        ps = properties.loads(r"""
                               ke\=y1 = val1
                               key\:2 : val2
                               key3\: = val3\=\:
                               """)
        self.assertEquals(ps, [(u'ke=y1', u'val1'),
                               (u'key:2', u'val2'),
                               (u'key3:', u'val3=:')])


    def test_logical_lines(self):
        ps = properties.loads(r"""
                               key1=one \
                                    pair
                               key2=two \\
                                    pairs
                               key3=one \\\
                                    pair
                               """)
        self.assertEquals(ps, [(u'key1', u'one pair'),
                               (u'key2', u'two \\'),
                               (u'pairs', u''),
                               (u'key3', u'one \\pair')])

    def test_first_unescaped(self):
        self.assertEquals(properties.first_unescaped(u'\\==', [u'=']), 2)
        self.assertEquals(properties.first_unescaped(u'\\=\\=', [u'=']), None)
