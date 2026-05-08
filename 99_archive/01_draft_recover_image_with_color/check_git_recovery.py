import os
import subprocess
import glob
from PIL import Image

project_dir = r'C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC'
os.chdir(project_dir)

print("="*100)
print("CHECK: Are there grayscale images in git that we can restore?")
print("="*100)

# Find all current images in project
current_images = (
    glob.glob("**/*.jpg", recursive=True)
    + glob.glob("**/*.jpeg", recursive=True)
    + glob.glob("**/*.png", recursive=True)
)

# Filter to only those that are grayscale
grayscale_images = []
for img_path in current_images:
    try:
        img = Image.open(img_path)
        if img.mode == 'L':  # Grayscale
            grayscale_images.append(img_path)
    except:
        pass

print(f"\nFound {len(grayscale_images)} grayscale images in project")

if grayscale_images:
    print("\nChecking if these exist in git history (with color)...\n")
    
    recoverable_from_git = []
    
    for img_path in grayscale_images[:20]:  # Check first 20
        try:
            # Check if file is tracked by git and has history
            result = subprocess.run(
                ['git', 'log', '--oneline', '-1', '--', img_path],
                capture_output=True,
                text=True,
                timeout=2
            )
            
            if result.returncode == 0 and result.stdout.strip():
                recoverable_from_git.append(img_path)
                print(f"✓ In git: {img_path}")
        except:
            pass
    
    print(f"\n{len(recoverable_from_git)} grayscale images can be recovered from git")
    
    if recoverable_from_git:
        print("\nTo recover from git, run:")
        print("  git checkout HEAD -- <file_path>")
else:
    print("\n✓✓✓ NO grayscale images found! All images are in COLOR ✓✓✓")

print("="*100)
