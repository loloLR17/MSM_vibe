\# FT-STR-02 — Fiche de spécification  

\## Typage Modbus des champs



---



\## 1. Identification



\- \*\*ID\*\* : FT-STR-02  

\- \*\*Nom\*\* : Typage des champs  

\- \*\*Famille parente\*\* : FT-STR  

\- \*\*Criticité\*\* : P0 (bloquant)



---



\## 2. Objectif



Valider que chaque champ exposé dans les blocs Modbus :



\- occupe le bon nombre de registres,

\- respecte le type attendu,

\- est correctement aligné,

\- n’empiète pas sur les champs adjacents.



Cette sous-famille garantit que la centrale peut \*\*interpréter correctement la structure binaire des données\*\*.



---



\## 3. Périmètre



\### Inclus



\- Correspondance type ↔ nombre de registres :

&nbsp; - `uint16` → 1 registre

&nbsp; - `uint32` → 2 registres

&nbsp; - `enum` → 1 registre

&nbsp; - `bitfield` → 1 registre

&nbsp; - `ASCII fixe` → N registres

\- Alignement des champs

\- Ordonnancement des champs dans un bloc

\- Absence de chevauchement entre champs

\- Continuité des champs multi-registres



\### Exclus



\- Valeurs métier

\- Validité des enums

\- Interprétation des bitfields

\- Encodage interne des types (traité en FT-STR-03 et FT-STR-04)

\- Droits d’accès



---



\## 4. Références d’entrée



\- Mapping Modbus détaillé (registre par registre)

\- Définition des types autorisés

\- Spécification des champs par bloc



⚠️ Toute incohérence doc ↔ implémentation = anomalie



---



\## 5. Règles de conformité



Un champ est conforme si :



\- son type correspond à la spécification

\- sa taille (en registres) est correcte

\- il commence à la bonne adresse

\- il se termine à la bonne adresse

\- il n’empiète pas sur le champ suivant



Un bloc est conforme si :



\- tous les champs respectent leur typage

\- aucun décalage cumulatif n’est observé



---



\## 6. Préconditions



\- FT-STR-01 validée

\- Mapping stabilisé

\- Accès Modbus opérationnel

\- Capteur en état stable



---



\## 7. Résultats attendus



\- chaque champ est identifiable avec précision

\- les tailles correspondent exactement

\- aucun décalage structurel n’est détecté

\- le parsing est possible sans heuristique



---



\## 8. Risques couverts



| Risque | Impact |

|---|---|

| Mauvaise taille de champ | Décalage global |

| Mauvais type | Mauvaise interprétation |

| Chevauchement | Données corrompues |

| Champ tronqué | Valeur inutilisable |

| Mauvais alignement | Parsing instable |



---



\## 9. Cas de figure à couvrir



\### 9.1 Cas nominaux



\- Lecture complète d’un champ `uint16`

\- Lecture complète d’un champ `uint32`

\- Lecture complète d’un champ ASCII

\- Lecture d’un champ enum / bitfield



---



\### 9.2 Cas limites



\- Lecture du premier registre d’un champ

\- Lecture du dernier registre d’un champ

\- Lecture partielle d’un champ multi-registre

\- Lecture à cheval entre deux champs



---



\### 9.3 Cas d’erreur



\- Champ supposé sur 2 registres mais accessible sur 1

\- Champ débordant sur le suivant

\- Mauvaise taille effective



---



\### 9.4 Cas de robustesse



\- Lecture répétée d’un champ

\- Lecture avec tailles variables

\- Lecture décalée (offset incorrect volontaire)



---



\## 10. Points d’attention critiques



\### 10.1 Effet domino

Une erreur sur un champ entraîne :

→ décalage de tous les suivants



---



\### 10.2 Faux positifs

Un champ peut sembler correct :

→ mais être mal aligné si le précédent est faux



---



\### 10.3 Champs multi-registres

Doivent être :

\- contigus

\- sans trou

\- sans interférence



---



\### 10.4 ASCII mal dimensionné

Très fréquent :

\- longueur incorrecte

\- débordement sur champ suivant



---



\### 10.5 Champs implicites

Certains firmwares ajoutent :

\- padding non documenté

→ interdit



---



\## 11. Ambiguïtés à lever



\- Alignement obligatoire sur 2 registres ?

\- Padding autorisé ou interdit ?

\- Ordre strict des champs garanti ?

\- Champs optionnels possibles ?



---



\## 12. Critères de réussite



FT-STR-02 est validée si :



\- 100% des champs respectent leur typage

\- aucune dérive d’adresse

\- aucun chevauchement

\- aucun champ ambigu



---



\## 13. Critères d’échec



FT-STR-02 échoue si :



\- un champ a une taille incorrecte

\- un champ est mal positionné

\- un champ empiète sur un autre

\- un champ est non décodable



---



\## 14. Classification des anomalies



| Type | Exemple |

|---|---|

| BLOQUANTE | décalage global |

| MAJEURE | champ mal dimensionné |

| MINEURE | alignement non critique |

| SPEC | doc incohérente |



---



\## 15. Dépendances



\### Amont

\- FT-STR-01



\### Aval

\- FT-STR-03 (endianness)

\- FT-STR-04 (ASCII)

\- FT-BLK



---



\## 16. Ordre d’exécution interne



1\. Vérification du mapping champ par champ

2\. Vérification des tailles

3\. Vérification des positions

4\. Vérification des frontières

5\. Détection des chevauchements

6\. Tests de lecture partielle



---



\## 17. Livrables



\- tableau des champs validés

\- liste anomalies

\- mapping réel observé

\- traces Modbus



---



\## 18. Critère de maturité



FT-STR-02 est mature si :



\- parsing possible sans ambiguïté

\- aucun effet domino détecté

\- structure robuste et stable

\- mapping fiable pour implémentation tiers



---

