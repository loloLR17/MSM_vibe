# FT-STR-04 — Encodage des chaînes ASCII fixes

## 1. Identification

- **ID** : FT-STR-04
- **Nom** : ASCII fixe
- **Famille parente** : FT-STR
- **Criticité** : P1

## 2. Objectif

Valider que tout champ déclaré `ASCII fixe` respecte sans ambiguïté la convention V1 de représentation des caractères dans les registres Modbus.

## 3. Références normatives

- `Modbus RTU/01_Specification_source/charte_typage.md`
- blocs V1 contenant des champs ASCII fixes
- GEL-MAP-V1 comme dérivé opérationnel de V1

La règle V1 est :

- 2 caractères par registre ;
- caractère 1 → octet haut ;
- caractère 2 → octet bas ;
- `"AB"` → `0x4142` ;
- caractère final isolé → octet haut, octet bas `0x00` ;
- octets inutilisés en fin de champ → `0x00` ;
- ASCII uniquement ;
- longueur fixe obligatoire.

## 4. Périmètre inclus

FT-STR-04 couvre :

- identification exhaustive des champs `ASCII fixe` dans GEL-MAP-V1 ;
- capacité du champ : 2 caractères par registre ;
- ordre des deux caractères dans chaque registre ;
- reconstruction déterministe de la séquence d’octets ;
- padding terminal `0x00` ;
- rejet d’un encodage non ASCII dans un scénario d’essai contrôlé ;
- cas chaîne vide, chaîne partielle et chaîne occupant toute la capacité.

## 5. Hors périmètre

Ne sont pas validés ici :

- la sémantique métier du texte ;
- les règles métier de caractères autorisés au-delà de la contrainte ASCII ;
- la géométrie globale des blocs (FT-STR-01) ;
- le typage général (FT-STR-02) ;
- les droits d’accès ou lectures partielles (FT-STR-06 / FT-ACC) ;
- la stabilité temporelle des lectures (FT-STR-07).

## 6. Règles de conformité

Un champ ASCII fixe est structurellement conforme si :

1. son type déclaré est `ASCII fixe` ;
2. son nombre de registres est strictement positif ;
3. sa plage d’adresses contient exactement ce nombre de registres ;
4. sa capacité est exactement `2 × nombre_de_registres` caractères/octet ASCII ;
5. aucun artefact dérivé n’introduit un ordre d’octets différent de V1.

L’implémentation est conforme à l’encodage si, sur des valeurs d’essai connues :

1. chaque paire de caractères est encodée dans l’ordre octet haut puis octet bas ;
2. les octets inutilisés en fin de champ sont `0x00` ;
3. aucun octet de donnée ne sort de la plage ASCII 7 bits ;
4. aucune heuristique de décodage, inversion ou trim implicite ne remplace la convention normative.

## 7. Tests génériques

- `TT-STR-04-GEN-001` — structure et capacité ASCII fixe ;
- `TT-STR-04-GEN-002` — ordre des octets et reconstruction ;
- `TT-STR-04-GEN-003` — padding terminal et ASCII strict.

## 8. Instanciation

L’instanciation active est dérivée directement de GEL-MAP-V1. Les anciennes fiches champ-par-champ ne sont pas une source de vérité et sont archivées.

Le validateur mécanique contrôle les invariants structurels du mapping ; les vecteurs d’encodage et de padding restent des tests d’implémentation à exécuter sur banc, simulation ou firmware.

## 9. Critère de clôture

FT-STR-04 est prête pour validation terrain lorsque :

- tous les champs ASCII GEL-MAP-V1 sont couverts une fois ;
- les invariants structurels sont conformes ;
- les tests génériques sont définis sans ambiguïté ;
- aucun artefact historique n’est actif.
