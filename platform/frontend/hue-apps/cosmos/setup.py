from setuptools import setup, find_packages
import os

def expand_package_data(src_dirs, strip=""):
  ret = []
  for src_dir in src_dirs:
    for path, dnames, fnames in os.walk(src_dir):
      for fname in fnames:
        ret.append(os.path.join(path, fname).replace(strip, ""))
  return ret

os.chdir(os.path.dirname(os.path.abspath(__file__)))
setup(
    name = "cosmos",
    version = "0.4",
    url = 'http://www.tid.es',
    description = 'Cosmos HUE application',
    author = 'Telefonica I+D S.A.U',
    author_email = 'cosmos@tid.es',
    packages = find_packages('src'),
    package_dir = {'': 'src'},
    install_requires = ['setuptools', 'desktop'],
    entry_points = { 'desktop.sdk.application': 'cosmos=cosmos' },
    zip_safe = False,
    include_package_data = True,
    package_data = {
        # Include static resources.  Package_data doesn't
        # deal well with directory globs, so we enumerate
        # the files manually.
        'cosmos': expand_package_data(
            ["src/cosmos/templates", "src/cosmos/static"],
            "src/cosmos/")
    }
)
