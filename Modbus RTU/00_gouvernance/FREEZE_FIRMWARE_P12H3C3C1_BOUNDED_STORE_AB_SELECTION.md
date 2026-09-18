# FREEZE — Firmware P12-H3c3-C1 — Sélection A/B du store borné

## Statut

P12-H3c3-C1 est gelée après validation locale Host et cross-build STM32.

Le matériel cible n'étant pas disponible pour cette tranche, la validation matérielle reste **HARDWARE PENDING**.

## Périmètre gelé

Cette tranche introduit le mécanisme élémentaire de lecture et de sélection des copies persistantes A/B d'un slot logique du futur journal de commandes borné.

Elle ne remplace pas encore `CommandJournalStore` et ne modifie pas le comportement métier B5.

## Géométrie

- 256 slots logiques.
- 2 copies persistantes A/B par slot.
- 70 octets par copie, format V3.
- taille brute : 35 840 octets.
- `transaction_id` n'intervient jamais dans l'adressage physique.

Formule :

```text
offset = (logical_slot * 2 + copy_index) * 70
```

avec `logical_slot` dans 0..255 et `copy_index` dans 0..1.

## Classification des copies et sélection

Une copie entièrement à `0x00` ou entièrement à `0xFF` est vide.

Après lecture et décodage V3 :

- deux copies vides -> `EMPTY`;
- une copie `VALID` -> cette copie est retenue;
- deux copies `VALID` de générations différentes -> la génération la plus élevée est retenue;
- deux copies `VALID` de même génération et contenu identique -> slot `VALID`;
- deux copies `VALID` de même génération mais contenus différents -> `CORRUPTED`;
- `VALID + CORRUPTED` -> la copie valide est retenue;
- `VALID + UNSUPPORTED` -> la copie valide est retenue;
- aucune copie valide et au moins une `UNSUPPORTED` -> `UNSUPPORTED`;
- aucune copie valide, aucune `UNSUPPORTED`, au moins une `CORRUPTED` -> `CORRUPTED`;
- toute erreur physique de lecture -> `UNAVAILABLE`, sans masquage par l'autre copie.

## Invariant power-loss

Le mécanisme permet de conserver une ancienne copie valide lorsque l'écriture de la nouvelle copie est corrompue ou incomplète.

Les futures mutations devront écrire la copie opposée avec `generation + 1`.

Une réadmission après éviction devra également continuer la génération physique du slot et ne devra pas repartir à 1 tant qu'une ancienne copie peut subsister.

Aucun wrap silencieux de `generation` n'est autorisé.

## Implémentation gelée

- `include/tr2/persistence/command_journal_bounded_slot.h`
- `src/persistence/command_journal_bounded_slot.c`
- `tests/unit/test_p12h3c3c1_bounded_slot.c`

Le helper est compilé dans `tr2_core` et donc contrôlé par les builds Host et Cortex-M33.

## Validation acquise

Validation locale rapportée après la couverture finale :

```text
73/73 tests passed
HOST VALIDATED
CROSS-BUILD VALIDATED
HARDWARE PENDING
```

Les tests couvrent notamment :

- offsets extrêmes;
- slot vide;
- copie valide unique;
- choix de la génération la plus récente;
- repli sur l'ancienne copie valide si la nouvelle est corrompue;
- conflit de deux copies valides de même génération;
- copies identiques de même génération;
- masquage d'une copie unsupported par une copie valide;
- unsupported sans copie valide;
- erreur physique de lecture.

## Hors périmètre

Restent hors C1 :

- scan/recovery global des 256 slots;
- détection des transaction_id dupliqués entre slots;
- détection de plusieurs transactions non terminales;
- reconstruction des compteurs admission/completion;
- find/visit/latest_completed du store borné;
- admission, éviction et réutilisation des slots;
- mutations persistantes du journal;
- choix du média persistant STM32 de production.

Ces points relèvent des tranches P12-H3c3-C2 et suivantes.
