# Résultat FT-STR-05 — GEL-MAP-V1

## Références

- sous-famille : FT-STR-05 ;
- mapping : GEL-MAP-V1 ;
- gel mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca` ;
- gouvernance : GEL-GOV-01/02/03.

## Couverture

GEL-MAP-V1 contient 18 zones logiques explicitement réservées, représentant 70 registres :

- B0 : 1 zone / 1 registre ;
- B1 : 3 / 3 ;
- B2 : 2 / 2 ;
- B3 : 1 / 8 ;
- B4 : 8 / 46 ;
- B6 : 2 / 8 ;
- B7 : 1 / 2.

Le Bloc 5 ne comporte aucune zone réservée active dans GEL-MAP-V1.

## Résultat documentaire

**CONFORME** pour l'identification et la géométrie du mapping dérivé, sous réserve que l'exécution de `validate_ft_str_05_reserved.py` retourne 0 sur le worktree considéré.

L'ancien doublon de l'adresse 4017 (`reserved` / `reserved_4B_17`) n'est pas actif : GEL-MAP-V1 conserve uniquement `reserved_4B_0`.

L'ancien nom B0 `reserved` est remplacé par le nom gelé `reserved_0`.

## Sentinelles

La règle historique globale `ID = 0 → non renseigné` est **obsolète et inactive**.

Une valeur zéro ne reçoit une signification particulière que si V1 la définit explicitement pour le champ concerné. Toute autre interprétation est `NON DÉFINI / À ARBITRER`.

## Contrôle d'implémentation restant

La conformité documentaire ne prouve pas la valeur réellement renvoyée par le capteur. `TT-STR-05-GEN-002` doit être exécuté sur la cible afin de confirmer `0x0000` pour chaque registre réservé soumis à cette règle.

Les droits d'écriture sur réservés sont hors FT-STR-05 et relèvent de FT-ACC/GEL-GOV-02.

## Conclusion

FT-STR-05 est structurellement alignée sur V1 et GEL-MAP-V1. Le statut terrain reste conditionné à l'exécution des contrôles d'implémentation.
