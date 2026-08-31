# FT-SEQ — Évolutions candidates V1.1

## 1. Objet

Ce document isole les améliorations possibles révélées par l'audit FT-SEQ sans les promouvoir en exigences V1.

Aucune entrée ci-dessous ne doit être utilisée comme oracle V1.

## 2. Candidats issus de FT-SEQ-01

### V1.1-SEQ-01 — Définir ou non un handshake initial de qualification
Décider explicitement si une centrale doit suivre une séquence minimale de découverte/qualification avant toute commande, et si oui préciser :
- blocs obligatoires ;
- ordre ou absence d'ordre ;
- critère de qualification terminée ;
- comportement si la qualification est incomplète.

### V1.1-SEQ-02 — Cohérence système multi-transactions
Si nécessaire, définir un mécanisme de génération/snapshot global permettant de savoir si plusieurs lectures successives appartiennent au même état système logique.

## 3. Candidats configuration

### V1.1-SEQ-03 — Observabilité de l'état VALIDE
Préciser si `config_state = VALIDE` doit être observable extérieurement avant activation, et le cas échéant quand.

### V1.1-SEQ-04 — Politique de config_revision_counter
Définir précisément les événements qui incrémentent le compteur de révision et le moment où l'incrément devient observable.

### V1.1-SEQ-05 — Injection/diagnostic d'échec interne d'application
Prévoir un moyen de validation reproductible de `ERREUR_APPLICATION`, ou préciser qu'aucun test déterministe externe n'est exigé.

## 4. Candidats temps

### V1.1-SEQ-06 — Tolérance temporelle de validation
Définir, si utile, une méthode et une tolérance permettant de comparer formellement `prepared_time`, `current_time`, `last_sync_time` et timestamps de commande sans dépendre d'une lecture simultanée impossible.

### V1.1-SEQ-07 — Relation entre indicateurs B2 et B5 de préparation temporelle
Définir explicitement si les indicateurs de disponibilité/état de temps préparé des blocs B2 et B5 doivent être équivalents ou seulement corrélés.

## 5. Candidats campagnes

### V1.1-SEQ-08 — Relation active_campaign_id / campaign_id
Décider si `B1.active_campaign_id` doit être égal au `campaign_id` B6 de la campagne courante et formaliser l'oracle si souhaité.

### V1.1-SEQ-09 — Sémantique observable de total_campaign_count
Préciser si START doit produire un incrément exactement égal à 1, à quel instant il devient observable, et comment traiter campagnes invalides/corrompues.

### V1.1-SEQ-10 — État final nominal après STOP
Préciser les conditions exactes conduisant à `campaign_state = 3` et les cas où un STOP réussi peut aboutir à un état d'erreur/corruption.

### V1.1-SEQ-11 — Durée de campagne
Formaliser l'algorithme de `duration_s`, ses arrondis et sa relation exacte avec `start_timestamp` / `end_timestamp`.

### V1.1-SEQ-12 — Observabilité de la finalisation physique
Si utile à la validation, exposer un indicateur permettant de distinguer : buffers vidés, fichiers fermés, métadonnées finalisées.

## 6. Candidats orchestration E2E

### V1.1-SEQ-13 — Ordre métier recommandé versus obligatoire
Séparer explicitement :
- l'ordre recommandé côté centrale ;
- les dépendances obligatoires du protocole ;
- les opérations facultatives.

Cela éviterait qu'une procédure d'exploitation soit interprétée comme machine d'états normative.

### V1.1-SEQ-14 — Synchronisation temporelle avant campagne
Décider si une campagne peut démarrer avec une heure non synchronisée et, si oui, comment la qualité temporelle de la campagne est représentée.

### V1.1-SEQ-15 — Diagnostic final de campagne
Décider si une consultation B7 après STOP est seulement recommandée ou constitue une étape normative dans certains profils d'exploitation.

## 7. Candidats reprise

### V1.1-SEQ-16 — Politique de retry
Si une politique commune est souhaitée, définir :
- nouvelle transaction ou réutilisation d'identifiant ;
- nombre maximal de tentatives ;
- backoff ;
- conditions de reprise autorisées.

Cette politique doit rester cohérente avec FT-CMD.

### V1.1-SEQ-17 — Procédures de récupération de causes externes
Éventuellement définir des procédures testables pour :
- horloge indisponible ;
- support SD absent ;
- mémoire insuffisante ;
- défaut critique bloquant.

En V1, les refus sont normés mais pas leur récupération générique.

## 8. Priorisation suggérée

Priorité haute si une V1.1 vise à rendre le référentiel plus déterministe pour un banc automatisé :
- relation B1/B6 de campagne active ;
- sémantique `total_campaign_count` ;
- règles temporelles/tolérances ;
- distinction ordre recommandé / ordre obligatoire ;
- politique de retry.

Les autres éléments peuvent rester documentaires tant qu'ils ne bloquent pas l'implémentation et l'interopérabilité.
