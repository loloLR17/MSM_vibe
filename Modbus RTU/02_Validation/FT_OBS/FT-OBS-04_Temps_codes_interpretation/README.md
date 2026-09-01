# FT-OBS-04 — Temps, codes et interprétation déterministe

## 1. Objet

FT-OBS-04 vérifie qu'une centrale peut interpréter de manière déterministe les **codes, états, timestamps et valeurs spéciales** lorsque la V1 leur attribue explicitement une sémantique.

Cette sous-famille ne revalide pas les domaines numériques eux-mêmes : elle vérifie la règle d'exploitation côté centrale : **un code défini peut être interprété ; un code réservé reste non supporté ; une valeur sans table normative ne reçoit aucune signification inventée.**

## 2. Principes V1

- une valeur réservée n'a aucune signification implicite ;
- une sentinelle n'est utilisable que dans le champ où elle est explicitement définie ;
- un timestamp n'est interprétable que selon la base temporelle et la sémantique définies pour son champ ;
- un champ exposé sans table détaillée peut être observable sans être interprétable exhaustivement.

## 3. Cas normativement interprétables

FT-OBS-04 couvre notamment :
- états de temps B2 ;
- états et sévérités B3 ;
- états de configuration B4 ;
- statuts et résultats de commande B5 ;
- états campagne, intégrité et santé stockage B6 ;
- santé système, flags défaut détaillés, autotest et cause reset B7 ;
- sentinelles locales explicitement définies (`B4 ID=0`, paramètres B5 non utilisés à 0, `B6 end_timestamp=0` en campagne en cours, `B7 last_fault_code=0`).

## 4. Cas non interprétables exhaustivement

Restent `NOT_DEFINED` :
- `B1.system_status` ;
- détails de `B1.system_flags`, `fault_flags`, `warning_flags` ;
- tables `B1.error_code` / `warning_code` ;
- `B4.config_error_code` détaillé ;
- catalogue exhaustif `B5.cmd_result_detail` ;
- catalogue détaillé `B7.selftest_result_code` / `selftest_detail` ;
- valeur de `B7.last_fault_timestamp` quand aucun défaut n'est connu.

## 5. Frontières

- validité des domaines et codes réservés : `DELEGATED → FT-LIM` ;
- encodage, MSW/LSW, ASCII : `DELEGATED → FT-STR` ;
- cohérences entre champs/blocs : `DELEGATED → FT-BLK / FT-INT` ;
- mécanique transactionnelle B5 : `DELEGATED → FT-CMD` ;
- comportement après reboot : `DELEGATED → FT-PER`.

## 6. Tests

- `TT-OBS-CODE-001` — interprétation d'un code défini et rejet sémantique d'un code réservé ;
- `TT-OBS-SENT-001` — sentinelles locales : preuve de non-généralisation ;
- `TT-OBS-TIME-001` — interprétation déterministe des timestamps et limites associées.

## 7. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.