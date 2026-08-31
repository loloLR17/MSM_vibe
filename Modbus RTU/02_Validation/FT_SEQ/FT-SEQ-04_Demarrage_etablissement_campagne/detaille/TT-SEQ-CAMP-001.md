# TT-SEQ-CAMP-001 — Démarrage nominal et établissement d'une campagne

## 1. Objectif

Valider de bout en bout qu'une commande START exécutée depuis un contexte V1 valide conduit effectivement à une acquisition active et à l'ouverture d'une nouvelle campagne B6 cohérente avec l'état « en cours ».

## 2. Sources

- `01_Specification_source/bloc1.md`
- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc5.md`
- `01_Specification_source/bloc6.md`
- FT-CMD-06 — START ACQUISITION
- FT-INT-04 — acquisition et campagnes
- FT-BLK-05 — inventaire campagnes

## 3. Préconditions

Selon les oracles propriétaires :
- configuration active valide ;
- support SD exploitable ;
- absence de défaut critique bloquant ;
- acquisition non déjà active ;
- accès aux observables B1, B5 et B6 nécessaires.

Les domaines, droits et structures sont délégués aux familles gelées correspondantes.

## 4. Procédure

1. Établir et enregistrer le contexte avant START nécessaire pour démontrer que l'acquisition est arrêtée et pour distinguer la campagne qui sera ouverte des campagnes déjà présentes, sans imposer un mécanisme d'identification non normé.
2. Soumettre la commande B5 START dans une transaction conforme à FT-CMD.
3. Attendre l'état terminal selon FT-CMD, sans imposer de séquence intermédiaire de `cmd_status` ni de délai absent de V1.
4. Vérifier avec FT-CMD-06 que START se termine avec succès.
5. Lire B1 et vérifier avec FT-INT-04 que l'acquisition est effectivement en cours (`acquisition_state = 1`).
6. Consulter B6 selon les mécanismes normatifs d'inventaire/sélection et établir avec FT-INT-04 qu'une nouvelle campagne a été ouverte par ce START.
7. Vérifier avec FT-INT-04 et FT-BLK-05 que la campagne ouverte est cohérente avec l'état en cours, notamment les invariants normatifs applicables à une campagne valide en cours.
8. Vérifier avec FT-INT-04 que les timestamps B6 observés utilisent la base temporelle B2, sans imposer d'égalité exacte avec un timestamp de commande lu séparément.

## 5. Identification de la nouvelle campagne

Le banc doit établir que la campagne observée est celle ouverte par le START en utilisant les observables et mécanismes B6 normatifs disponibles ainsi que le contexte avant/après.

Il est interdit de faire dépendre le verdict exclusivement :
- de `B1.active_campaign_id == B6.campaign_id`, relation `NOT_DEFINED` ;
- de `total_campaign_count_après = total_campaign_count_avant + 1` dès la première lecture, relation `NOT_DEFINED`.

Si le banc ne peut pas établir l'ouverture de la nouvelle campagne sans utiliser un oracle non normatif, le jalon concerné est non concluable et doit être signalé comme tel ; aucune égalité supplémentaire ne doit être inventée.

## 6. Verdict FT-SEQ

### PASS

Dans une même exécution :
- les préconditions START sont satisfaites ;
- START réussit ;
- l'acquisition devient active ;
- une nouvelle campagne est ouverte ;
- cette campagne est cohérente avec l'état en cours ;
- les contrôles temporels applicables sont satisfaits.

### FAIL

Un jalon normatif échoue alors que ses préconditions sont satisfaites.

Le rapport doit identifier le jalon fautif et la famille propriétaire de l'oracle élémentaire.

## 7. Non-oracles

Ne constituent pas à eux seuls un FAIL :
- différence entre `B1.active_campaign_id` et un `campaign_id` B6 faute d'égalité normative explicite ;
- compteur total ne présentant pas immédiatement un `+1` strict ;
- absence de visibilité dans un délai arbitraire ;
- différence numérique entre timestamps issus de transactions non simultanées ;
- absence d'arrêt automatique après `campaign_duration_s` sans exigence V1 explicite correspondante.

## 8. Traçabilité

- Exigence propriétaire : `SEQ04-R01`.
- Oracles composés : FT-CMD-06 + FT-INT-04 + FT-BLK-05.
- Arrêt/clôture : FT-SEQ-05.
- Cycle complet : FT-SEQ-06.
- Refus/reprise : FT-SEQ-07.