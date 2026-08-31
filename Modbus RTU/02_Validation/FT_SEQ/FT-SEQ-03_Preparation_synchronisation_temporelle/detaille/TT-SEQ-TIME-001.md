# TT-SEQ-TIME-001 — Préparation puis synchronisation temporelle nominale

## 1. Objectif

Valider de bout en bout qu'une centrale peut préparer une référence temporelle B2 sans modifier immédiatement l'horloge, puis appliquer cette référence par une commande B5 SYNC TIME réussie et retrouver un état temporel cohérent.

## 2. Sources

- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc5.md`
- FT-BLK-02 — temps, monotonie et dérivations
- FT-CMD-05 — SYNC TIME
- FT-INT-01 — temps ↔ commandes

## 3. Préconditions

- horloge disponible ;
- valeur de temps préparé conforme au domaine V1 ;
- accès aux observables B2 et B5 nécessaires ;
- mécanisme de mesure du banc permettant d'ordonner les lectures et écritures sans prétendre à leur simultanéité.

## 4. Procédure

1. Lire et enregistrer l'état temporel initial nécessaire au scénario, notamment `current_time` et `last_sync_time` lorsque disponibles.
2. Écrire une nouvelle référence temporelle dans la zone préparée B2 conformément à la V1.
3. Avant toute commande SYNC TIME, vérifier avec l'oracle FT-BLK-02 que la seule préparation n'a pas réglé immédiatement l'horloge système et n'a pas matérialisé une synchronisation effective.
4. Soumettre la commande B5 SYNC TIME dans une transaction conforme à FT-CMD.
5. Attendre son état terminal selon les règles FT-CMD, sans imposer une succession intermédiaire de `cmd_status` ni un délai non défini par la V1.
6. Vérifier avec FT-CMD-05 que la commande se termine avec succès.
7. Lire les observables temporels B2 après succès.
8. Vérifier avec FT-INT-01 que `current_time` est cohérent avec la référence temporelle préparée et le temps réellement écoulé entre application et lecture.
9. Vérifier avec FT-INT-01 que `last_sync_time` a été mis à jour vers la nouvelle référence temporelle effectivement appliquée.
10. Vérifier avec l'oracle borné FT-INT-01 que les observables d'état temporel B2 ne restent pas contradictoires avec une synchronisation effectivement réussie.

## 5. Verdict FT-SEQ

### PASS

Tous les jalons normatifs sont satisfaits dans la même exécution :
- préparation du temps ;
- absence d'application immédiate ;
- succès transactionnel SYNC TIME ;
- application effective cohérente ;
- mise à jour de `last_sync_time` ;
- état temporel post-synchronisation non contradictoire.

### FAIL

Au moins un jalon normatif échoue alors que ses préconditions sont satisfaites.

Le rapport doit identifier le jalon fautif et la famille propriétaire de l'oracle élémentaire.

## 6. Règles de comparaison temporelle

Ce test n'utilise pas une égalité bit-à-bit entre valeurs issues de transactions différentes.

La comparaison de `current_time` après synchronisation doit reprendre l'oracle FT-INT-01 : cohérence avec le temps préparé et le temps réellement écoulé. Si le banc ne permet pas d'établir cette cohérence selon l'oracle gelé, le test doit être déclaré non concluable pour ce jalon plutôt que d'inventer une tolérance.

## 7. Non-oracles

Ne constituent pas à eux seuls un FAIL :
- `current_time` différent numériquement du temps préparé à cause du temps écoulé ;
- absence d'égalité bit-à-bit entre lectures séparées ;
- absence d'une combinaison précise de `time_status`, `time_flags` et `prepared_time_status` non imposée par V1 ;
- durée de traitement sans dépassement d'une borne normative existante ;
- différence entre indicateurs B2 et B5 lorsqu'aucune équivalence normative explicite n'existe.

## 8. Traçabilité

- Exigence propriétaire : `SEQ03-R01`.
- Oracles composés : FT-BLK-02 + FT-CMD-05 + FT-INT-01.
- Refus puis reprise : FT-SEQ-07.
- Reboot/persistance temporelle : FT-PER.
- Timing hostile : FT-RBT.