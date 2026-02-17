_="""import os
import shutil

for root, dirs, files in os.walk(".", topdown=False):
    for d in dirs:
        if d == ".ipynb_checkpoints" or __pycache__:
            folder_path = os.path.join(root, d)
            print(f"Removing: {folder_path}")
            shutil.rmtree(folder_path, ignore_errors=True)

print("Cleanup complete!")

"""

import os
import shutil

TARGETS = {".ipynb_checkpoints", "__pycache__"}

for root, dirs, files in os.walk(".", topdown=False):
    for d in dirs:
        if d in TARGETS:
            folder_path = os.path.join(root, d)
            print(f"Removing: {folder_path}")
            shutil.rmtree(folder_path, ignore_errors=True)

print("Cleanup complete!")
