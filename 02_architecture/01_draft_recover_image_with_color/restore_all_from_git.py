import os
import subprocess
import glob
from PIL import Image

project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'
os.chdir(project_dir)

print("="*100)
print("RECOVERING ALL GRAYSCALE IMAGES FROM GIT")
print("="*100)

# Find all grayscale images
current_images = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

grayscale_images = []
for img_path in current_images:
    try:
        img = Image.open(img_path)
        if img.mode == 'L':  # Grayscale
            grayscale_images.append(img_path)
    except:
        pass

print(f"\nFound {len(grayscale_images)} grayscale images")
print(f"Checking git history for each...\n")

recoverable = []
not_in_git = []
errors = []

for img_path in grayscale_images:
    try:
        # Check if file exists in git HEAD (last commit)
        result = subprocess.run(
            ['git', 'ls-tree', 'HEAD', '--', img_path],
            capture_output=True,
            text=True,
            timeout=2
        )
        
        if result.returncode == 0 and result.stdout.strip():
            recoverable.append(img_path)
        else:
            not_in_git.append(img_path)
            
    except Exception as e:
        errors.append((img_path, str(e)))

print(f"✓ Recoverable from git: {len(recoverable)}")
print(f"✗ Not in git (lost): {len(not_in_git)}")
print(f"⚠ Errors: {len(errors)}")

if recoverable:
    print("\n" + "="*100)
    confirm = input(f"Do you want to restore {len(recoverable)} images from git? (yes/no): ")
    
    if confirm.lower() in ['yes', 'y']:
        print("\nRestoring from git...")
        restored = 0
        failed = []
        
        for img_path in recoverable:
            try:
                result = subprocess.run(
                    ['git', 'checkout', 'HEAD', '--', img_path],
                    capture_output=True,
                    timeout=5
                )
                if result.returncode == 0:
                    restored += 1
                    if restored % 50 == 0:
                        print(f"  Restored {restored}/{len(recoverable)}...")
                else:
                    failed.append(img_path)
            except Exception as e:
                failed.append(img_path)
        
        print(f"\n✓ Restored: {restored}")
        print(f"✗ Failed: {len(failed)}")
        
        if failed[:5]:
            print("\nFirst 5 failures:")
            for f in failed[:5]:
                print(f"  - {f}")
    else:
        print("\nCancelled. No images restored.")

if not_in_git:
    print(f"\n⚠ WARNING: {len(not_in_git)} grayscale images NOT in git - cannot recover")
    print("\nFirst 10:")
    for f in not_in_git[:10]:
        print(f"  - {f}")

print("\n" + "="*100)
