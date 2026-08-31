# FT-SEQ-02 — Préparation et activation d'une configuration

## 1. Objet

Valider la chaîne fonctionnelle nominale complète qui transforme une configuration préparée B4 en configuration active au moyen de la commande B5 APPLY CONFIG.

FT-SEQ-02 ne reprend pas les tests unitaires du cycle B4, du moteur B5 ou des effets inter-blocs. Il compose leurs oracles dans une seule exécution séquentielle.

## 2. Résultat d'audit

La V1 définit suffisamment la chaîne nominale pour créer un oracle FT-SEQ propriétaire :

`préparation B4` → `CRC préparé conforme` → `absence d'effet actif immédiat` → `APPLY CONFIG réussi` → `image active cohérente`.

Un test séquentiel propriétaire est donc créé :
- `TT-SEQ-CONFIG-001` — préparation puis activation nominale d'une configuration.

## 3. Couverture

- `COVERED` propriétaire FT-SEQ : 1
- `CONDITIONAL` : 1
- `DELEGATED` : 4
- `TRACE_ONLY` : 0
- `NOT_DEFINED` : 3

## 4. Délégations

- séparation préparé/actif, CRC et cycle B4 : FT-BLK-04 ;
- acceptation, refus et résultat APPLY CONFIG : FT-CMD-05 ;
- effets B5 → B4 après succès : FT-INT-02 ;
- structure, accès et domaines : FT-STR / FT-ACC / FT-LIM.

## 5. Limites V1 conservées

FT-SEQ-02 n'impose pas :
- l'observation externe d'un état intermédiaire `VALIDE` ;
- un délai maximal global préparation→activation ;
- une politique d'incrément de `config_revision_counter` ;
- un scénario reproductible d'échec interne d'application lorsque le banc ne fournit aucun mécanisme normé d'injection.

Le véritable échec interne d'application reste `CONDITIONAL`, conformément à FT-INT-02.

## 6. Frontières

- refus APPLY CONFIG isolé : FT-CMD-05 ;
- refus puis correction puis succès : FT-SEQ-07 ;
- persistance après reboot : FT-PER ;
- perturbations et timing hostiles : FT-RBT.

Voir `source/FT-SEQ-02_source.md` pour la matrice d'exigences et `detaille/TT-SEQ-CONFIG-001.md` pour le cas de test.