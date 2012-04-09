"""
Utilities for handling file uploads.

"""

from os import makedirs, path

def save(upload, target_dir, target_name):
    """
    Saves an upload to a given dir renaming it to target_name.

    """
    ensure_dir(target_dir)
    target_file = path.join(target_dir, target_name)
    with open(target_file, 'w') as dest:
        for chunk in upload.chunks():
            dest.write(chunk)

def ensure_dir(dir_path):
    """Creates the dir when not exists"""
    if not path.exists(dir_path):
        makedirs(dir_path)
