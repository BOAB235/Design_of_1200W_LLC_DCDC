import os
import glob
from PIL import Image
import numpy as np

# Input and output directories
#input_dir = r'Photos'
output_dir = r'Images_no_Meta'

# Create output directory if it doesn't exist
os.makedirs(output_dir, exist_ok=True)

# Get all JPG and PNG files
# image_paths = glob.glob(os.path.join(input_dir, '*.jpg')) + glob.glob(os.path.join(input_dir, '*.png'))


# Find all JPG/JPEG/PNG recursively in the current directory
image_paths  = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

arr_nok=[]
# Process each image
for img_path in image_paths:
    try: 
        # Open image
        img = Image.open(img_path)
        
        # Convert to numpy array
        arr = np.array(img)
        
        # Recreate image from array
        img_no_meta = Image.fromarray(arr)
        
        # Build output path
        base_name = os.path.basename(img_path)
        #out_path = os.path.join(output_dir, base_name)
        buckup_path = output_dir+'\\'+img_path


        #break 
        # Save image — no metadata included
        
        # CREATE FODER IN BUCKUP DIRECTORY 
        os.makedirs(os.path.dirname(buckup_path), exist_ok=True)  # <-- ensure folders exist

        
        img.save(buckup_path)
        img_no_meta.save(img_path)
        print("DONE", img_path)
    except :
        arr_nok.append(img_path)
        print("NOKK", img_path)
print("="*100)
print(f"Processed {len(image_paths)} images. All saved in '{output_dir}'.")
print("="*100)
for f in arr_nok:
    print("NOK", f)
