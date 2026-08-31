# FT-STR — Fiche de spécification  

## Conformité structurelle du protocole Modbus RTU



---



## 1. Identification



- **ID famille** : FT-STR  

- **Nom** : Conformité structurelle  

- **Type** : Validation protocolaire bas niveau  

- **Niveau de criticité** : **P0 (bloquant)**  

- **Dépendance** : aucune  

- **Pré-requis pour autres familles** : obligatoire



---



## 2. Objectif



Valider que l’interface Modbus RTU du capteur expose une **structure de registres conforme, stable et décodable**, indépendamment du sens métier des données.



Cette famille garantit que :



- la cartographie est correcte,

- les types sont respectés,

- les conventions d’encodage sont appliquées,

- les zones neutres sont propres,

- l’image registre est exploitable sans ambiguïté par la centrale.



---



## 3. Périmètre



### 3.1 Inclus



- Organisation en blocs (0 à 7)

- Adressage Modbus (offsets)

- Tailles de blocs

- Typage des champs :

&nbsp; - uint16

&nbsp; - uint32

&nbsp; - enum

&nbsp; - bitfield

&nbsp; - ASCII fixe

- Encodage :

&nbsp; - uint32 = MSW puis LSW

&nbsp; - ASCII fixe + padding 0x00

- Registres réservés

- Valeurs structurelles imposées (ex : ID = 0)

- Modes de lecture structurels

- Stabilité de lecture



### 3.2 Exclus



- Cohérence métier des valeurs

- Validité fonctionnelle des états

- Logique de commande

- Séquences d’usage

- Persistance après reboot

- Performance et charge



---



## 4. Références d’entrée



FT-STR s’appuie sur :



- Mapping Modbus officiel (tous blocs)

- Spécification protocole TR2

- Conventions validées :

&nbsp; - types autorisés

&nbsp; - endianness

&nbsp; - ASCII fixe

&nbsp; - registres réservés

- Règles sentinelles (partielles)

- Convention de repérage des réservés : tout champ logique réservé doit être nommé `reserved_*` dans le mapping unifié.



> ⚠️ Toute ambiguïté doit être remontée comme anomalie de spécification



---



## 5. Décomposition



### Sous-familles :



| ID | Nom |

|---|---|

| FT-STR-01 | Structure des plages |

| FT-STR-02 | Typage des champs |

| FT-STR-03 | Encodage multi-registres |

| FT-STR-04 | ASCII fixe |

| FT-STR-05 | Réservés et sentinelles |

| FT-STR-06 | Accessibilité lecture |

| FT-STR-07 | Stabilité d’image |

| FT-STR-08 | Conformité documentaire |



---



## 6. Règles générales de validation



Une implémentation est conforme si :



- tous les blocs sont présents et correctement positionnés,

- aucun champ ne viole les conventions de type,

- les encodages sont cohérents,

- les zones réservées sont neutres,

- l’interface est lisible sans hypothèse implicite,

- la documentation correspond à l’implémentation.

- pour tout champ multi-registres exposé comme valeur logique unique, l’image registre doit rester cohérente en lecture passive lorsque le capteur est en état stable.



---



## 7. Critères d’entrée



- Mapping Modbus gelé

- Bus opérationnel

- Outil Modbus validé

- Capteur dans un état stable

- Configuration minimale appliquée



---



## 8. Critères de sortie



FT-STR est validée si :



- 100% des sous-familles exécutées

- 0 anomalie bloquante

- 0 ambiguïté structurelle

- toutes les divergences documentées



Rejet si :



- erreur de mapping

- incohérence de type

- encodage incorrect

- ambiguïté empêchant le décodage



---



## 9. Résultats attendus



- image registre fiable

- protocole décodable sans hypothèse implicite

- intégration centrale possible sans contournement

- mapping validé comme référentiel



---



## 10. Stratégie de test



### 10.1 Approche



- lecture passive prioritaire

- comparaison mapping

- injection minimale

- tests en état figé



### 10.2 Philosophie



- reproductibilité

- isolation des erreurs structurelles

- pas de dépendance au dynamique



---



## 11. Risques couverts



| Risque | Impact |

|---|---|

| Mauvais adressage | Lecture incorrecte |

| Mauvaise taille de champ | Décodage corrompu |

| Endianness incorrecte | Données inutilisables |

| ASCII mal formé | Interface illisible |

| Réservés pollués | Ambiguïté |

| Lecture non standard | Intégration difficile |

| Instabilité | Non fiabilité |



---



## 12. Risques non couverts



| Risque | Famille |

|---|---|

| Cohérence métier | FT-BLK |

| Droits d’accès | FT-ACC |

| Commandes | FT-CMD |

| Séquences | FT-SEQ |

| Reboot | FT-RBT |

| Performance | FT-PER |



---



## 13. Points d’attention critiques



### Endianness

Toute inversion MSW/LSW = anomalie critique.



### Réservés

Doivent être strictement à 0.



### ASCII

Padding obligatoire en 0x00.



### Frontières

Pas de débordement implicite.



### uint32

Atomicité non garantie → prudence.


### Cohérence multi-registres

Pour tout champ multi-registres exposé comme valeur logique unique, l’image registre doit rester cohérente en lecture passive lorsque le capteur est en état stable.  
Toute reconstruction incohérente d’une valeur multi-registres constitue une anomalie structurelle.


---



## 14. Gestion des anomalies



| Type | Description |

|---|---|

| BLOQUANTE | empêche décodage |

| MAJEURE | contournement requis |

| MINEURE | non conforme mais exploitable |

| SPEC | ambiguïté de spécification |



> ⚠️ Les anomalies SPEC doivent être levées avant FT-BLK



---



## 15. Ordre d’exécution



1. FT-STR-08 — Documentation  

2. FT-STR-01 — Structure  

3. FT-STR-02 — Typage  

4. FT-STR-03 — Encodage  

5. FT-STR-04 — ASCII  

6. FT-STR-05 — Réservés  

7. FT-STR-06 — Lecture  

8. FT-STR-07 — Stabilité  



---



## 16. Dépendances aval



Requis pour :



- FT-ACC

- FT-BLK

- FT-INT

- FT-CMD

- FT-SEQ

- FT-RBT

- FT-PER



---



## 17. Critère de maturité



- aucune divergence mapping / implémentation

- aucune ambiguïté

- décodage possible par un tiers

- reproductibilité totale



---



## 18. Livrables



- rapport FT-STR

- liste anomalies

- mapping validé

- traces Modbus

- matrice conformité



---

