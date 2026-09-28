# FREEZE — Firmware P12-H3e — WallClock et continuité temporelle STM32

## Statut

**GELÉ — VALIDATION LOGICIELLE ET QUALIFICATION PHYSIQUE NIVEAU A ACQUISES**

Le candidat nettoyé a passé la validation complète locale `tr2_validate.sh` avec succès.

## Périmètre

P12-H3e fournit les adapters STM32 réels pour :

- `WallClock` ;
- `TimeContinuityEvidenceProvider`.

Le core portable reste indépendant de HAL/CMSIS.

## Référence temporelle

La conversion RTC STM32 respecte la référence normative du protocole :

- **Epoch TR2 = 2020-01-01 00:00:00 UTC** ;
- unité : seconde ;
- `Tr2CivilTimestamp = 0` représente exactement l'Epoch TR2 et constitue une valeur valide.

Le calendrier RTC est borné au domaine STM32 utilisé par l'implémentation, de 2020 à 2099, avec validation des jours par mois et des années bissextiles.

## Implémentation STM32

Le RTC utilise :

- LSE 32,768 kHz ;
- format 24 h ;
- prescaler asynchrone 127 ;
- prescaler synchrone 255 ;
- clocks RTC/APB nécessaires activées avant `HAL_RTC_Init()`.

La première qualification physique a mis en évidence un timeout dans `HAL_RTC_Init()` lorsque seules les clocks RTC initialement prévues étaient activées. L'alignement sur l'exemple ST NUCLEO-U575ZI-Q a conduit à activer également `RTCAPB` et `RTCAPB_CLKAM`. Le RTC s'initialise ensuite avec `HAL_OK`.

## Preuve de continuité backup

La continuité ne repose jamais sur la seule plausibilité du calendrier RTC.

Après un `WallClock.set()` réussi :

1. le timestamp synchronisé est écrit dans `RTC_BKP_DR1` comme ancre ;
2. le marqueur/version `0x54523202` est écrit dans `RTC_BKP_DR0` en dernier ;
3. marqueur et ancre sont relus ;
4. l'opération n'est déclarée réussie que si la relecture correspond.

Au boot, `TIME_CONTINUITY_EVIDENCE_PROVEN` exige simultanément :

- plateforme RTC disponible ;
- marqueur/version reconnu ;
- `WallClock.read()` valide ;
- timestamp courant supérieur ou égal à l'ancre backup.

Sinon l'implémentation retourne de façon conservatrice `INDETERMINATE`. Aucun état `BROKEN` n'est fabriqué sans preuve matérielle explicite de rupture.

## Qualification physique H3e-C — niveau A

La campagne a qualifié le cas **reset matériel avec carte restant alimentée**.

La séquence finale observée après synchronisation préalable a donné :

- `initial_read_result = WALL_CLOCK_OK` ;
- `initial_continuity = TIME_CONTINUITY_EVIDENCE_PROVEN` ;
- aucun nouvel appel à `WallClock.set()` ;
- `post_read_result = WALL_CLOCK_OK` ;
- `post_continuity = TIME_CONTINUITY_EVIDENCE_PROVEN` ;
- harnais terminé avec succès.

Le timestamp observé est passé de `0x6A18A528` à `0x6A18A5CE`, soit **+166 secondes**, sans resynchronisation.

Le marqueur et l'ancre sont restés respectivement :

- `BKP0 = 0x54523202` ;
- `BKP1 = 0x6A18A500`.

Cette preuve qualifie la conservation fonctionnelle du WallClock et de la preuve de continuité à travers le reset matériel alimenté testé.

## Diagnostic intermédiaire

Des snapshots bruts pris avant l'initialisation HAL ont montré `TR = 0`, `DR = 0x2101`, tandis qu'après `HAL_RTC_Init()` le calendrier retenu était de nouveau lisible et la continuité était `PROVEN` sans nouvel appel à `set()`.

Ces lectures pré-init ne sont donc pas utilisées comme preuve autonome d'une perte du calendrier. Aucune cause non démontrée n'est attribuée à ce comportement.

L'instrumentation et le harnais H3e-C ont été retirés du candidat final après acquisition des preuves.

## Limites explicites

Ce gel ne qualifie pas :

- continuité après coupure complète VDD/VBAT ;
- fonctionnement avec batterie VBAT de production ;
- brownout/ramp ;
- précision ou dérive longue durée du LSE ;
- calibration ppm ;
- synchronisation réseau/GNSS ;
- comportement arbitraire lors d'une perte d'alimentation pendant `WallClock.set()` au-delà des garanties conservatrices de marker/ancre ;
- `BROKEN` en l'absence d'une preuve matérielle explicite.

Sans alimentation VBAT indépendante qualifiée, une perte complète d'alimentation reste une situation où la continuité peut être perdue et où `INDETERMINATE` est la réponse sûre.

## Non-régression

P12-H3e ne modifie pas :

- les sémantiques B0..B7 ;
- la logique transactionnelle B5 ;
- la baseline FRAM H3d3 ;
- le mapping Modbus ;
- les contrats portables `WallClock` et `TimeContinuityEvidenceProvider`.

La baseline FRAM physique demeure :

`VALID / génération 10 / image B / payload[0] = 0xAC`.

## Validation finale et baseline de gel

Après retrait du harnais et de l'instrumentation temporaire, la validation complète locale a été exécutée avec succès : host et cross-build STM32 entièrement verts.

HEAD du candidat effectivement validé :

`6236a653a2299c54f7722d2b59111526761d3ed9`

Ce HEAD constitue la **baseline logicielle qualifiée P12-H3e**. Le commit documentaire portant le présent statut GELÉ ne modifie aucun code firmware.
