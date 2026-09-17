# P12-H3c3-B — Format persistant borné et admission_order

## 1. Objet

Définir précisément le nouveau record de slot nécessaire au journal borné avant toute modification du store.

## 2. Principe

Le `CommandJournalEntry` métier reste inchangé.

Le nouveau record persistant associe :
- une génération de copie ;
- un `admission_order` interne au store ;
- le `CommandJournalEntry` existant ;
- un CRC32 couvrant l'ensemble des champs précédents.

`admission_order` n'est pas exposé par B5 et n'appartient pas au domaine métier. Il sert uniquement à l'ordre d'éviction.

## 3. Format V3 proposé

Le format v2 courant occupe 66 octets.

Le format v3 ajoute un uint32 `admission_order` avant le CRC et décale le CRC :

```text
offset  size  champ
0       4     magic TR2J
4       2     format_version = 3
6       2     record_size = 70
8       4     generation
12      2     transaction_id
14      2     lifecycle
16..61        champs métier identiques au format v2
58      4     completion_order
62      4     admission_order
66      4     CRC32 des octets 0..65
total   70
```

## 4. Invariants du record

Un record V3 est valide seulement si :
- `generation != 0`;
- `admission_order != 0`;
- `CommandJournalEntry` est cohérent ;
- magic/version/taille sont exacts ;
- CRC32 est valide.

Le `completion_order` conserve ses règles existantes :
- 0 pour RESERVED/STARTED ;
- non nul pour COMPLETED.

## 5. Compatibilité

Le décodeur V3 n'accepte pas silencieusement V2.

Une image persistante V2 rencontrée par le futur store borné doit être qualifiée `UNSUPPORTED`, sauf si une migration explicite est ultérieurement décidée.

Pour le prototype actuel sans matériel déployé, aucune migration V2 -> V3 n'est requise dans H3c3-B.

## 6. Portée de H3c3-B

Sous-tranches prévues :
- B1 : nouveau type/codec V3 indépendant du codec V2 ;
- B2 : tests codec V3, bornes et corruption CRC ;
- B3 : intégration CMake/cross-build ;
- B4 : gel.

Le store dense continue d'utiliser son record V2 pendant toute H3c3-B.

Le remplacement du store est reporté à H3c3-C.

## 7. Taille

```text
256 * 2 * 70 = 35 840 octets
```

hors métadonnées globales éventuelles.

L'objectif global < 40 KiB reste réaliste mais non encore démontré tant que le layout final du store n'est pas figé.
