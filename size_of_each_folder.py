import os

def get_folder_size(folder):
    total_size = 0
    for dirpath, _, filenames in os.walk(folder):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            if os.path.isfile(fp):
                total_size += os.path.getsize(fp)
    return total_size

def list_folders_with_size():
    txt=""
    current_dir = os.getcwd()  # Get current directory
    for folder in os.listdir(current_dir):
        folder_path = os.path.join(current_dir, folder)
        if os.path.isdir(folder_path):  # Only process directories
            size_gb = get_folder_size(folder_path) / (1024 ** 3)  # Convert bytes to GB
            print(f"{folder} - {size_gb:.2f} GB")
            txt+=f"{folder} - {size_gb:.2f} GB\n"
    return txt

if __name__ == "__main__":
    txt=list_folders_with_size()
    with open("size_of_each_folder.txt","w") as f:
        f.write(txt)
