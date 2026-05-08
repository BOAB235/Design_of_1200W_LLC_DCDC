import os

def add_gitkeep_to_empty_dirs(root='.'):
    for dirpath, dirnames, filenames in os.walk(root):
        # Si aucun fichier dans le dossier (on ignore sous-dossiers)
        if not filenames:
            gitkeep_path = os.path.join(dirpath, '.gitkeep')
            if not os.path.exists(gitkeep_path):
                with open(gitkeep_path, 'w') as f:
                    pass  # Crée un fichier vide .gitkeep
                print(f"Créé : {gitkeep_path}")

if __name__ == "__main__":
    add_gitkeep_to_empty_dirs()
