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

- après une synchronisation civile réussie, l'implémentation STM32 publie dans le domaine backup une paire cohérente : une ancre `Tr2CivilTimestamp` dans `RTC_BKP_DR1`, puis un marqueur/version dans `RTC_BKP_DR0` ;
- le marqueur est publié en dernier et la paire marqueur/ancre est relue après écriture ;
- au boot, `TIME_CONTINUITY_EVIDENCE_PROVEN` exige le marqueur reconnu, un RTC lisible/valide et un timestamp courant supérieur ou égal à l'ancre ;
- un défaut matériel explicitement détectable du domaine backup/LSE peut conduire à `TIME_CONTINUITY_EVIDENCE_BROKEN`;
- absence de marqueur, marqueur inconnu, RTC invalide ou situation non distinguable d'un premier démarrage conduisent à `TIME_CONTINUITY_EVIDENCE_INDETERMINATE`.

En particulier, une perte totale de VDD et VBAT peut effacer simultanément RTC et preuve backup. Au redémarrage, si aucune cause matérielle persistante ne permet de prouver la rupture, l'état honnête est `INDETERMINATE`, pas artificiellement `BROKEN`.

## 5. WallClock STM32

L'implémentation doit :

1. utiliser le RTC STM32 avec le LSE 32,768 kHz ;
2. convertir explicitement entre le calendrier RTC et `Tr2CivilTimestamp` selon l'**Epoch TR2 = 2020-01-01 00:00:00 UTC**, conformément à `charte_typage.md` ;
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


## 10. Consolidation après implémentation et qualification H3e-C

L'implémentation réelle a renforcé la politique initiale avec une preuve backup liée au calendrier :

- `RTC_BKP_DR0 = 0x54523202` : marqueur/version de continuité ;
- `RTC_BKP_DR1` : ancre `Tr2CivilTimestamp` correspondant au dernier `WallClock.set()` réussi ;
- publication : ancre d'abord, marqueur ensuite, puis relecture des deux registres ;
- `PROVEN` exige un marqueur reconnu, un `WallClock.read()` valide et `timestamp_courant >= ancre` ;
- toute incohérence conservatrice retourne `INDETERMINATE`.

Cette liaison marqueur + ancre a été introduite après l'observation physique d'un cas où un marqueur backup pouvait être présent alors que le calendrier lu n'était pas exploitable comme preuve suffisante. Elle empêche qu'un marqueur retenu soit, à lui seul, interprété comme preuve de continuité.

La conversion STM32 utilise l'Epoch TR2 normative `2020-01-01 00:00:00 UTC`. La valeur `0` est donc un timestamp valide et ne peut pas servir de sentinelle d'absence d'ancre.

La qualification physique H3e-C a démontré, carte alimentée, la séquence :

1. synchronisation RTC et publication marqueur + ancre ;
2. reset matériel externe de la NUCLEO ;
3. récupération d'un `WallClock` valide ;
4. `TIME_CONTINUITY_EVIDENCE_PROVEN` sans nouvel appel à `set()` ;
5. progression du timestamp observé.

Lors de la preuve finale, les sondes ont donné notamment :

- `initial_read_result = WALL_CLOCK_OK` ;
- `initial_continuity = PROVEN` ;
- `set_attempted = 0` ;
- `post_read_result = WALL_CLOCK_OK` ;
- `post_continuity = PROVEN` ;
- `completed = 1` ;
- timestamp passé de `0x6A18A528` à `0x6A18A5CE`, soit +166 s entre deux observations.

Les lectures brutes `TR/DR` effectuées avant l'initialisation/synchronisation HAL ne sont pas retenues comme preuve autonome de perte du calendrier : durant le diagnostic, elles présentaient `00:00:00 / 01-01-2000` alors que le calendrier retenu redevenait lisible après `HAL_RTC_Init()` et que la continuité fonctionnelle était ensuite démontrée sans `set()`. Aucune cause non démontrée n'est attribuée à ce comportement intermédiaire.

Le niveau B reste explicitement non qualifié : aucune continuité `PROVEN` n'est revendiquée après coupure complète VDD/VBAT sans alimentation VBAT indépendante qualifiée.
