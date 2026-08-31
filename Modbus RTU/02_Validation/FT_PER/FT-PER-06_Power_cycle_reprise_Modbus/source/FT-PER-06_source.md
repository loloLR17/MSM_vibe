# FT-PER-06 — Source normative consolidée

## PER06-R01 — Cause power-on dans B1

**Classification : `COVERED`, exécution `CONDITIONAL`.**

Après une mise sous tension effectivement provoquée et identifiable, `B1.last_reset_cause = 1` signifie `Power-on`.

Test : `TT-PER-B01B07-002`.

## PER06-R02 — Cause power-on dans B7

**Classification : `COVERED`, exécution `CONDITIONAL`.**

Après une mise sous tension effectivement provoquée et identifiable, `B7.reset_cause = 1` signifie `power-on`.

Test : `TT-PER-B01B07-002`.

## PER06-R03 — Distinction power-on / reset logiciel

**Classification : `COVERED`.**

La V1 attribue des codes distincts : `1` pour power-on et `2` pour reset logiciel. Les deux causes ne doivent donc pas être confondues dans les observables de cause de reset.

## PER06-R04 — Cohérence générale B1/B7

**Classification : `DELEGATED` vers FT-INT-05.**

FT-PER-06 applique à chaque champ sa propre sémantique normative sans créer une nouvelle règle générale d'égalité inter-blocs.

## PER06-R05 — Uptime après power-on

**Classification : `TRACE_ONLY`.**

B7 définit l'uptime comme le temps depuis le dernier reset. Aucun instant exact de première lecture ni délai de boot n'étant défini, aucune valeur exacte n'est imposée au premier accès post-boot.

## PER06-R06 — Délai maximal de boot

**Classification : `NOT_DEFINED`.**

## PER06-R07 — Délai maximal avant première réponse Modbus valide

**Classification : `NOT_DEFINED`.**

## PER06-R08 — Comportement Modbus pendant le boot

**Classification : `NOT_DEFINED`.**

La V1 ne spécifie ni silence obligatoire, ni exception, ni réponse particulière pendant l'initialisation.

## PER06-R09 — État système initial après power-on

**Classification : `NOT_DEFINED`.**

Aucune valeur générale obligatoire de `system_status`, `acquisition_state`, défauts, configuration ou moteur de commandes n'est définie au boot.

## PER06-R10 — Équivalence de persistance power cycle / RESET SOFTWARE

**Classification : `NOT_DEFINED`.**

La distinction des causes de reset n'implique pas une politique identique de conservation des données.

## PER06-R11 — Politique par autres causes de reset

**Classification : `NOT_DEFINED`.**

Watchdog, brown-out, reset externe et mise à jour firmware ont des codes normatifs, mais aucune matrice V1 ne définit les propriétés de reprise/persistance pour chacune de ces causes.

## PER06-R12 — Temps de reprise Modbus mesuré

**Classification : `TRACE_ONLY`.**

La mesure peut être conservée pour caractérisation industrielle et future spécification, mais ne produit aucun PASS/FAIL V1.

## Règles anti-fabrication

- ne pas imposer un boot time arbitraire ;
- ne pas imposer `uptime = 0` à la première réponse ;
- ne pas transformer le premier retour Modbus en exigence temporelle ;
- ne pas supposer que power cycle et software reset ont la même politique de persistance ;
- ne pas inventer un état par défaut global ;
- ne pas qualifier une cause de reset sur la seule perte de communication.
