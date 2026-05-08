import glob
import os

# Path to the directory you want to search
directory = "./"

# Use glob to find all .zip files
zip_files = glob.glob(os.path.join(directory, "*.zip"))

# Sort by modification time (newest first)
zip_files.sort(key=os.path.getmtime, reverse=True)

# Print the sorted list of zip files
for f in zip_files:
    print(f)







import zipfile
import os
import shutil





# Example zip file path
zip_path = zip_files[0] # replace with your path
new_name = zip_files[0].replace("LIB_", "").replace(".zip", "")#.split("\\")[1]

# Temporary folder to extract
temp_extract_folder = "./temp_extract"

# Ensure temp folder exists
os.makedirs(temp_extract_folder, exist_ok=True)

# Extract the zip file
with zipfile.ZipFile(zip_path, 'r') as zip_ref:
    zip_ref.extractall(temp_extract_folder)

# Search for a folder named "KiCad"
found = False
for root, dirs, files in os.walk(temp_extract_folder):
    if "KiCad" in dirs:
        kicad_path = os.path.join(root, "KiCad")
        # Copy it to current folder as "NEWcomp"
        shutil.copytree(kicad_path, new_name, dirs_exist_ok=True)
        print(f'"KiCad" folder found and copied as',new_name )
        found = True
        break

if not found:
    print('"KiCad" folder not found')

# Optional: cleanup temp folder
shutil.rmtree(temp_extract_folder)



_=input("click to go")