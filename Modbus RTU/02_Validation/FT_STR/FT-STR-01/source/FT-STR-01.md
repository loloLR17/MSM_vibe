\# FT-STR-01 — Fiche de spécification  

\## Structure des plages et cartographie Modbus



---



\## 1. Identification



\- \*\*ID\*\* : FT-STR-01  

\- \*\*Nom\*\* : Structure des plages  

\- \*\*Famille parente\*\* : FT-STR  

\- \*\*Criticité\*\* : P0 (bloquant)



---



\## 2. Objectif



Valider que la \*\*cartographie Modbus globale\*\* du capteur est conforme à la spécification :



\- les blocs existent,

\- leurs adresses sont correctes,

\- leurs tailles sont respectées,

\- aucune dérive ou collision n’existe.



Cette sous-famille garantit que la centrale lit \*\*les bons registres au bon endroit\*\*.



---



\## 3. Périmètre



\### Inclus



\- Présence des blocs 0 à 7

\- Adresse de début de chaque bloc

\- Taille de chaque bloc (nombre de registres)

\- Continuité ou discontinuité conforme

\- Absence de recouvrement entre blocs

\- Accessibilité des plages

\- Détection des trous non spécifiés



\### Exclus



\- Typage des champs internes

\- Valeurs des registres

\- Encodage des données

\- Droits d’accès (RO/RW)

\- Cohérence métier



---



\## 4. Références d’entrée



\- Mapping Modbus officiel (version figée)

\- Définition des blocs 0 à 7

\- Table d’adressage complète



⚠️ Toute incohérence documentaire doit être signalée (pas corrigée implicitement)



---



\## 5. Règles de conformité



Un bloc est conforme si :



\- son adresse de début correspond exactement à la spécification

\- sa longueur correspond exactement à la spécification

\- il est entièrement accessible en lecture

\- aucun autre bloc n’empiète sur sa plage



La cartographie globale est conforme si :



\- tous les blocs sont présents

\- aucun recouvrement non spécifié

\- aucun trou interdit

\- toutes les plages sont cohérentes



---



\## 6. Préconditions



\- Capteur alimenté et opérationnel

\- Communication Modbus fonctionnelle

\- Aucun mode dégradé actif

\- Capteur dans un état stable

\- Mapping de référence disponible



---



\## 7. Résultats attendus



À l’issue des tests :



\- chaque bloc est localisable précisément

\- la taille réelle correspond à la taille attendue

\- aucune dérive d’adresse n’est détectée

\- la centrale peut adresser chaque bloc sans ambiguïté



---



\## 8. Risques couverts



| Risque | Impact |

|---|---|

| Décalage d’adresse | Lecture erronée |

| Mauvaise taille de bloc | Mauvais parsing |

| Chevauchement de blocs | Données corrompues |

| Bloc absent | Fonction indisponible |

| Trou non documenté | Comportement imprévisible |



---



\## 9. Cas de figure à couvrir (logique de test)



\### 9.1 Cas nominaux



\- Lecture du premier registre de chaque bloc

\- Lecture du dernier registre de chaque bloc

\- Lecture complète du bloc (si taille raisonnable)



---



\### 9.2 Cas limites



\- Lecture à la frontière entre deux blocs

\- Lecture incluant le dernier registre d’un bloc

\- Lecture démarrant juste après un bloc

\- Lecture d’un bloc de taille minimale (si existant)



---



\### 9.3 Cas d’erreur



\- Lecture dans une zone non définie

\- Lecture chevauchant deux blocs non contigus

\- Lecture hors plage globale



---



\### 9.4 Cas de robustesse structurelle



\- Lecture répétée des mêmes adresses

\- Lecture avec tailles variables

\- Lecture multi-blocs (si autorisée implicitement)



---



\## 10. Points d’attention critiques



\### 10.1 Off-by-one

Erreur classique :

\- bloc déclaré sur N registres

\- implémenté sur N±1



---



\### 10.2 Alignement implicite

Certains firmwares imposent :

\- alignement pair,

\- padding non documenté



→ Doit être détecté



---



\### 10.3 Chevauchement silencieux

Deux blocs peuvent :

\- partager un registre sans que ce soit visible immédiatement



→ critique



---



\### 10.4 Zones non documentées

Présence possible de :

\- registres “fantômes”

\- zones mémoire exposées accidentellement



---



\### 10.5 Dépendance taille de requête

Certains capteurs :

\- fonctionnent uniquement avec certaines tailles de lecture



→ non acceptable



---



\## 11. Ambiguïtés à lever



\- Les trous entre blocs sont-ils autorisés ?

\- Lecture multi-blocs autorisée ou non ?

\- Taille max de requête supportée ?

\- Comportement attendu hors plage (exception vs silence) ?



Ces points doivent être :

\- soit spécifiés,

\- soit classés en anomalie SPEC



---



\## 12. Critères de réussite



FT-STR-01 est validée si :



\- 100% des blocs sont conformes

\- aucune collision détectée

\- aucune dérive d’adresse

\- aucune ambiguïté structurelle



---



\## 13. Critères d’échec



FT-STR-01 échoue si :



\- un bloc est absent

\- un bloc est mal positionné

\- un bloc a une mauvaise taille

\- deux blocs se chevauchent

\- une zone critique est inaccessible



---



\## 14. Classification des anomalies



| Type | Exemple |

|---|---|

| BLOQUANTE | bloc absent / décalé |

| MAJEURE | taille incorrecte |

| MINEURE | trou non critique |

| SPEC | doc ambiguë |



---



\## 15. Dépendances



\### Amont

\- FT-STR-08 (mapping validé)



\### Aval

\- FT-STR-02 (typage)

\- FT-STR-06 (lecture)



---



\## 16. Ordre d’exécution interne



1\. Vérification documentaire (mapping)

2\. Détection des blocs

3\. Vérification des adresses de début

4\. Vérification des tailles

5\. Vérification des frontières

6\. Tests hors plage

7\. Tests de robustesse lecture



---



\## 17. Livrables



\- tableau de validation des blocs

\- liste des anomalies

\- cartographie réelle observée

\- traces Modbus associées



---



\## 18. Critère de maturité



FT-STR-01 est mature si :



\- cartographie parfaitement alignée avec la doc

\- aucune hypothèse implicite nécessaire

\- comportement hors plage maîtrisé

\- structure stable et reproductible



---

