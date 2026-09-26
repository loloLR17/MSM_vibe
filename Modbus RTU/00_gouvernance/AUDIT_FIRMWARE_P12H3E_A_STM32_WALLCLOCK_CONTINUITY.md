# P12-H3e-A — Audit WallClock / continuité temporelle STM32

## 1. Objet

Arbitrer l'implémentation STM32 de `WallClock` et `TimeContinuityEvidenceProvider` avant composition complète de `SystemRuntime`.

Référence de travail : branche `main`, après gel physique P12-H3d3-D3-C.

## 2. Contrats portables existants

`WallClock` expose :

- `read(context, timestamp)` -> `WALL_CLOCK_OK`, `WALL_CLOCK_INVALID` ou `WALL_CLOCK_UNAVAILABLE`;
- `set(context, timestamp)` -> `Tr2Result`.

`TimeContinuityEvidenceProvider` expose trois états :

- `INDETERMINATE`;
- `PROVEN`;
- `BROKEN`.

Le core portable ne doit pas dépendre de HAL/CMSIS.

## 3. Capacités matérielles STM32U575 retenues

Le STM32U575 dispose d'un domaine backup alimentable par VBAT contenant notamment :

- le RTC ;
- 32 registres backup 32 bits ;
- 2 Kio de backup SRAM.

Le RTC peut fonctionner dans le domaine VBAT avec le LSE. La NUCLEO-U575ZI-Q MB1549 dispose d'un quartz LSE 32,768 kHz.

La carte expose VBAT sur le connecteur Morpho. Dans la configuration NUCLEO documentée, SB50 est ON pour l'alimentation VBAT. Une qualification de continuité à travers une coupure complète d'alimentation ne doit donc pas être revendiquée sans configuration explicite d'une alimentation VBAT de secours.

## 4. Politique de continuité retenue

La présence d'un RTC qui fournit une date plausible ne constitue pas, seule, une preuve de continuité.

La politique STM32 doit utiliser une preuve conservée dans le domaine backup, indépendante de la RAM normale :

- après une synchronisation civile réussie, un marqueur/version de continuité est écrit dans un registre backup RTC ;
- au boot, un marqueur reconnu et un RTC lisible/valide autorisent `TIME_CONTINUITY_EVIDENCE_PROVEN`;
- un défaut matériel explicitement détectable du domaine backup/LSE peut conduire à `TIME_CONTINUITY_EVIDENCE_BROKEN`;
- absence de marqueur, marqueur inconnu, RTC invalide ou situation non distinguable d'un premier démarrage conduisent à `TIME_CONTINUITY_EVIDENCE_INDETERMINATE`.

En particulier, une perte totale de VDD et VBAT peut effacer simultanément RTC et preuve backup. Au redémarrage, si aucune cause matérielle persistante ne permet de prouver la rupture, l'état honnête est `INDETERMINATE`, pas artificiellement `BROKEN`.

## 5. WallClock STM32

L'implémentation doit :

1. utiliser le RTC STM32 avec le LSE 32,768 kHz ;
2. convertir explicitement entre le calendrier RTC et `Tr2CivilTimestamp` ;
3. refuser les dates hors domaine représentable ;
4. retourner `WALL_CLOCK_INVALID` si le contenu RTC n'est pas reconnu comme initialisé/valide ;
5. retourner `WALL_CLOCK_UNAVAILABLE` en cas d'indisponibilité matérielle ;
6. n'écrire le marqueur de continuité qu'après un réglage RTC réussi ;
7. ne pas fabriquer une heure par défaut au boot.

## 6. Conséquences pour la NUCLEO actuelle

Deux niveaux de qualification sont séparés :

### Niveau A — reset avec carte alimentée

Peut qualifier :

- initialisation LSE/RTC ;
- set/read du WallClock ;
- conservation RTC + marqueur à travers reset logiciel/externe ;
- transition de continuité vers PROVEN après synchronisation.

### Niveau B — coupure complète d'alimentation

Ne sera qualifié comme continuité PROVEN que si VBAT reste réellement alimenté pendant la coupure VDD.

Avec la configuration actuelle sans alimentation de secours explicitement qualifiée, une coupure totale doit être traitée comme perte possible de preuve et donc comme INDETERMINATE au redémarrage.

## 7. Tranche d'implémentation suivante

P12-H3e-B doit rester minimale :

- ajouter `WallClock` et `TimeContinuityEvidenceProvider` au package `stm32_runtime_platform`;
- configurer RTC + LSE sans modifier le core portable ;
- réserver un registre backup pour le marqueur/version ;
- fournir conversion timestamp/calendrier bornée ;
- cross-builder ;
- ajouter les tests host possibles sur la logique de conversion si elle est extraite en code portable/testable.

La validation physique viendra ensuite dans une tranche séparée.

## 8. Hors périmètre

- calibration fine ppm du LSE ;
- batterie de secours de production ;
- endurance ;
- précision longue durée ;
- synchronisation réseau/GNSS ;
- modification des sémantiques du `TimeService`;
- nouvelle qualification FRAM.

## 9. Décision

La continuité n'est jamais déduite de la seule plausibilité du RTC.

**PROVEN exige une preuve backup reconnue et un RTC valide.**

**INDETERMINATE est la valeur sûre lorsqu'une rupture ne peut pas être distinguée d'un premier démarrage.**

Cette politique permet une implémentation STM32 réelle sans sur-promettre la continuité à travers une coupure complète non secourue.
