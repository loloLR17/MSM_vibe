# FT-STR-02 — Fiche de spécification

## Typage des champs

## 1. Identification

- **ID** : FT-STR-02
- **Nom** : Typage des champs
- **Famille parente** : FT-STR
- **Criticité** : P0

## 2. Objectif

Valider que chaque champ logique du protocole possède un type explicitement déclaré, autorisé par `charte_typage.md`, conforme à V1 et correctement dérivé dans GEL-MAP-V1.

FT-STR-02 vérifie également que la taille structurelle du champ est compatible avec son type.

## 3. Références d'entrée

Par ordre de priorité :

1. `bloc0.md` à `bloc7.md` ;
2. `charte_typage.md` ;
3. GEL-MAP-V1 ;
4. gouvernance gelée applicable.

## 4. Types autorisés

Types protocolaires strictement autorisés :

- `uint16` : 1 registre ;
- `int16` : 1 registre ;
- `uint32` : 2 registres ;
- `bitfield16` : 1 registre ;
- `enum16` : 1 registre ;
- `ASCII fixe` : nombre de registres fixé par la longueur normative du champ, à raison de 2 caractères par registre.

La notation `uint16[n]` peut apparaître comme regroupement documentaire. Elle signifie `n` registres consécutifs de type `uint16` et ne crée aucun type protocolaire nouveau.

## 5. Périmètre inclus

- présence d'un type explicite pour chaque champ logique ;
- appartenance du type à la liste autorisée ;
- conformité du type entre V1, charte et mapping ;
- compatibilité du nombre de registres avec le type déclaré ;
- détection de types historiques ou implicites (`enum`, `bitfield`, float, etc.) ;
- absence de conversion cachée ou d'interprétation heuristique dans les artefacts de validation.

## 6. Hors périmètre

- position et frontières des champs : FT-STR-01 ;
- ordre MSW/LSW et atomicité des `uint32` : FT-STR-03/07 selon le contrôle ;
- contenu et padding des `ASCII fixe` : FT-STR-04 ;
- validité des valeurs d'énumération et signification des bits : familles fonctionnelles concernées ;
- règles de réservés/sentinelles : FT-STR-05 ;
- comportement des lectures partielles ou décalées : FT-STR-06 ;
- droits d'accès : FT-ACC.

## 7. Invariants

### 7.1 Type déclaré

Chaque champ doit être interprété exclusivement selon son type normatif déclaré. Une lecture Modbus brute ne constitue pas une preuve du type.

### 7.2 Taille

- `uint16`, `int16`, `enum16`, `bitfield16` occupent exactement 1 registre ;
- `uint32` occupe exactement 2 registres ;
- `ASCII fixe` occupe exactement la longueur normative prévue ;
- `uint16[n]`, lorsqu'utilisé comme notation documentaire, correspond exactement à `n` registres `uint16`.

### 7.3 Types interdits ou hérités

Sont non conformes sans évolution formelle de la charte :

- `float`, `float32`, `float64` ;
- `enum` en lieu et place de `enum16` ;
- `bitfield` en lieu et place de `bitfield16` ;
- tout type implicite ou non documenté.

## 8. Cas génériques

- **GEN-001** — type déclaré autorisé et conforme à V1/charte ;
- **GEN-002** — taille structurelle compatible avec le type ;
- **GEN-003** — absence de type implicite, historique ou interdit.

## 9. Instanciation

Les contrôles GEN-001 et GEN-002 sont appliqués aux 183 champs logiques de GEL-MAP-V1. GEN-003 est contrôlé globalement sur l'ensemble du mapping et des artefacts actifs.

La vérification d'une fiche instanciée est documentaire/structurelle : elle compare les références normatives et dérivées. Elle ne prétend pas identifier le type en observant uniquement les bits retournés par le serveur Modbus.

## 10. Critères de réussite

- 100 % des champs ont un type explicite autorisé ;
- 100 % des tailles sont compatibles avec le type déclaré ;
- aucun type implicite, interdit ou historique n'est actif ;
- les 183 instanciations sont traçables à GEL-MAP-V1 et aux génériques.

## 11. Critères d'échec

- type absent ou ambigu ;
- type différent entre V1 et mapping ;
- type hors charte ;
- taille incompatible avec le type ;
- interprétation heuristique utilisée comme substitut à la déclaration normative.

## 12. Dépendances

### Amont

- FT-STR-08 ;
- FT-STR-01 ;
- V1, charte de typage et GEL-MAP-V1.

### Aval

- FT-STR-03 ;
- FT-STR-04 ;
- familles fonctionnelles qui décodent les champs.
