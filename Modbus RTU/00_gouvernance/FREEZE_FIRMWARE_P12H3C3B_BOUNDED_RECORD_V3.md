# FREEZE — P12-H3c3-B — CommandJournal bounded record V3

## Statut

P12-H3c3-B est gelée après revue du layout et validation Host + cross-build locale.

## Format persistant V3

Le record borné occupe exactement 70 octets :

```text
0..3    magic TR2J
4..5    format_version = 3
6..7    record_size = 70
8..11   generation
12..61  CommandJournalEntry sérialisé selon le layout métier existant
58..61  completion_order
62..65  admission_order
66..69  CRC32 sur les octets 0..65
```

Le champ `admission_order` est interne à la persistance et ne modifie pas `CommandJournalEntry` ni B5.

## Invariants

- `generation != 0`;
- `admission_order != 0`;
- l'entrée métier doit satisfaire `command_journal_entry_is_consistent()`;
- magic, taille et CRC doivent être valides;
- une version différente avec enveloppe structurelle/CRC valides est signalée `TR2_ERROR_UNSUPPORTED`;
- aucune migration V2 -> V3 n'est introduite;
- le store dense V2 existant reste inchangé dans cette tranche.

## Tests acquis

- round-trip complet, y compris transaction_id 65535;
- conservation de generation/admission_order et de tous les champs métier;
- refus generation/admission_order nuls;
- détection d'une corruption CRC;
- détection d'une version incompatible avec CRC valide;
- refus des tailles incorrectes.

## Taille cible du futur store

```text
256 slots logiques * 2 copies * 70 octets = 35 840 octets
```

Cette valeur ne sélectionne pas encore le média STM32 et n'inclut pas d'éventuelles métadonnées globales.

## Validation

Validation locale utilisateur :

```text
HOST VALIDATED
CROSS-BUILD VALIDATED
HARDWARE PENDING
```

## Suite

P12-H3c3-C : remplacement atomique du store dense par le store borné 256 entrées, avec sélection A/B, admission_order, éviction du plus ancien COMPLETED et recovery borné.
