\# FT-STR-06 — Fiche de spécification  

\## Accessibilité et découpage de lecture Modbus



---



\## 1. Identification



\- \*\*ID\*\* : FT-STR-06  

\- \*\*Nom\*\* : Accessibilité lecture  

\- \*\*Famille parente\*\* : FT-STR  

\- \*\*Criticité\*\* : P0 (bloquant)



---



\## 2. Objectif



Valider que l’interface Modbus du capteur est \*\*lisible de manière standard\*\*, sans contrainte implicite sur :



\- la taille des requêtes,

\- le découpage des lectures,

\- l’alignement des accès.



---



\## 3. Périmètre



\### Inclus



\- lecture registre unitaire

\- lecture multi-registres

\- lecture partielle de blocs

\- lecture complète de blocs

\- lecture chevauchant plusieurs champs

\- lecture aux frontières de blocs

\- comportement hors plage



\### Exclus



\- droits d’accès (FT-ACC)

\- performance (FT-PER)



---



\## 4. Références



\- Standard Modbus RTU (fonction 03)

\- Mapping blocs



---



\## 5. Règles de conformité



L’interface est conforme si :



\- toute plage valide est lisible

\- aucune taille de requête implicite n’est requise

\- les lectures partielles sont possibles

\- les frontières de blocs sont correctement gérées



---



\## 6. Préconditions



\- FT-STR-01 validée

\- FT-STR-02 validée

\- communication stable



---



\## 7. Résultats attendus



\- lecture possible de n’importe quel segment valide

\- aucun comportement erratique selon la taille

\- réponse cohérente



---



\## 8. Risques couverts



| Risque | Impact |

|---|---|

| dépendance taille requête | non interopérable |

| lecture partielle impossible | rigidité |

| erreur frontière | données incorrectes |

| lecture hors plage mal gérée | instabilité |



---



\## 9. Cas de figure



\### Nominal

\- lecture 1 registre

\- lecture N registres

\- lecture bloc complet



\### Limites

\- lecture fin de bloc

\- lecture début bloc

\- lecture frontière



\### Erreurs

\- lecture hors plage

\- lecture chevauchante invalide



\### Robustesse

\- variation tailles de requêtes

\- répétition



---



\## 10. Points critiques



\- dépendance implicite firmware

\- taille max non documentée

\- erreurs silencieuses



---



\## 11. Ambiguïtés



\- taille max supportée ?

\- comportement hors plage ?

\- lecture multi-blocs autorisée ?



---



\## 12. Critères de réussite



\- lecture possible sur toute plage valide

\- comportement cohérent

\- aucune dépendance implicite



---



\## 13. Échec



\- lecture refusée sans raison

\- incohérence selon taille

\- comportement non déterministe



---



\## 14. Anomalies



| Type | Exemple |

|---|---|

| BLOQUANTE | lecture impossible |

| MAJEURE | dépendance taille |

| MINEURE | limitation non critique |

| SPEC | comportement non défini |



---



\## 15. Dépendances



\### Amont

\- FT-STR-01 / 02



\### Aval

\- FT-ACC

\- FT-PER



---



\## 16. Ordre



1\. lectures unitaires

2\. lectures multi-registres

3\. lectures limites

4\. lectures hors plage

5\. robustesse



---



\## 17. Livrables



\- matrice lecture

\- traces Modbus



---



\## 18. Maturité



\- interface librement lisible

\- aucun comportement caché



---

