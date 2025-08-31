import os
import glob

# Get the directory where this script is located
base_dir = os.path.dirname(os.path.abspath(__file__))

# Patterns to search for
patterns = ["**/*.raw", "**/*.log" , "**/*.fft"]

for pattern in patterns:
    files = glob.glob(os.path.join(base_dir, pattern), recursive=True)
    
    for file_path in files:
        try:
            os.remove(file_path)
            print(f"Deleted: {file_path}")
        except PermissionError:
            print(f"Permission denied: {file_path}")
        except Exception as e:
            print(f"Error deleting {file_path}: {e}")




import os
import glob
import shutil
import stat

def remove_readonly(func, path, excinfo):
    # Change the file to writable and try again
    os.chmod(path, stat.S_IWRITE)
    func(path)

# Delete folders
folders_to_delete = [".ipynb_checkpoints", ".virtual_documents"]
for folder_name in folders_to_delete:
    for folder_path in glob.glob(os.path.join(base_dir, "**", folder_name), recursive=True):
        try:
            shutil.rmtree(folder_path, onerror=remove_readonly)
            print(f"Deleted folder: {folder_path}")
        except Exception as e:
            print(f"Error deleting {folder_path}: {e}")

