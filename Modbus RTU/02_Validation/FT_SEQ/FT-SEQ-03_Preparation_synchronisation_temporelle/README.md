# FT-SEQ-03 — Préparation et synchronisation temporelle

## 1. Objet

Valider la chaîne fonctionnelle nominale complète permettant de préparer une référence temporelle B2 puis de l'appliquer effectivement par la commande B5 SYNC TIME.

FT-SEQ-03 compose les oracles gelés sans redéfinir la logique temporelle élémentaire ni la mécanique de commande.

## 2. Résultat d'audit

La V1 définit suffisamment la chaîne nominale :

`préparation B2` → `absence d'effet immédiat` → `SYNC TIME réussi` → `application effective` → `last_sync_time mis à jour` → `état temporel cohérent`.

Un test séquentiel propriétaire est créé :
- `TT-SEQ-TIME-001` — préparation puis synchronisation temporelle nominale.

## 3. Couverture

- `COVERED` propriétaire FT-SEQ : 1
- `CONDITIONAL` : 0
- `DELEGATED` : 5
- `TRACE_ONLY` : 1
- `NOT_DEFINED` : 4

## 4. Délégations

- absence d'effet immédiat, monotonie et cohérence interne B2 : FT-BLK-02 ;
- acceptation/refus et résultat SYNC TIME : FT-CMD-05 ;
- application effective, `last_sync_time` et cohérence minimale post-synchronisation : FT-INT-01 ;
- structure, accès et domaines : FT-STR / FT-ACC / FT-LIM.

## 5. Garde-fou temporel majeur

FT-SEQ-03 n'impose jamais une égalité stricte entre valeurs temporelles lues lors de transactions distinctes.

Le temps continue de progresser entre l'application et la lecture. L'oracle retenu est donc celui déjà gelé par FT-INT-01 : cohérence avec la référence préparée et le temps réellement écoulé, sans inventer de tolérance chiffrée absente de V1.

## 6. Limites V1 conservées

Ne sont pas transformés en exigences :
- égalité bit-à-bit `current_time == prepared_time` après commande ;
- égalité bit-à-bit universelle entre `last_sync_time` et une lecture antérieure ;
- délai maximal global préparation→synchronisation ;
- équivalence exacte entre l'indicateur B5 de synchronisation préparée et les états/flags B2.

## 7. Frontières

- refus SYNC TIME isolé : FT-CMD-05 ;
- refus 19 puis préparation puis succès : FT-SEQ-07 ;
- persistance/reboot : FT-PER ;
- perturbations temporelles hostiles : FT-RBT.

Voir `source/FT-SEQ-03_source.md` et `detaille/TT-SEQ-TIME-001.md`.