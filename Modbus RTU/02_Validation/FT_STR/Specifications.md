# FT-STR — Fiche de spécification

## Conformité structurelle du protocole Modbus RTU

---

## 1. Identification

- **ID famille** : FT-STR
- **Nom** : Conformité structurelle
- **Type** : Validation protocolaire bas niveau
- **Niveau de criticité** : **P0 (bloquant)**
- **Dépendance inter-familles** : aucune
- **Entrées de référence obligatoires** : spécification Modbus RTU V1 gelée et mapping unifié dérivé
- **Pré-requis pour autres familles** : obligatoire

---

## 2. Objectif

Valider que l’interface Modbus RTU du capteur expose une **structure de registres conforme, stable et décodable**, indépendamment du sens métier des données.

Cette famille garantit que :

- la cartographie est correcte ;
- les types sont respectés ;
- les conventions d’encodage sont appliquées ;
- les zones neutres sont propres ;
- l’image registre est exploitable sans ambiguïté par la centrale.

---

## 3. Périmètre

### 3.1 Inclus

- organisation en blocs 0 à 7 ;
- adressage Modbus ;
- tailles de blocs ;
- typage des champs selon la charte V1 :
  - `uint16` ;
  - `int16` ;
  - `uint32` ;
  - `bitfield16` ;
  - `enum16` ;
  - `ASCII fixe` ;
- encodage :
  - `uint32` = MSW puis LSW ;
  - ASCII fixe avec padding `0x00` ;
- registres réservés ;
- sentinelles uniquement lorsqu’elles sont explicitement définies par la spécification normative ;
- accessibilité en lecture des plages exposées ;
- validité des lectures portant sur des sous-plages valides, y compris lorsqu’elles sont partielles ;
- stabilité et cohérence des lectures.

### 3.2 Exclus

- cohérence métier des valeurs ;
- validité fonctionnelle des états ;
- logique de commande ;
- séquences d’usage ;
- validation des droits d’écriture et des refus d’accès en écriture, couverte par FT-ACC ;
- persistance après reboot ;
- performances physiques ou temporelles hors exigences explicitement spécifiées.

---

## 4. Références d’entrée et hiérarchie

FT-STR s’appuie sur la hiérarchie documentaire suivante :

1. **spécification Modbus RTU V1 gelée** : `bloc0.md` à `bloc7.md` et `charte_typage.md` ;
2. **mapping unifié**, artefact dérivé de la spécification ;
3. fiches source de validation ;
4. tests détaillés ;
5. tests instanciés.

Le mapping unifié est la source opérationnelle d’instanciation des tests, mais il ne constitue pas une norme indépendante. En cas de divergence avec la spécification V1, la spécification fait foi et la divergence doit être remontée comme anomalie documentaire.

Convention de repérage : tout champ logique réservé doit être nommé avec le préfixe `reserved_` dans le mapping unifié.

Toute ambiguïté doit être remontée comme anomalie de spécification ; aucune règle implicite ne doit être inventée par FT-STR.

---

## 5. Décomposition

| ID | Nom |
|---|---|
| FT-STR-01 | Structure des plages |
| FT-STR-02 | Typage des champs |
| FT-STR-03 | Encodage multi-registres |
| FT-STR-04 | ASCII fixe |
| FT-STR-05 | Réservés et sentinelles |
| FT-STR-06 | Accessibilité et découpage de lecture Modbus |
| FT-STR-07 | Stabilité d’image |
| FT-STR-08 | Conformité documentaire |

La numérotation ci-dessus est gelée pour la V1.

---

## 6. Règles générales de validation

Une implémentation est conforme si :

- tous les blocs sont présents et correctement positionnés ;
- aucun champ ne viole les conventions de type ;
- les encodages sont cohérents ;
- les zones réservées respectent les exigences normatives ;
- l’interface est lisible sans hypothèse implicite ;
- la documentation correspond à l’implémentation ;
- les champs multi-registres et les réponses multi-registres respectent la cohérence d’un même instant logique exigée par `charte_typage.md`.

---

## 7. Critères d’entrée

- spécification Modbus RTU V1 gelée ;
- mapping unifié dérivé disponible ;
- outil Modbus validé ;
- moyen de test opérationnel ;
- capteur ou simulateur placé dans un état compatible avec le scénario exécuté.

---

## 8. Critères de sortie

FT-STR est validée si :

- 100 % des sous-familles applicables sont exécutées ;
- aucune anomalie bloquante ne subsiste ;
- aucune ambiguïté structurelle ne subsiste ;
- toutes les divergences sont documentées.

