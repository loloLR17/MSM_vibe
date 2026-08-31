# FT-INT-02 — Source normalisée

## 1. Références normatives

- `01_Specification_source/bloc4.md`
- `01_Specification_source/bloc5.md`
- `02_Validation/FT_BLK/FT-BLK-04_Cycle_configuration/`

Les compléments métier explicitement informatifs ne servent pas d'oracle.

## 2. Exigences retenues

### INT02-R01 — Application effective après commande B5 réussie

- Blocs : B4 ↔ B5.
- Source : B4, logique préparé/validé/actif ; B5, commande `1` appliquer configuration préparée.
- Exigence : la configuration préparée ne devient active qu'à la suite d'une commande B5 d'application réussie.
- Classification : `COVERED`.
- Test : `TT-INT-B04B05-001`.
- Frontière : l'absence d'effet immédiat avant commande est déjà couverte par FT-BLK-04 ; FT-INT-02 vérifie uniquement la branche positive inter-blocs après succès.

### INT02-R02 — Mise à jour de active_config_id

- Blocs : B4 ↔ B5.
- Source : B4, `prepared_config_id` / `active_config_id` et logique d'activation ; B5, effet de la commande d'application.
- Exigence : après application réussie, l'identité active correspond à la configuration qui vient d'être appliquée.
- Classification : `COVERED`.
- Test : `TT-INT-B04B05-002`.

### INT02-R03 — Transition vers ACTIF

- Blocs : B4 ↔ B5.
- Source : B4 §7.1, transition normative `VALIDE` + commande d'application réussie → `ACTIF`.
- Exigence : après succès de la commande d'application, `config_state = ACTIF`.
- Classification : `COVERED`.
- Test : `TT-INT-B04B05-003`.

### INT02-R04 — Cohérence de l'image active 4E

- Blocs : B4 ↔ B5.
- Source : B4, image active 4E RO et séparation préparé/actif ; B5, effet de la commande d'application.
- Exigence : après succès, l'image active 4E reflète la configuration effectivement appliquée.
- Classification : `COVERED`.
- Test : `TT-INT-B04B05-004`.
- Limite : le test ne redéfinit ni domaines, ni droits d'accès, ni algorithme CRC.

### INT02-R05 — Cohérence de active_config_crc

- Blocs : B4 ↔ B5.
- Exigence : après application réussie, `active_config_crc` doit rester cohérent avec l'image active.
- Classification : `COVERED` comme contrôle associé à `TT-INT-B04B05-004`.
- Propriétaire de l'algorithme CRC : FT-BLK-04.
- Justification : FT-INT observe la cohérence post-application ; FT-BLK-04 reste seul propriétaire du calcul CRC, de sa sérialisation et du vecteur normatif.

### INT02-R06 — Application interdite si config_state != VALIDE

- Blocs fonctionnellement impliqués : B4 ↔ B5.
- Source : B4 §7.1.
- Classification : `DELEGATED`.
- Propriétaire : FT-CMD.
- Justification : le verdict principal porte sur l'acceptation/refus de la commande B5.

### INT02-R07 — CRC préparé incorrect

- Blocs fonctionnellement impliqués : B4 ↔ B5.
- Classification : `DELEGATED`.
- Propriétaire principal : FT-CMD pour le refus ; FT-BLK-04 pour la construction/validation CRC ; FT-LIM pour les domaines lorsque pertinent.
- Justification : FT-INT-02 n'a pas à réexécuter les oracles spécialisés.

### INT02-R08 — Configuration préparée incomplète

- Blocs fonctionnellement impliqués : B4 ↔ B5.
- Classification : `DELEGATED`.
- Propriétaire : FT-CMD pour le refus et le résultat de commande.

### INT02-R09 — Échec réel d'application

- Blocs : B4 ↔ B5.
- Source : B4 §7.1, transition `VALIDE` + commande d'application échouée → `ERREUR_APPLICATION`.
- Classification : `CONDITIONAL`.
- Justification : la V1 distingue un refus préalable d'un véritable échec d'application. Aucun moyen reproductible d'injection d'un échec interne d'application n'est défini dans le référentiel V1 courant.
- Condition de promotion vers `COVERED` : disposer d'un mécanisme de banc ou firmware explicitement défini permettant de provoquer un échec réel pendant l'application sans confondre celui-ci avec un refus préalable.

## 3. Anti-duplication

FT-INT-02 ne répète pas :
- séparation préparé / actif avant commande : FT-BLK-04 ;
- invalidation de `VALIDE` après modification : FT-BLK-04 ;
- CRC préparé et vecteur normatif : FT-BLK-04 ;
- domaines et valeurs invalides : FT-LIM ;
- refus et codes résultat de commande : FT-CMD.

## 4. Règles anti-fabrication

- ne pas assimiler un refus de commande à un échec d'application ;
- ne pas reconstruire un oracle CRC déjà possédé par FT-BLK-04 ;
- ne pas exiger une politique d'incrément de `config_revision_counter` absente de la V1 ;
- ne pas déduire de comportement post-reboot dans FT-INT-02 ;
- ne pas étendre l'oracle à B3 : les effets supervision appartiennent à FT-INT-03.
