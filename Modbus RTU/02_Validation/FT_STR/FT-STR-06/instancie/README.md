# FT-STR-06 — Validation instanciée GEL-MAP-V1

L'instanciation active est volontairement compacte et orientée **plages d'adresses**, pas champs logiques.

Elle couvre :
- les 8 plages exposées des blocs 0 à 7 ;
- la segmentation des plages dépassant 125 registres ;
- les 7 lacunes inter-blocs comme frontières d'adressage invalide.

Les 183 anciennes fiches champ par champ sont conservées dans `archive_pre_renforcement/instancie_legacy/` et ne participent plus à la validation active.