Rejet notamment en cas de :

- divergence entre mapping et spécification non résolue ;
- incohérence de type ;
- encodage incorrect ;
- ambiguïté empêchant le décodage ;
- incohérence d’une valeur multi-registres.

---

## 9. Résultats attendus

- image registre fiable ;
- protocole décodable sans hypothèse implicite ;
- intégration centrale possible sans contournement ;
- mapping unifié validé comme **artefact dérivé conforme à la spécification V1**.

---

## 10. Stratégie de test

### 10.1 Approche

- lecture passive prioritaire ;
- comparaison avec le mapping dérivé et la spécification normative ;
- injection minimale lorsque nécessaire ;
- conditions de test maîtrisées et reproductibles.

### 10.2 Philosophie

- reproductibilité ;
- traçabilité ;
- isolation des erreurs structurelles ;
- absence d’hypothèse implicite.

---

## 11. Risques couverts

| Risque | Impact |
|---|---|
| Mauvais adressage | Lecture incorrecte |
| Mauvaise taille de champ | Décodage corrompu |
| Endianness incorrecte | Données inutilisables |
| ASCII mal formé | Interface illisible |
| Réservés pollués | Ambiguïté |
| Découpage de lecture non conforme | Intégration difficile |
| Incohérence multi-registres | Valeur reconstruite invalide |
| Instabilité d’image | Non-fiabilité |

---

## 12. Risques non couverts

| Risque | Famille |
|---|---|
| Cohérence métier | FT-BLK |
| Droits d’accès et refus d’écriture | FT-ACC |
| Valeurs limites / domaines invalides | FT-LIM |
| Commandes | FT-CMD |
| Séquences | FT-SEQ |
| Robustesse protocolaire | FT-RBT |
| Reboot / persistance | FT-PER |
| Performance non spécifiée | **NON DÉFINI / hors périmètre actuel** |

---

## 13. Points d’attention critiques

### Endianness
Toute inversion MSW/LSW constitue une anomalie critique.

### Réservés
Les registres réservés doivent respecter la valeur et les règles structurelles définies par la spécification V1. La validation du refus d’écriture sur un registre réservé relève de FT-ACC. Lorsqu’un réservé est normativement défini à zéro, toute valeur lue non nulle est non conforme.

### ASCII
Le padding `0x00` est obligatoire pour les chaînes ASCII fixes conformément à la charte de typage.

### Frontières
Aucun débordement implicite de plage n’est autorisé. Une lecture portant sur un sous-ensemble valide n’est toutefois pas invalide du seul fait qu’elle est partielle.

### `uint32` et champs multi-registres
MSW et LSW doivent provenir du **même instant logique**. Plus généralement, une réponse Modbus multi-registres doit respecter les règles globales de cohérence définies dans `charte_typage.md`.

Toute reconstruction incohérente d’une valeur multi-registres constitue une anomalie structurelle.

---

## 14. Gestion des anomalies

| Type | Description |
|---|---|
| BLOQUANTE | empêche le décodage ou viole une exigence structurelle critique |
| MAJEURE | contournement requis |
| MINEURE | non conforme mais exploitable |
| SPEC | ambiguïté ou divergence de spécification |

Les anomalies SPEC doivent être levées avant les familles aval qui dépendent de la règle concernée.

---

## 15. Ordre d’exécution

1. FT-STR-08 — Documentation
2. FT-STR-01 — Structure
3. FT-STR-02 — Typage
4. FT-STR-03 — Encodage
5. FT-STR-04 — ASCII
6. FT-STR-05 — Réservés et sentinelles
7. FT-STR-06 — Accessibilité et découpage de lecture Modbus
8. FT-STR-07 — Stabilité

---

## 16. Dépendances aval

FT-STR est un prérequis pour :

- FT-ACC ;
- FT-BLK ;
- FT-INT ;
- FT-LIM ;
- FT-CMD ;
- FT-SEQ ;
- FT-RBT ;
- FT-PER.

---

## 17. Critère de maturité

- aucune divergence non résolue entre spécification, mapping et implémentation ;
- aucune ambiguïté structurelle ;
- décodage possible par un tiers ;
- reproductibilité totale ;
- traçabilité des anomalies et arbitrages.

---

## 18. Livrables

- rapport FT-STR ;
- liste des anomalies ;
- mapping unifié validé comme dérivé conforme ;
- traces Modbus ;
- matrice de conformité.
