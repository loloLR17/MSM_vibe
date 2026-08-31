# Instanciation active FT-STR-03

La validation active de FT-STR-03 est dérivée directement de GEL-MAP-V1.

Elle ne duplique pas chaque champ `uint32` dans une fiche Markdown individuelle. Le mapping gelé constitue la table d'instanciation et `validate_ft_str_03_multireg.py` applique les invariants structurels d'encodage à tous les `uint32`.

Les anciennes fiches champ-par-champ ont été déplacées dans `archive_pre_renforcement/instancie_legacy/` et ne participent plus à la validation active.

Les essais de reconstruction génériques utilisent les vecteurs asymétriques définis par `TT-STR-03-GEN-002.md`.
