# FT-CMD-07 — Maintenance et commandes protégées

## Objet
Valider les règles transactionnelles des commandes 8 ENTER MAINTENANCE, 9 EXIT MAINTENANCE, 10 RESET SOFTWARE et 11 RESET STATISTICS.

## Principes
- seules les commandes 10 et 11 sont protégées ;
- la clé valide est `0xA55A` ; `0x0000` signifie absence de confirmation ;
- une commande protégée sans confirmation valide est refusée avec `cmd_result_code = 9` ;
- la recommandation d'accepter ENTER MAINTENANCE seulement acquisition arrêtée n'est pas normative ;
- RESET SOFTWARE exige acquisition arrêtée et absence d'opération critique non terminée ;
- RESET STATISTICS ne doit pas effacer campagnes, identité, configuration ni journaux critiques.

## Frontières
- effets de mode maintenance visibles ailleurs : FT-INT ;
- comportement après reboot et persistance : FT-PER ;
- non-effets inter-blocs de RESET STATISTICS déjà couverts par FT-INT ;
- accès/structure/domaines simples : familles dédiées.

## Tests
- `TT-CMD-B05-600` ENTER MAINTENANCE admissible ;
- `TT-CMD-B05-601` EXIT MAINTENANCE admissible ;
- `TT-CMD-B05-602` RESET SOFTWARE sans clé valide -> 9 ;
- `TT-CMD-B05-603` RESET SOFTWARE avec clé valide et préconditions satisfaites ;
- `TT-CMD-B05-604` RESET SOFTWARE acquisition en cours -> 5 ;
- `TT-CMD-B05-605` RESET SOFTWARE avec opération critique non terminée -> `CONDITIONAL`, refus attendu mais code exact `NOT_DEFINED` ;
- `TT-CMD-B05-606` RESET STATISTICS sans clé valide -> 9 ;
- `TT-CMD-B05-607` RESET STATISTICS avec clé valide ;
- `TT-CMD-B05-608` commandes 1..9 non soumises à la clé de confirmation.

## Dettes V1
- code résultat exact pour RESET SOFTWARE refusé pour opération critique non terminée non défini ;
- définition de « opération critique » non exhaustive ;
- portée exacte des statistiques remises à zéro non exhaustive ;
- politique d'entrée en maintenance acquisition arrêtée seulement recommandée.

## Statut
Reconstruite sur branche d'audit, en attente de validation avant merge.
