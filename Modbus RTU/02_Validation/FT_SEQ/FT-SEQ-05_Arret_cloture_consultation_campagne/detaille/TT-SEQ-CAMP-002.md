# TT-SEQ-CAMP-002 — Arrêt nominal, clôture et consultation de campagne

## 1. Objectif

Valider de bout en bout qu'une campagne réellement en cours peut être arrêtée proprement par STOP, que l'acquisition devient arrêtée, que la campagne est mise en cohérence/clôturée et qu'elle reste consultable dans l'inventaire B6.

## 2. Sources

- `01_Specification_source/bloc1.md`
- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc5.md`
- `01_Specification_source/bloc6.md`
- FT-CMD-06 — STOP ACQUISITION
- FT-INT-04 — acquisition et campagnes
- FT-BLK-05 — inventaire campagnes

## 3. Préconditions

- acquisition effectivement active ;
- campagne B6 effectivement en cours, établie selon les oracles gelés ;
- contexte suffisant enregistré pour retrouver cette campagne après STOP sans utiliser une relation non normative ;
- accès aux observables B1, B5 et B6 nécessaires.

Le scénario peut être préparé par `TT-SEQ-CAMP-001`, mais ne dépend pas d'une égalité `B1.active_campaign_id == B6.campaign_id`.

## 4. Procédure

1. Avant STOP, sélectionner/observer la campagne en cours selon les mécanismes B6 normatifs et enregistrer les métadonnées nécessaires à son identification ultérieure.
2. Vérifier que l'acquisition est active et que la campagne observée satisfait les invariants applicables à une campagne en cours.
3. Soumettre la commande B5 STOP dans une transaction conforme à FT-CMD.
4. Attendre l'état terminal selon FT-CMD, sans imposer une succession intermédiaire ni une borne temporelle absente de V1.
5. Vérifier avec FT-CMD-06 que STOP se termine avec succès.
6. Lire B1 et vérifier avec FT-INT-04 que l'acquisition est effectivement arrêtée (`acquisition_state = 0`).
7. Retrouver dans B6 la campagne qui était en cours avant STOP, via les mécanismes normatifs de sélection/inventaire et le contexte enregistré.
8. Vérifier avec FT-INT-04 que cette campagne a été mise en cohérence/clôturée par STOP.
9. Vérifier avec FT-BLK-05 les invariants internes applicables à l'entrée sélectionnée finale, notamment validité de sélection et `campaign_id != 0`.
10. Vérifier avec FT-INT-04 que les timestamps finaux utilisent la base temporelle B2, sans exiger une égalité stricte avec un timestamp de STOP lu séparément.
11. Consigner comme traces les effets normatifs « flush buffers » et « fermeture fichiers » ; leur absence de témoin Modbus direct interdit un verdict autonome sur chacun d'eux.

## 5. État final de campagne

`campaign_state = 3` signifie « terminée » et constitue l'état nominal attendu pour une campagne normalement clôturée lorsque les conditions observées permettent d'appliquer cet oracle.

Le test ne transforme toutefois pas cette valeur en règle universelle couvrant aussi des situations d'erreur ou de corruption prévues par B6. Le verdict doit suivre l'oracle borné de FT-INT-04 : campagne mise en cohérence/clôturée conformément au contexte normatif réellement rencontré.

## 6. Verdict FT-SEQ

### PASS

Dans la même exécution :
- campagne et acquisition sont initialement en cours ;
- STOP réussit ;
- l'acquisition devient arrêtée ;
- la campagne précédemment active est retrouvée et mise en cohérence/clôturée ;
- elle est consultable avec des métadonnées finales satisfaisant les oracles applicables.

### FAIL

Un jalon normatif observable échoue alors que ses préconditions sont satisfaites.

Le rapport identifie le jalon et la famille propriétaire de l'oracle élémentaire.

## 7. Non-oracles

Ne constituent pas à eux seuls un FAIL :
- impossibilité d'observer directement un « buffer vide » ou un « fichier fermé » faute de témoin V1 ;
- `end_timestamp` différent d'un timestamp de commande lu dans une autre transaction ;
- absence d'égalité universelle `duration_s = end_timestamp - start_timestamp` ;
- absence de finalisation dans un délai arbitraire ;
- absence d'égalité entre identifiants B1 et B6 non normée.

## 8. Traçabilité

- Exigence propriétaire : `SEQ05-R01`.
- Oracles composés : FT-CMD-06 + FT-INT-04 + FT-BLK-05.
- Ouverture : FT-SEQ-04.
- Cycle complet : FT-SEQ-06.
- Refus/reprise : FT-SEQ-07.