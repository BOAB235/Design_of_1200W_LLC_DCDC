import os
import shutil

for root, dirs, files in os.walk(".", topdown=False):
    for d in dirs:
        if d == ".ipynb_checkpoints":
            folder_path = os.path.join(root, d)
            print(f"Removing: {folder_path}")
            shutil.rmtree(folder_path, ignore_errors=True)

print("Cleanup complete!")
