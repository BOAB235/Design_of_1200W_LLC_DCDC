import os
import glob
from PIL import Image

project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'
os.chdir(project_dir)

print("="*100)
print("SCANNING ALL IMAGES FOR GRAYSCALE")
print("="*100)

# Find ALL images (same extensions as the problematic script)
print("\nSearching for .jpg, .jpeg, .png files recursively...")

image_paths = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

print(f"Found {len(image_paths)} total images")
print("\nChecking each image mode...\n")

grayscale_images = []
color_images = []
errors = []

for i, img_path in enumerate(image_paths):
    try:
        img = Image.open(img_path)
        
        if img.mode == 'L':  # Grayscale
            grayscale_images.append((img_path, img.mode, img.size))
            print(f"GRAY: {img_path}")
        else:
            color_images.append(img_path)
        
        # Progress indicator every 100 images
        if (i + 1) % 100 == 0:
            print(f"  ...checked {i+1}/{len(image_paths)} images")
            
    except Exception as e:
        errors.append((img_path, str(e)))
        print(f"ERROR: {img_path} - {e}")

print("\n" + "="*100)
print("RESULTS")
print("="*100)
print(f"\nTotal images scanned: {len(image_paths)}")
print(f"✓ Color images: {len(color_images)}")
print(f"✗ Grayscale images: {len(grayscale_images)}")
print(f"⚠ Errors: {len(errors)}")

# Save results to file
output_file = "grayscale_images_list.txt"
with open(output_file, 'w', encoding='utf-8') as f:
    f.write("="*100 + "\n")
    f.write("GRAYSCALE IMAGES FOUND\n")
    f.write(f"Scan date: {os.popen('date /t').read().strip()} {os.popen('time /t').read().strip()}\n")
    f.write("="*100 + "\n\n")
    
    f.write(f"Total images scanned: {len(image_paths)}\n")
    f.write(f"Color images: {len(color_images)}\n")
    f.write(f"Grayscale images: {len(grayscale_images)}\n")
    f.write(f"Errors: {len(errors)}\n")
    f.write("\n" + "="*100 + "\n\n")
    
    if grayscale_images:
        f.write("GRAYSCALE IMAGE PATHS:\n")
        f.write("-"*100 + "\n\n")
        for i, (path, mode, size) in enumerate(grayscale_images, 1):
            f.write(f"{i}. {path}\n")
            f.write(f"   Mode: {mode}, Size: {size}\n\n")
    else:
        f.write("✓✓✓ NO GRAYSCALE IMAGES FOUND - ALL IMAGES ARE IN COLOR! ✓✓✓\n\n")
    
    if errors:
        f.write("\n" + "="*100 + "\n")
        f.write("ERRORS:\n")
        f.write("-"*100 + "\n\n")
        for path, err in errors:
            f.write(f"- {path}\n  Error: {err}\n\n")

print(f"\n✓ Results saved to: {output_file}")

if grayscale_images:
    print("\n⚠⚠⚠ FOUND GRAYSCALE IMAGES ⚠⚠⚠")
    print(f"\nTotal: {len(grayscale_images)} grayscale images")
    print("\nFirst 20 grayscale images:")
    for i, (path, mode, size) in enumerate(grayscale_images[:20], 1):
        print(f"  {i}. {path}")
    if len(grayscale_images) > 20:
        print(f"  ... and {len(grayscale_images)-20} more")
    print(f"\nSee full list in: {output_file}")
else:
    print("\n✓✓✓ ALL IMAGES ARE IN COLOR! ✓✓✓")
    print("\nNo grayscale images found.")

print("="*100)
