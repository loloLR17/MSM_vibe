# FT-CMD-07 — Validation détaillée

## TT-CMD-B05-600 — ENTER MAINTENANCE admissible
Soumettre la commande 8 avec un `transaction_id` frais dans un contexte où aucune autre règle normative ne l'interdit.

Oracle FT-CMD : la commande doit pouvoir être acceptée. Ne pas imposer comme précondition normative que l'acquisition soit arrêtée.

## TT-CMD-B05-601 — EXIT MAINTENANCE admissible
Soumettre la commande 9 avec un `transaction_id` frais.

Oracle : la commande doit pouvoir être traitée selon le moteur Bloc 5. Les effets visibles du mode maintenance restent FT-INT.

## TT-CMD-B05-602 — RESET SOFTWARE sans confirmation valide
Préparer la commande 10 avec `confirm_key = 0x0000` ou sans clé valide, et des préconditions par ailleurs admissibles.

Oracle : refus avec `cmd_result_code = 9`.

## TT-CMD-B05-603 — RESET SOFTWARE avec confirmation valide
Préconditions : acquisition arrêtée, aucune opération critique non terminée, `confirm_key = 0xA55A`.

Oracle FT-CMD : la commande ne doit pas être refusée pour absence de confirmation ni acquisition en cours et doit pouvoir être prise en compte.

Le redémarrage et l'état après reboot relèvent de FT-PER.

## TT-CMD-B05-604 — RESET SOFTWARE pendant acquisition
Commande 10, clé valide, acquisition active.

Oracle : refus avec `cmd_result_code = 5`.

## TT-CMD-B05-605 — RESET SOFTWARE avec opération critique non terminée
Classification : `CONDITIONAL`.

Construire ce cas uniquement si le banc/DUT permet d'identifier sans hypothèse une opération critique non terminée.

Oracle disponible : la commande ne doit pas être acceptée. Le code résultat exact est `NOT_DEFINED` en V1 ; ne pas imposer `3`, `17`, `18` ou un autre code par déduction.

## TT-CMD-B05-606 — RESET STATISTICS sans confirmation valide
Commande 11 avec absence de clé valide.

Oracle : refus avec `cmd_result_code = 9`.

## TT-CMD-B05-607 — RESET STATISTICS avec confirmation valide
Commande 11, `confirm_key = 0xA55A`, `param1 = 0`.

Oracle FT-CMD : la commande doit pouvoir être prise en compte. Les non-effets sur campagnes, identité, configuration et journaux critiques restent vérifiés dans la famille inter-blocs appropriée.

## TT-CMD-B05-608 — Absence de protection des commandes 1..9
Pour une commande non protégée choisie parmi 1..9 et autrement admissible, utiliser `confirm_key = 0x0000`.

Oracle : l'absence de `0xA55A` ne doit pas, à elle seule, provoquer le refus `cmd_result_code = 9`.

## Règles communes
- `transaction_id` frais ;
- pas d'oracle générique sur `cmd_result_detail` ;
- pas de séquence intermédiaire imposée ;
- ne pas transformer une politique recommandée en exigence normative.
