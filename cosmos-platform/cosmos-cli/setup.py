from setuptools import setup

setup(name='cosmos',
      version='0.1',
      author='Cosmos Team',
      author_email='cosmos@tid.es',
      packages=['cosmos'],
      entry_points={
          'console_scripts': ['cosmos=cosmos.main:run']},
      install_requires=[
          'pyyaml',
          'pymlconf',
          'requests',
      ]
      )
