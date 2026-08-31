# Instanciation FT-STR-04

La validation active est dérivée directement de GEL-MAP-V1.

Les huit champs `ASCII fixe` sont recensés dans `COUVERTURE_FT-STR-04_GEL-MAP-V1.csv`. Le validateur `validate_ft_str_04_ascii.py` applique les invariants structurels de `TT-STR-04-GEN-001`.

Les contrôles d’ordre d’octets et de padding définis par GEN-002 et GEN-003 nécessitent une valeur d’essai connue et doivent être exécutés sur simulation, banc ou firmware.

Les anciennes fiches champ-par-champ sont historiques et ont été déplacées dans `archive_pre_renforcement/instancie_legacy/`.
